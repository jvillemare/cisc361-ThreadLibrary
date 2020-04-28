<center>

# Programming Assignment 6

<font size="+2">**User-Level Thread Library**</font>  
</center>

### Objective

In this project, you will design and implement a <span style="font-weight: bold;">user-level thread library</span> which supports thread creation, thread scheduling, thread synchronization, and inter-thread communication (message passing and mailbox) functions.

### Assignment

You will design and implement, in 4 phases, a user-level thread library.

#### Phase 1

*   Thread library initialization and shutdown, and thread creation and termination:

*   <font face="Courier New">void t_init();</font> /* Initialize the thread library by setting up the "running" and the "ready" queues, creating TCB of the "main" thread, and inserting it into the running queue. */
*   <font face="Courier New">void t_shutdown();</font> /* Shut down the thread library by freeing all the dynamically allocated memory. */
*   <font face="Courier New">int t_create(void (*func)(int), int thr_id, int pri);</font> /* Create a thread with priority <font face="Courier New">pri,</font> start function <font face="Courier New">func</font> with argument <font face="Courier New">thr_id</font> as the thread id. Function <font face="Courier New">func</font> should be of type <font face="Courier New">void func(int)</font>. TCB of the newly created thread is added to the <font color="blue">**end**</font> of the "ready" queue; the parent thread calling <font face="Courier New">t_create()</font> continues its execution upon returning from <font face="Courier New">t_create()</font>. */
*   <font face="Courier New">void t_terminate();</font> /* Terminate the calling thread by removing (and freeing) its TCB from the "running" queue, and resuming execution of the thread in the head of the "ready" queue via <font face="Courier New">setcontext()</font>. Every thread must end (semantically) with a call to <font face="Courier New">t_terminate()</font>. */
*   An initial thread control block (TCB) may look like the following.

    <pre> <tt>struct tcb {
    	  int         thread_id;
              int         thread_priority;
    	  ucontext_t *thread_context;
    	  struct tcb *next;
           };

           typedef struct tcb tcb;</tt> </pre>

*   Yield the CPU:

*   <font face="Courier New">void t_yield();</font> /* The calling thread volunarily relinquishes the CPU, and is placed at the end of the ready queue. The first thread (if there is one) in the ready queue resumes execution. */

*   Test cases: T1 and T1x

#### Phase 2

*   CPU scheduling and time slicing:

*   2-Level Queue (2LQ) with level 0 (high priority) and 1 (low priority). By default, the <font face="Courier New">main</font> thread is with level 1 (low priority)<font face="Courier New">.</font>

*   Round-Robin (RR) scheduler within each level with time quantum of, say, 10,000 microseconds. Use <font face="Courier New">ualarm()</font> to "simulate" timer interrupts so that when a <font face="Courier New">SIGALRM</font> is caught by the signal handler (i.e., the arrival of a timer interrupt), context switching is performed. Notice that right before context switching to a new thread, another <font face="Courier New">SIGALRM</font> needs to be scheduled (via <font face="Courier New">ualarm</font>) as the next timer interrupt. Also, when a thread yields before its time quantum expires, the scheduled <font face="Courier New">SIGALRM</font> should be canceled via <font face="Courier New">ualarm(0,0)</font>. The thread library functions of <font face="Courier New">t_init()</font>, <font face="Courier New">t_terminate()</font>, <font face="Courier New">t_shutdown()</font>, and <font face="Courier New">t_yield()</font> should be made **atomic** by distabling (timer) interrupts via <font face="Courier New">sighold()</font> and <font face="Courier New">sigrelse()</font>.

*   Test cases: T1a (2LQ), T2 (RR), T4 (RR), and T7 (2LQ)

#### Phase 3

*   <font size="+0">Thread synchronization with Semaphore:</font>

*   <font size="+0"><font face="Courier New">int sem_init(sem_t **sp, int sem_count);</font> /* Create a new semaphore pointed to by <font face="Courier New">sp</font> with a count value of <font face="Courier New">sem_count</font>. */</font>
*   <font size="+0"><font face="Courier New">void sem_wait(sem_t *sp);</font> /* Current thread does a wait (P) on the specified semaphore. */</font>
*   <font size="+0"><font face="Courier New">void sem_signal(sem_t *sp);</font> /* Current thread does a signal (V) on the specified semaphore. Follow the <font color="blue">**Mesa**</font> semantics (p. 9 of Chapter 30 Condition Variables) where the thread that signals continues, and the first waiting (blocked) thread (if there is any) becomes ready. */</font>
*   <font size="+0"><font face="Courier New">void sem_destroy(sem_t **sp);</font> /* Destroy (free) any state related to specified semaphore. */</font>
*   Semaphore type <font face="Courier New">sem_t</font> is defined as follows.

    <pre> <tt>typedef struct {
             int count;
             tcb *q;
           } sem_t;</tt> </pre>

