# FIFOS<br />
FIFO scheduler on Bare Metal x86 hardware<br />
To compile the code, run make.<br />
The two directories contain support for running multiple threads on a single address space with separate stacks for each thread.<br />

FIFOS-1:<br />
	 In this version, the scheduler is not automatically preempted and hence the yield() function has to be called to switch to the next available thread.<br />
	 The MAX_THREADS header determines the maximum number of threads you can create.<br />
	 The int thread_create function is used to create a thread and allocate respective stack.<br />
	 The threads are scheduled in the order in which they are created.<br />
	 Each function has it's own stack and the yield function pushes data onto the corresponding stacks before switching them.<br />

FIFOS-2:<br />
	The IDT, PIC and PIT are set up to fire hardware interrupts periodically.<br />
	This executes the ISR which calls the scheduler to execute the next thread in the queue.<br />
	This is also based on the FIFO scheduling algorithm.<br />
	The only difference between this and the previous version is that the context_switch function is called from the ISR which switches stacks.<br />
