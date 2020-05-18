/* 
 * thread library function prototypes
 */



typedef struct tcb tcb;
typedef struct ready ready;
typedef struct sem_t sem_t;

void t_create(void(*function)(int), int thread_id, int priority);
void t_yield(void);
void t_init(void);
void t_shutdown(void);
void t_terminate(void);
void addToReady(struct tcb * thread);
void addToRunning(struct tcb * thread);
void printReadyQueue();
void printRunningQueue();
int sem_init(struct sem_t **sp, int sem_count); 
void sem_wait(struct sem_t *sp);
void sem_signal(struct sem_t *sp);
void sem_destroy(struct sem_t **sp);

