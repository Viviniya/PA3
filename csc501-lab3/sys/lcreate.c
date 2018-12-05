/* screate.c - screate, newsem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

LOCAL int newlock();

/*------------------------------------------------------------------------
 * lcreate  --  create and initialize a lock, returning its id
 *------------------------------------------------------------------------
 */
SYSCALL lcreate()
{
	STATWORD ps;
	int	lock_id;
	disable(ps);
	if ((lock_id=newlock())==SYSERR ) {
		restore(ps);
		return(SYSERR);
	}
  if (isbadsem(lock_id)) {
		restore(ps);
		return(SYSERR);
	}
	restore(ps);
	return(lock_id);
}

/*------------------------------------------------------------------------
 * newlock  --  allocate an unused lock and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newlock()
{
	int	lock_id;
	int	i;

	for (i=0 ; i<NLOCKS ; i++) {
		lock_id=nextloc;
		if (locks_table[lock_id].lock_state==LFREE) {
			locks_table[lock_id].lock_state = LUSED;
      nextloc = (nextloc+1) % NLOCKS;
			return(lock_id);
		}
	}
	return(SYSERR);
}