*   [sample thread library where applications include ud_thread.h](UD_Thread_Sem.tar)
*   Test cases: T3, T10, [Shone-Hoffman](3test.c) ([output](3test_output.txt)), and [Dining Philosophers](philosophers.c) [[4 sample outputs](p) (where all philosophers ate in the 4th run)]

#### Phase 4

*   Inter-thread communications:

*   mailbox:

*   <font face="Courier New">int mbox_create(mbox **mb);</font> /* Create a mailbox pointed to by <font face="Courier New">mb.</font> */
*   <font face="Courier New">void mbox_destroy(mbox **mb);</font> /* Destroy <font size="+0">any state related to the</font> mailbox pointed to by <font face="Courier New">mb.</font> */
*   <font face="Courier New">void mbox_deposit(mbox *mb, char *msg, int len);</font> /* Deposit message <font face="Courier New">msg</font> of length <font face="Courier New">len</font> into the mailbox pointed to by <font face="Courier New">mb</font>. */
*   <font face="Courier New">void mbox_withdraw(mbox *mb, char *msg, int *len);</font> /* Withdraw the first message from the mailbox pointed to by <font face="Courier New">mb</font> into <font face="Courier New">msg</font> and set the message's length in <font face="Courier New">len</font> accordingly. The caller of <font face="Courier New">mbox_withdraw()</font> is responsible for allocating the space in which the received message is stored. If there is no message in the mailbox, <font face="Courier New">len</font> is set to 0\. Notice that <font face="Courier New">mbox_withdraw()</font> is <font color="blue">**not blocking**</font>, i.e., <font face="Courier New">mbox_withdraw()</font> returns immediately if there is no message available in the specified mailbox. Even if more than one message awaits the caller, only one message is returned per call to <font face="Courier New">mbox_withdraw()</font>. Messages are withdrew in the order in which they were deposited. */
*   An initial mailbox may look like the following.

    <pre> <tt>struct messageNode {
             char *message;     // copy of the message
             int  len;          // length of the message
             int  sender;       // TID of sender thread
             int  receiver;     // TID of receiver thread
             struct messageNode *next; // pointer to next node
           };

           typedef struct {
    	  struct messageNode  *msg;       // message queue
    	  sem_t               *mbox_sem;
           } mbox;</tt> </pre>

*   Test cases: T6

*   message passing:

*   <font face="Courier New">void send(int tid, char *msg, int len);</font> /* Send a message to the thread whose tid is <font face="Courier New">tid. msg</font> is the pointer to the start of the message, and <font face="Courier New">len</font> specifies the length of the message in bytes. In your implementation, all messages are character strings. */
*   <font face="Courier New">void receive(int *tid, char *msg, int *len);</font> /* Wait for and receive a message from another thread. The caller has to specify the sender's tid in <font face="Courier New">tid</font>, or sets <font face="Courier New">tid</font> to 0 if it intends to receive a message sent by any thread. If there is no "matching" message to receive, the calling thread waits (i.e., blocks itself). [A sending thread is responsible for waking up a waiting, receiving thread.] Upon returning, the message is stored starting at <font face="Courier New">msg.</font> The tid of the thread that sent the message is stored in <font face="Courier New">tid</font>, and the length of the message is stored in <font face="Courier New">len</font>. The caller of <font face="Courier New">receive()</font> is responsible for allocating the space in which the message is stored. Even if more than one message awaits the caller, only one message is returned per call to <font face="Courier New">receive()</font>. Messages are received in the order in which they were sent. The caller will not resume execution until it has received a message (blocking receive). */
*   Note: send() and receive() together are termed <font color="blue">asynchronous communication</font>.
*   Test cases: T5 and T8  

