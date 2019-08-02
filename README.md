# Python-Kill-Thread-Extension
This is a Python module written in C that lets a Python developer kill a thread.

## The Problem
While Python provides a powerful threading interface, there is no way (externally) to kill a thread. A proposed solution to get around Python's lack of kill thread functionality has been to set a flag in the thread's code that will cleanly exit the thread. However, for many developers this is not sufficient. What if the thread code will never reach the flag check (for instance, if you're running an external process?)

## The Solution
This is the exact situation I was faced with at work one day. So when my Stack Overflow searches turned up hundreds of hits on people asking how to kill Python threads but with no solutions, I decided to write my own.

The `threader.c`,  `threaderPy3.c`, and `threaderPy3windows.c` files are modules that will allow you to kill a thread from anywhere in its execution. If you are running Python 2.x, use `threader.c`. No Windows support is available for Python 2. Python 3 users should use `threaderPy3.c` or `threaderPy3windows.c`. The former file is for Linux and the latter is for Windows.

### Windows Warning
The `threaderPy3windows.c` file uses `TerminateThread` to kill a thread. On Windows, this could result in important processes being left in undefined states. From Microsoft:
> TerminateThread is a dangerous function that should only be used in the most extreme cases. You should call TerminateThread only if you know exactly what the target thread is doing, and you control all of the code that the target thread could possibly be running at the time of the termination. For example, TerminateThread can result in the following problems:
> * If the target thread owns a critical section, the critical section will not be released.
> * If the target thread is allocating memory from the heap, the heap lock will not be released.
> * If the target thread is executing certain kernel32 calls when it is terminated, the kernel32 state for the thread's process could be inconsistent.
> * If the target thread is manipulating the global state of a shared DLL, the state of the DLL could be destroyed, affecting other users of the DLL.

So, be aware that `TerminateThread` doesn't provide the same safeguards as `pthread_cancel`.

## How to use
Whether you are running `threader.c` or `threaderPy3.c`, the build instructions are the same. The only thing you need to do with `threaderPy3.c` is rename it to `threader.c` so `setup.py` can find it.

First, you need to build the threader.c module. You can use the supplied setup.py file, or make your own.
```
git clone https://github.com/munawarb/Python-Kill-Thread-Extension.git
cd Python-Kill-Thread-Extension
```
Then run the setup script:
For Python 2:
```
python setup.py build
sudo python setup.py install
```
For Python 3:
```
python3 setup.py build
sudo python3 setup.py install
```

On Windows, use:
```
python setup.py build
python setup.py install
```

### Test Your Installation
It's a good idea to test that the module installed successfully before you start doing thread-heavy work with it. A file is provided called `test.py` that will do this. It will run a thread and then kill it. Once the thread is killed, the program will wait twenty seconds. If you see no more output from the thread, the thread has been successfully destroyed.

Here is sample output from the test script:
```
$ python3 test.py                             
I'm running...and I'll never end!                                                                                       
I'm running...and I'll never end!                                                                                       
Ended thread 807601920, with return code 0                                                                              
Ended.                                                                                                                  
 $                                             
```
Notice that after the word "Ended," there is no more output from the test program. If the thread continues printing messages, it wasn't destroyed.

## Example Usage 1
In your Python code, if you subclass the threading.Thread class, create a method called `end`. It will look like this:
```python
import threader
import threading
from time import sleep

class AThread(threading.Thread):
	def run(self):
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

## Example Usage 2
The problem with the code above is that Python will never know that the thread has ended, because we're not using Python's functionality to kill it. So, we need to provide a way to tell Python that this thread is no longer running. We can do this by overriding the `is_alive` method in the `threading.Thread` class. We'll also have to handle the case if an exception occurs during execution.
```python
import threader
import threading
from time import sleep

class AThread(threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self)
		self.stopped = False # This is the flag we'll use to signal thread termination.

	def is_alive(self):
		return not self.stopped

	def run(self):
		try:
			while True:
				print "I'm running...and I'll never end!"
				sleep(5)
		except Exception as e:
			raise e
		finally:
			self.stopped = true

	def end(self):
		if self.is_alive():
			threader.killThread(self.ident)
			self.stopped = True
```

### Explanation
We give the ident property to the killThread function. The ident property on the threading.Thread class is a unique identifier for the particular thread you are running. By passing it to the C module, we can kill the thread by using C's low-level thread functionality. These identifiers CAN be recycled, but no two running threads will have the same identifier. So, once you kill a thread, consider the identifier for the thread to be "dirty" and don't reuse it.

Since we bypass Python's implementation for a thread, the task we have to be careful with is telling Python accurately when the thread has finished execution. We do this by setting a flag called `stopped`. This variable is `True` when the thread is not running, and `False` when the thread is running. So, in our second implementation of code, we set `stopped` to `True` after calling `threader.killThread(self.ident)`. Now, when Python calls `is_alive`, it will get `False`, signaling that the thread has terminated. Without overriding `is_alive`, the thread will die, but if you have code that waits for the thread to terminate, that code will block forever.
