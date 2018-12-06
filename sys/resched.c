/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than any pid(pinh or pprio) in queue*/
	int pid= return_max_pinh();
	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (pid == currpid) ){
		return(OK);
	}

	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */

	nptr = &proctab[pid];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif

	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

	/* The OLD process returns here when resumed. */
	return OK;
}
int return_max_pinh(){
	int	next;			/* runs through list		*/
	int	prev;
	next = q[rdyhead].qnext;
	int flag =1;
	int curprio=-1;
	if(proctab[currpid].pinh!= 0)
		curprio=proctab[currpid].pinh;
	else
		curprio=proctab[currpid].pprio;
		int pid = currpid;
	while (q[next].qnext>0)	{
		 kprintf("output pid=%d\n", next);
		 if(proctab[next].pinh!=0){
			  if(curprio < proctab[next].pinh){
					pid=next;
					curprio=proctab[next].pinh;
				}

		 }
		 else{
			 if(curprio < proctab[next].pprio){
				  pid=next;
						curprio=proctab[next].pprio;
			 }

		 }
			next = q[next].qnext;
	}
	return pid;
}