*   <font color="blue">Notes on "message passing" implementation</font>: You may implement Phase 4 through the following two intermediate steps,
    *   **Non-blocking** receive()  
        When a thread executes a non-blocking receive(), the receive() function goes through the thread's message queue and looks for the first "matching" message. If there exists such a matching message, copies it to the space allocated, deletes the message from the message queue, and returns. If no such message, simply returns. Of course, you have to use a **binary** semaphore [from Phase 3] as a lock to protect the examining and dequeuing process.  
        The send() function simply enqueues the sent message to the receiving thread's message queue. Of course, you will need to lock such an enqueue operation.  
        Since there is no blocking involved, you don't need to use any semaphore to block any thread.
    *   Non-discriminatory, **blocking** receive()  
        In this version of receive(), receive() ignores the sender's tid and simply returns the first message (if there is one) in the message queue. However, when receive() is executed but there is no message in the message queue, the invoking thread (executing inside the receive() function) is blocked. To implement such blocking, you use a **counting** semaphore [from Phase 3].  
        Now, the send() function, when invoked by a sending thread, not only enqueues the message, but also "signals" the receiving thread's counting semaphore to wake it up (if the receiving thread is currently blocked). Notice that threads could send messages to a receiving thread at any time so that multiple messages may have been enqueued in a receiving thread's message buffer before the receiving thread even executes a receive(). For instance, send() may have been invoked 3 times (from the same or different threads) so the the receiving thread's semaphore has been signaled 3 times to raise the value of the semaphore to 3! Now when the receiving thread executes receive(), it does sem_wait() and dequeue the first message. Since the semaphore value is not negative, the receiving thread continues and returns from receive() [without blocking]. Given that there are 3 messages in the queue originally, the receiving thread could execute receive() 3 times without being blocked. However, when the 4th receive() is executed, the receiving thread blocks waiting for the next (4th) message to wake it up.  
    <font color="red">Extra Credits</font> <font color="blue">[rendezvous -- synchronous communication]</font> (Refer to Question #2, Chapter 31 Semaphores)  

*   <font face="Courier New">void block_send(int tid, char *msg, int length);</font> /* Send a message and wait for reception. The same as <font face="Courier New">send()</font>, except that the caller does not return until the destination thread has received the message. */
*   <font face="Courier New">void block_receive(int *tid, char *msg, int *length);</font> /* Wait for and receive a message; the same as <font face="Courier New">receive()</font>, except that the caller (a receiver) also needs to specify the sender. */
*   Test cases: T9 and T11

### How to get started

Start the project from the [sample thread library (a tar file).](UD_Thread.tar)

*   Context: Use <font face="Courier New"><span style="font-weight: bold;">getcontext()</span></font> and <font face="Courier New"><span style="font-weight: bold;">setcontext()</span></font> within the same thread ([let's loop](loop.c))
*   Switching between threads : Use <font face="Courier New"><span style="font-weight: bold;">getcontext()</span>/<span style="font-weight: bold;">setcontext()</span>/<span style="font-weight: bold;">swapcontext()</span></font> system calls.  
    ([sample swapcontext code](swapcontext.c) and its [execution trace](swapcontext.pdf))  
    ([sample swapcontext via yield code](swapcontext-yield.c))
*   Creating a new thread: Use <font face="Courier New"><span style="font-weight: bold;">makecontext()</span></font> system call.
*   Scheduling: Use <font face="Courier New">ualarm()</font>and a signal handler that catches <font face="Courier New">SIGALRM</font> and schedules the next thread in accordance with the scheduling policy. Use <font face="Courier New"><span style="font-weight: bold;">sigrelse(3c)</span>/<span style="font-weight: bold;">sighold(3c)</span></font> to make thread library APIs atomic. ([sample alarm code](test_alarm.c)) ([example critical section code](interrupts.c) by Sean Krail)
*   Semaphore : Atomic execution of wait and signal must be enforced. This can be achieved by disabling and enabling "interrupts" via `sighold(3c)` and `sigrelse(3c)`, respectively.

### Test Runs

The TA will use the test programs [T1](test01.c), [T1x](test01x.c), [T1a](test01a.c), [T2](test02.c), [T2a](test02a.c), [T3](test03.c), [T4](test04.c), [T5](test05.c), [T6](test06.c), [T7](test07.c), [T8](test08.c), [T9](test09.c), [T10](test10.c), and [T11](test11.c), to test your code against outputs [1](1), [1x](1x), [1a](1a), [2](2), [2a](2a), [3](3), [4](4), [5](5), [6](6), [7](7), [8](8), [9](9) or [9.alt](9.alt), [10](10), and [11](11) or [11.alt](11.alt), and may use other test programs as well.  
Blocking send/receive: [[9 with active V](9.active)], [[9 with passive V](9.passive)], [[11 with active V](11.active)], [[11 with passive V](11.passive)]

### Contributed Test Runs from Students

Phase 1:

*   [Shone-Hoffman](mytest.c) ([output](mytest_output.txt))
*   [Sullivan](Tfib.c) ([makefile](Makefile))

Phase 2:Phase 3:

*   [Senzer-Spicer](TSS.c) ([output](TSSOutput.txt) and [README](README.TSS))

Phase 4:

*   [Senzer-Spicer](SenzerSpicerMailbox.c) ([output](SSP4Output.txt) and [README](README.SSmbox))

* * *

### Grading

*   50% correctly working thread library initialization, thread creation/termination, 2LQ/RR and voluntary scheduling.
*   15% correctly working thread synchronization via semaphore.
*   25% correctly working inter-thread communication.
*   10% documentation and code structure.
