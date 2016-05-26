#include "print.h"

#define MAX_THREADS 3

//declaring global variables

long stack[MAX_THREADS][20];

typedef struct TCB
{
	int threadID;
	int idle;
	int busy;
	int ready;
	void (*func)();
	long* stack_ptr;
}tcb_ptr;

 tcb_ptr thread_pool[MAX_THREADS];
 
typedef struct idle_queue
{
	tcb_ptr idle_next;
}idle_ptr;

typedef struct run_queue
{
	tcb_ptr run_next;
	tcb_ptr run_current;
}run_ptr;
//Variables

idle_ptr idle;
run_ptr run;
int queue_full=0; 
int ID[10];
int create_count = 0;
long dummy;
 //declaring functions
void a(void);
void b(void);
void c(void);
extern long* context_init(long*, void *);
extern void func_exe(long*);
extern void esp_restore(void);
extern void yield();
void scheduler_init();
int thread_create(void *stack, void *func);
void scheduler(int);
void (*task[MAX_THREADS])(void);
void print_test(void);
void detour(long*);



//initialising the scheduler
void scheduler_init()
{
	int i;
	int j;
	for(i=0;i<MAX_THREADS;i++)
	{
		for(j=0;j<20;j++)
		{
			stack[i][j] = i*j;
		}
	}
	task[0]= a;
	task[1]= b;
	task[2]= c;
	pos_x = 0;
	pos_y += 2;
	for(i =0 ; i < MAX_THREADS ; i++ )
	{
		thread_pool[i].threadID = i;
		thread_pool[i]. idle= 1;
		thread_pool[i]. busy= 0;
		thread_pool[i]. ready= -1;
		thread_pool[i]. stack_ptr= 0;
	}
	
	//initalise the idle queue 
	idle.idle_next = thread_pool[0];
	
	
	
	//creating threads for desired task
	ID[create_count] = thread_create((void*)&stack[0], task[0]);
	ID[create_count] = thread_create((void*)&stack[1], task[1]);
	ID[create_count] = thread_create((void*)&stack[2], task[2]);
	//ID[create_count] = thread_create((void*)&stack[2], task[0]);				
	scheduler(0);

	//ID[create_count] = thread_create((void*)&stack[0], task[0]);				
	//scheduler();

	return;
}

int thread_create(void *stack, void *func)
{
	create_count ++;
	int i,flag=1;int id;
	long *sp = (long *)stack;
	//	print_num((int))
		if(queue_full==1)
		{
			return -1;
		}
		else
		{
			if(idle . idle_next . idle == 1)
			{
					thread_pool[idle . idle_next.threadID].  idle = 0;
					thread_pool[idle . idle_next.threadID].  ready = create_count; //idle . idle_next.threadID + 1;
					thread_pool[idle . idle_next.threadID].  busy = 0;
					thread_pool[idle . idle_next.threadID]. func = func;
					thread_pool[idle . idle_next.threadID]. stack_ptr = sp;
					//print_num(thread_pool[idle . idle_next.threadID]. func);
					//initialising the stack to the threads that are scheduled to run
					dummy = context_init(thread_pool[idle . idle_next.threadID] . stack_ptr, 
											thread_pool[idle . idle_next.threadID]. func);
					
					thread_pool[idle . idle_next.threadID]. stack_ptr = dummy;
					//print_num(thread_pool[idle . idle_next.threadID]. func);
					//print_num(thread_pool[idle . idle_next.threadID] . stack_ptr);
					
					//storing the id of the thread being created 				  
					id = idle. idle_next.threadID;
					
					//Finding the next thread that is available for next run    
					for(i = 0; i < MAX_THREADS; i ++)
					{
						if(thread_pool[i]. idle == 1)
						{
							idle . idle_next = thread_pool[i];
							flag = 0;
							queue_full = 0;
							break;
						}
					}
					if(flag == 1)
						queue_full = 1;
				}
			
				if(create_count == 1)
					//initialize the next thread to be run
					run.run_next = thread_pool[0];			
			return id;
		}
}


void scheduler(int a)
{
	if(a)
	__asm__ ("popl %eax;");
	int i;
	//print_num(1);
	//scheduling the task 
		run.run_current = run.run_next;
	if((run.run_next.ready >= 1) && (run . run_next . ready <= create_count ))
	{
		//print_num(thread_pool[0].ready);
		thread_pool[run . run_next . threadID].  busy = 1;
		thread_pool[run . run_next . threadID].  ready = create_count+1;
		for(i = 0; i < create_count ; i ++)
			{
				thread_pool[thread_pool[i].threadID].  ready--;
				if(thread_pool[thread_pool[i].threadID].  ready == 1)
				run . run_next = thread_pool[i];
			}
		func_exe(run . run_current .  stack_ptr);		
	}		
	//if thread_create is called more than the number of threads that are idle
	if(create_count > MAX_THREADS)
	{
		pos_x =0;
		pos_y +=2;
		WriteCharacter ("No more threads available to create");
	}
	
	//Resetting the counters to 0 when all the scheduled tasks have finished executing
	queue_full = 0;
	//create_count = 0;	
	return;
}		


void a(void)
{
	pos_x =0;
	pos_y +=2;
	WriteCharacter("a");
	yield();
	pos_x =0;
	pos_y +=2;	
	WriteCharacter("x");
	yield();
loop: goto loop;
}

void b(void)
{
	pos_x =0;
	pos_y +=2;
	WriteCharacter("b");
	yield();
	pos_x =0;
	pos_y +=2;
	WriteCharacter("y");
	yield();
loop: goto loop;
}

void c(void)
{
	pos_x =0;
	pos_y +=2;
	WriteCharacter("c");
	yield();
	pos_x =0;
	pos_y +=2;
	WriteCharacter("z");
	yield();
loop: goto loop;	
}

void detour(long* sp)
{
	__asm__("popl %eax;");
	int i;
	for(i=0;i<create_count;i++)
	{
		if(thread_pool[i].ready == create_count)
			thread_pool[i].stack_ptr = sp;
	}
	scheduler(1);
}


