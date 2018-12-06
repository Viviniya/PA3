/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>
/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);

	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
								if(proctab[pid].waiting_in_lock!=-1)
								adjust_priority(pid);
								resched();


	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
								pptr->pstate = PRFREE;
									if(proctab[pid].waiting_in_lock!=-1)
										adjust_priority(pid);
									break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}

int adjust_priority(pid){
	int ldes1=proctab[pid].waiting_in_lock;
	int result= delete_fromQ_withID(pid,&locks_table[ldes1].head_cirQ,ldes1); // deleted the process from wait queue
	if(result == SYSERR){
		return result;
	}
	if(proctab[pid].pprio<locks_table[ldes1].max_process_priority)
		return OK;
	int pid_priority=0;
		//Find max priority
	if(locks_table[ldes1].valid != -1){
		struct	lqueue	*lque= &locks_table[ldes1].process_list;

		while( lque != NULL)
		 {
				int pid =lque->pid;
				if(proctab[pid].pprio>pid_priority){
					 pid_priority=proctab[pid].pprio;
				}
				lque= lque->next;
		 }
	}
	locks_table[ldes1].max_process_priority=pid_priority;
	revert_inherit_priority(ldes1,pid_priority);
	return OK;

}
void revert_inherit_priority(int ldes1,int pid_priority){
	int i;
  for(i=0;i<NPROC;i++){
    if(  locks_table[ldes1].process_locked[i] == LOCKED){
        if(pid_priority<proctab[i].pinh){
            proctab[i].pinh=pid_priority;
        }
    }
  }
}
