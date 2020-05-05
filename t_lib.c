#include "t_lib.h"


//ucontext_t *running; --- not sure if we are using this one 

// running thread 
struct tcb *running;

// Ready Queue for threads 
struct ready { 
	struct tcb * thread;
	struct tcb * next;
}* head;


struct tcb {
	int thread_id;
  	int thread_priority;
	ucontext_t *thread_context;
};



/**
 * todo: switch the tcb from the running and ready queue
 * 
 *  need to call addToReady on the old running thread and addTo
 *  runing on the head of the ready queue
 */
void t_yield() {
	if(head){
		ucontext_t * tmpRun = running->thread_context;
		ucontext_t * tmpReady = head->thread->thread_context;

		// struct tcb * tmp = running;
		addToReady(running);
		addToRunning();

		swapcontext(tmpRun, tmpReady);
	
	}
	
	
}

// sets tup the running and ready queue
void t_init() {
	struct ready * ready = (struct ready *) malloc(sizeof(struct ready)); 

	ucontext_t *tmp;
	tmp = (ucontext_t *) malloc(sizeof(ucontext_t));
	getcontext(tmp);    /* let tmp be the context of main() */
	struct tcb * tcb = (struct tcb *) malloc(sizeof(struct tcb)); 
	tcb->thread_context = tmp;
	tcb->thread_id = 1;
	tcb->thread_priority = 1;

    running = tcb;
   
}


// todo: craete a new tcb and then call addToRunning with the new TCB 
int t_create(void (*fct)(int), int id, int pri) {
	size_t sz = 0x10000;

	ucontext_t *uc;
	uc = (ucontext_t *) malloc(sizeof(ucontext_t));



	getcontext(uc);
	/***
	 uc->uc_stack.ss_sp = mmap(0, sz,
		PROT_READ | PROT_WRITE | PROT_EXEC,
		MAP_PRIVATE | MAP_ANON, -1, 0);
	***/
	uc->uc_stack.ss_sp = malloc(sz);  /* new statement */
	uc->uc_stack.ss_size = sz;
	uc->uc_stack.ss_flags = 0;
	//uc->uc_link = running; 
	makecontext(uc, (void (*)(void)) fct, 1, id);

	struct tcb * tcb = (struct tcb *) malloc(sizeof(struct tcb)); 
	tcb->thread_context = uc;
	tcb->thread_id = id;
	tcb->thread_priority = pri;
	addToReady(tcb);
}

/**
 * adds a tcb to the end of the running queue. If the queue is empty
 * adds the tcb to the head of the list 
 * 
 * call when you are creating a new thread
 * or when you yield
 * 
 * @param: thread control block 
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


void addToRunning() {
    struct ready * tmp = head;
    head = head->next;
    running = tmp->thread;
}

void t_shutdown(void){

}

void t_terminate(void){
	
	ucontext_t * tmpRun = running->thread_context;
	ucontext_t * tmpReady = head->thread->thread_context;

	// free(running);
	// struct tcb * tmp = running;
	addToRunning();
	
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
