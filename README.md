The threader.c file contains a module that will let you kill a thread from anywhere in its execution.

#### Python-Kill-Thread-Extension
This is a Python module written in C that lets a Python developer kill a thread.

##### The Problem
While Python provides a powerful threading interface, there is no way (externally) to kill a thread. A proposed solution to get around Python's lack of kill thread functionality has been to set a flag in the thread's code that will cleanly exit the thread. However, for many developers this is not sufficient. What if the thread code will never reach the flag check (for instance, if you're running an external process?)

##### The Solution
This is the exact situation I was faced with at work one day. So when my Stack Overflow searches turned up hundreds of hits on people asking how to kill Python threads but with no solutions, I decided to write my own.

The `threader.c` and `threaderPy3.c` files are modules that will allow you to kill a thread from anywhere in its execution. If you are running Python 2.x, use `threader.c`. Python 3 users should use `threaderPy3.c`.

##### How to use
Whether you are running `threader.c` or `threaderPy3.c`, the build instructions are the same. The only thing you need to do with `threaderPy3.c` is rename it to `threader.c` so `setup.py` can find it.

First, you need to build the threader.c module. You can use the supplied setup.py file, or make your own.
`python setup.py build`

Next, install the threader module:
`sudo python setup.py install`

###### Example Usage 1
In your Python code, if you subclass the threading.Thread class, create a method called `end`. It will look like this:
```python
import threader
import threading
class AThread(threading.Thread):
	def run(self):
		from time import sleep
		while True:
			print "I'm running...and I'll never end!"
			sleep(5)

	def end(self):
		if self.is_alive():
			threader.killThread(self.ident)
```

Now, you can kill this thread anytime, like this:
```python
runMe = AThread()
runMe.start()
sleep(10)
runMe.end()
```

###### Example Usage 2
The problem with the code above is that Python will never know that the thread has ended, because we're not using Python's functionality to kill it. So, we need to provide a way to tell Python that this thread is no longer running. We can do this by overriding the `is_alive` method in the `threading.Thread` class. We'll also have to handle the case if an exception occurs during execution.
```python
import threader
import threading
class AThread(threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self)
		self.stopped = False # This is the flag we'll use to signal thread termination.

	def is_alive(self):
		return not self.stopped

	def run(self):
		try:
			from time import sleep
			while True:
				print "I'm running...and I'll never end!"
				sleep(5)
		except Exception as e:
			throw e
		finally:
			self.stopped = true

	def end(self):
		if self.is_alive():
			threader.killThread(self.ident)
			self.stopped = True
```

##### Explanation
Notice we give the ident property to the killThread function. The ident property on the threading.Thread class is a unique identifier for the particular thread you are running. By passing it to the C module, we can kill the thread by using C's low-level thread functionality. These identifiers CAN be recycled, but no two running threads will have the same identifier.

Since we bypass Python's implementation for a thread, the task we have to be careful with is telling Python accurately when the thread has finished execution. We do this by setting a flag called `stopped`. This variable is `True` when the thread is not running, and `False` when the thread is running. So, in our second implementation of code, we set `stopped` to `True` after calling `threader.killThread(self.ident)`. Now, when Python calls `is_alive`, it will get `False`, signaling that the thread has terminated. Without overriding `is_alive`, the thread will die, but if you have code that waits for the thread to terminate, that code will block forever.
