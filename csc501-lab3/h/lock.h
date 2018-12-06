/* lock.h - isbadlock */

#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef	NLOCKS
#define	NLOCKS		50	/* number of locks, if not defined	*/
#endif

#define	LFREE	1	/* this lock is free		*/
#define	LUSED	2		/* this lock is used		*/

#define	READ	1	/* this lock is free		*/
#define	WRITE	2		/* this lock is used		*/

#define	LOCKED	1	/* this lock is free		*/
#define	WAITING	2		/* this lock is used		*/

#ifndef	NPROC				/* set the number of processes	*/
#define	NPROC		30		/*  allowed if not already done	*/
#endif

struct	lqueue	{
	int	pid;
	int	priority;
	int lock_state;
  int wait_state;
  int type;
  int queuing_delay;
  struct lqueue *next;
};
struct	lentry	{		/* lock table entry		*/
	int lock_state;		/* the state LFREE or LUSED		*/
	int	lock_count;		/* count for this lock		*/
  int num_reader;
  int reader_waiting;
  int writer_waiting;
	int lock_type;   /* Read or Write Lock */
  int lock_priority; /*lprio described in assignment */
  int max_process_priority;
  int process_locked[NPROC];
  struct	lqueue process_list;
  struct	lqueue head_cirQ;
  struct	lqueue tail_cirQ;
	int valid;

};
extern	struct	lentry	locks_table[];
extern	int	nextloc;
extern unsigned long ctr1000;
extern int adjust_priority( int pid);
extern int release_held_locks(int pid);
extern int reset_locks(int pid);
#define	isbadlock(l)	(l<0 || l>=NLOCKS)

#endif
