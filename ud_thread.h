/* 
 * thread library function prototypes
 */

typedef struct tcb tcb;
typedef struct ready ready;

void t_create(void(*function)(int), int thread_id, int priority);
void t_yield(void);
void t_init(void);
void addToReady(struct tcb * thread);
struct tcb addToRunning(struct tcb * thread);

