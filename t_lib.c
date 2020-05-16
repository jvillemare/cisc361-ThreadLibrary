#include "t_lib.h"



/**
 *  Pointer to the running thread.
 */
struct tcb *running;


/**
 * Linked list that holds the ready queue
 */ 
struct ready { 
	struct tcb * thread;
	struct tcb * next;
}* head;


/**
 *  Thread Control Block i.e. node for the ready list 
 */
struct tcb {
	int thread_id;
  	int thread_priority;
	ucontext_t *thread_context;
};

/**
 *  Semephore
 * 
 */
struct sem_t{
	int count;
    struct tcb *q;
};



///////////////////////////////////////////////////
//												 //
//					Phase 1						 //
//												 //
///////////////////////////////////////////////////

/**
 *  need to call addToReady on the old running thread and addTo
 *  runing on the head of the ready queue
 */
void t_yield() {
	if(head){
		ucontext_t * tmpRun = running->thread_context;
		ucontext_t * tmpReady = head->thread->thread_context; 
		addToReady(running);
		addToRunning();
		swapcontext(tmpRun, tmpReady);
	}	
}

/**
 * Initilizaed the ready quene and makes the main thread. Once the main
 * thread is created it is added to the running queue. Allocated memory 
 * for the main thread
 */
void t_init() {
	struct ready * ready = (struct ready *) malloc(sizeof(struct ready)); 

	ucontext_t *tmp;
	tmp = (ucontext_t *) malloc(sizeof(ucontext_t));
	getcontext(tmp); 
	struct tcb * tcb = (struct tcb *) malloc(sizeof(struct tcb)); 
	tcb->thread_context = tmp;
	tcb->thread_id = 1;
	tcb->thread_priority = 1;

    running = tcb;
   
}


/**
 *  Creates a new thread and adds it to the ready queue. Allocates
 * memory for the new thread
 * 
 * @param fct entry function for the new thread
 * @param id the id of the the thread
 * @param pri the priority of the thread 
 * 
 */
int t_create(void (*fct)(int), int id, int pri) {
	size_t sz = 0x10000;

	ucontext_t *uc;
	uc = (ucontext_t *) malloc(sizeof(ucontext_t));
	getcontext(uc);
	uc->uc_stack.ss_sp = malloc(sz);  
	uc->uc_stack.ss_size = sz;
	uc->uc_stack.ss_flags = 0; 
	makecontext(uc, (void (*)(void)) fct, 1, id);

	struct tcb * tcb = (struct tcb *) malloc(sizeof(struct tcb)); 
	tcb->thread_context = uc;
	tcb->thread_id = id;
	tcb->thread_priority = pri;
	addToReady(tcb);
}

/**
 * adds a tcb to the end of the ready queue. If the queue is empty
 * adds the tcb to the head of the list 
 * 
 * call when you are creating a new thread
 * or when you yield
 * 
 * @param thread thread control block 
 */
void addToReady(struct tcb * thread) {
    struct ready *temp, *new;
    struct ready ** indirect = &head;

    temp = head;

    while((temp != NULL)){
        indirect = &temp->next;
        temp = temp->next;
    }

    new = (struct tcb *)malloc(sizeof(struct tcb));
    new->thread = thread;

    *indirect = new;
}


/**
 *  add the head of the ready queue to the running queue and sets 
 * head to the next node in the ready list
 */
void addToRunning() {
    struct ready * tmp = head;
    head = head->next;
    running = tmp->thread;
}

void t_shutdown(void){
	free(running);

	struct ready * tmp = head;
	while(tmp){
		struct ready * t2 = tmp->next;
		free(tmp->thread->thread_context);
		free(tmp);
		tmp = t2;
	}
}


/**
 *  removes a thread from the process 
 */ 
void t_terminate(void){
	ucontext_t * tmpRun = running->thread_context;
	ucontext_t * tmpReady = head->thread->thread_context;
	
	addToRunning();
	
	if(tmpRun && tmpReady)
		swapcontext(tmpRun, tmpReady);
	
}


void printReadyQueue(){

    struct ready * tmp = head;

    while(tmp) {
        printf("%d ",tmp->thread->thread_id);
        tmp = tmp->next;
    }
    printf("\n");
}

void printRunningQueue(){
	printf("print running queue \n%d \n", running->thread_id);
}

///////////////////////////////////////////////////
//												 //
//					Phase 3						 //
//												 //
///////////////////////////////////////////////////

/**
 * Create a new semaphore pointed to by sp with a count value of sem_count. 
 * '
 * @param sp holds the address of the new semeaphore
 * @param sem_count the count value of the new semaphore
 * 
 * @return value of sem_count 
 */
int sem_init(struct sem_t **sp, int sem_count){
	return sem_count;
}


/**
 * Current thread does a wait (P) on the specified semaphore.
 * 
 * @param sp the tread that does the P opperation 
 */
void sem_wait(struct sem_t *sp){

}

/**
 * Current thread does a signal (V) on the specified semaphore.
 * 
 * @param sp the tread the does the V opperation
 */
void sem_signal(struct sem_t *sp){

}

/**
 *  Destroy (free) any state related to specified semaphore
 */
void sem_destroy(struct sem_t **sp){

}