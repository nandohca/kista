

def split_list(alist, wanted_parts=1):
	length = len(alist)
	return [ alist[i*length // wanted_parts: (i+1)*length // wanted_parts] 
		for i in range(wanted_parts) ]


 
 #!/usr/bin/python3

import threading
import time

class myThread (threading.Thread):
	def __init__(self, threadID, name, counter,dest):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.name = name
		self.counter = counter
		self.A = [0,1,2,3,4,5,6,7,8,9]
		self.dest = dest
	def run(self):
		print ("Starting " + self.name)
        # Get lock to synchronize threads
        #threadLock.acquire()
		print_time(self.name, self.counter, 3, self.dest,self.A)
        # Free lock to release next thread
        #threadLock.release()

def print_time(threadName, delay, counter, dest,A):
	while counter:
		time.sleep(delay)
		print ("%s: %s" % (threadName, time.ctime(time.time())))
		counter -= 1
		print (split_list(A, wanted_parts=6))
		print (split_list(A, wanted_parts=3))
		print (split_list(A, wanted_parts=4))
		dest.append(counter)


#threadLock = threading.Lock()
threads = []
dest = []
# Create new threads
thread1 = myThread(1, "Thread-1", 1,dest)
thread2 = myThread(2, "Thread-2", 2,dest)

# Start new Threads
thread1.start()
thread2.start()

# Add threads to thread list
threads.append(thread1)
threads.append(thread2)

# Wait for all threads to complete
for t in threads:
	t.join()
print (dest)
print ("Exiting Main Thread")