# Python-Kill-Thread-Extension
This is a Python module written in C that lets a Python developer kill a thread. While Python provides a powerful threading interface, there is no way (externally) to kill a thread. The threader.c file contains a module that will let you kill a thread from anywhere in its execution. A proposed solution to get around Python's lack of kill thread functionality has been to set a flag in the thread's code that will cleanly exit the thread. However, for many developers this is not sufficient enough. What if the thread code will never reach the flag check (for instance, if you're running an external process?)

This is the exact situation I was faced with at work one day. So when my Stack Overflow searches turned up hundreds of hits on people asking how to kill Python threads but with no solutions, I decided to write my own solution.

## How to use
First, you need to build the threader.c module. You can use the supplied setup.py file, or make your own.
`python setup.py build`

Next, install the threader module:
`sudo python setup.py install`

In your Python code, if you subclass the threading.Thread class, create a method called end. It will look like this:
```python
import threader
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

That's all there is to it.

## Explanation
Notice we give the ident property to the killThread function. The ident property on the threading.Thread class is a unique identifier for the particular thread you are running. By passing it to the C module, we can kill the thread by using C's low-level thread functionality. These identifiers CAN be recycled, but no two running threads will have the same identifier.
