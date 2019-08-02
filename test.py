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
				print ("I'm running...and I'll never end!")
				sleep(5)
		except Exception as e:
			raise e
		finally:
			self.stopped = true

	def end(self):
		if self.is_alive():
			threader.killThread(self.ident)
			self.stopped = True
			
runMe = AThread()
runMe.start()
sleep(10)
runMe.end()
print("Ended.")
# We'll wait here to check that the thread has indeed ended.
# No more messages should print after this line.
sleep(20)