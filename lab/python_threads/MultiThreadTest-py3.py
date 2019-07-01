import random
from threading import Thread
from multiprocessing import Process
size = 10000000   # Number of random numbers to add to list
threads = 6 # Number of threads to create
my_list = []
for i in range(threads):
    my_list.append([])
def func(count, mylist):
    for i in range(count):
        mylist.append(random.random())
        print (random.random())
def multithreaded():
    print("Multi-thread")
    jobs = []
    for i in range(threads):
        thread = Thread(target=func,args=(size,my_list[i]))
        jobs.append(thread)
    # Start the threads
    for j in jobs:
        j.start()
    # Ensure all of the threads have finished
    for j in jobs:
        j.join()

def simple():
    print("Simple")
    for i in range(threads):
        func(size,my_list[i])

def multiprocessed():
    print("Multiprocess")
    processes = []
    for i in range(threads):
        p = Process(target=func,args=(size,my_list[i]))
        processes.append(p)
    # Start the processes
    for p in processes:
        p.start()
    # Ensure all processes have finished execution
    for p in processes:
        p.join()
if __name__ == "__main__":
    #multithreaded()
    #simple()
    multiprocessed()

