/* ldelete.c - ldelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL ldelete(int lock_id)
{
	STATWORD ps;
	int	pid;
	struct	lentry	*lptr;
	disable(ps);
	if (isbadlock(lock_id) || locks_table[lock_id].lock_state==LFREE) {
		restore(ps);
		return(SYSERR);
	}
	lptr = &locks_table[lock_id];
	lptr->lock_state = LFREE;
	if (lptr->process_list != NULL) {
    struct	lqueue	*lque= lptr->process_list;
		while( lque != NULL)
		  {
        pid=lque.pid;
		    proctab[pid].pwaitret = DELETED;
		    ready(pid,RESCHNO);
        lque= lque.next;
		  }
		resched();
	}

	restore(ps);
	return(OK);
}
