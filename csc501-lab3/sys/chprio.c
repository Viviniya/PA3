/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;
	switch (pptr->pstate) {
	case PRREADY:
		insert( dequeue(pid), rdyhead, newprio);
	case PRCURR:
		resched();
	default:
		break;
	}
	if(proctab[pid].waiting_in_lock != -1){ // if the process is currently in waitlist of a lock
		int lock = proctab[pid].waiting_in_lock;
		inherit_changed_priority( lock,  newprio);
	}

	// need to inherit priority
	restore(ps);
	return(newprio);
}
void inherit_changed_priority(int ldes1, int newprio){
  int i;
  int maxp=newprio;
  for(i=0;i<NPROC;i++){
    if(  locks_table[ldes1].process_locked[i] == LOCKED){
        if(maxp>proctab[i].pprio){
            proctab[i].pinh=maxp;
            locks_table[ldes1].max_process_priority=maxp;
        }
    }
  }
}
