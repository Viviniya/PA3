#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int lock (int ldes1, int type, int priority){
    STATWORD ps;
    disable(ps);

    if (isbadlock(ldes1) || locks_table[ldes1].lock_state==LFREE) {
      restore(ps);
      return(SYSERR);
    }
    if (proctab[currpid].pwaitret == DELETED) {
      restore(ps);
      return(SYSERR);
    }
    if(locks_table[ldes1].lock_type == 0)
      locks_table[ldes1].lock_type=type;
    int lock_type=locks_table[ldes1].lock_type;
    if(lock_type == READ && locks_table[ldes1].num_reader <0){
      restore(ps);
      return(SYSERR);
    }
    if(lock_type == WRITE && locks_table[ldes1].num_reader > 0){
      restore(ps);
      return(SYSERR);
    }
    if(lock_type == WRITE && type == WRITE){
      if(proctab[currpid].waiting_in_lock!= -1)
      {
        restore(ps);
        return(SYSERR);
      }else{
        wait_lock();
        resched();
        restore(ps);
        return(proctab[currpid].pwaitret);
      }

    }

    if(lock_type == READ && type == READ){
        int acq=1;
       if(locks_table[ldes1].writer_waiting>0){
         // need to check priority of writer waiting
         if (locks_table[ldes1].process_list != NULL) {
            struct	lqueue	*lque= lptr->process_list;
            while( lque != NULL)
             {
                int type =lque.type;
                if(type == WRITE){
                  if(lque.priority > priority){
                    acq=-1;
                    break;
                  }
                }
                lque= lque.next;
             }
         }
       }
       if( acq == 1){
           acquire_lock(ldes1,  type,  priority);
           restore(ps);
           return(OK);
       }
       else{
         if(proctab[currpid].waiting_in_lock!= -1)
         {
           restore(ps);
           return(SYSERR);
         }else{
           wait_lock();
           resched();
           restore(ps);
           return(proctab[currpid].pwaitret);
         }
       }
    }
    if(lock_type == READ && type == WRITE){
      if(proctab[currpid].waiting_in_lock!= -1)
      {
        restore(ps);
        return(SYSERR);
      }else{
        wait_lock();
        resched();
        restore(ps);
        return(proctab[currpid].pwaitret);
      }

    }
    if(lock_type == WRITE && type == READ){
      if(proctab[currpid].waiting_in_lock!= -1)
      {
        restore(ps);
        return(SYSERR);
      }else{
        wait_lock();
        resched();
        restore(ps);
        return(proctab[currpid].pwaitret);
      }

    }
}
void  acquire_lock(int ldes1, int  type,  int priority){
  proctab[currpid].lock_details[ldes1] = LOCKED;
  locks_table[ldes1].lock_count++;
  if(type==READ)
  locks_table[ldes1].num_reader++;
  locks_table[ldes1].lock_type=type;
  locks_table[ldes1].lock_priority=priority;
  locks_table[ldes1].process_locked[currpid]=LOCKED;
  int pid_priority=proctab[currpid].pprio;
    //Find max priority
  if(locks_table[ldes1].process_list != NULL){
    struct	lqueue	*lque= locks_table[ldes1].process_list;

    while( lque != NULL)
     {
        int pid =lque.pid;
        if(proctab[pid].pprio>pid_priority){
           pid_priority=proctab[pid].pprio;
        }
        lque= lque.next;
     }
  }
  locks_table[ldes1].max_process_priority=pid_priority;
  //inherit priority
  int i;
  for(i=0;i<NLOCKS;i++){
    if(proctab[currpid].lock_details[i]!=0){
        if(i != ldes1 && locks_table[i].max_process_priority>pid_priority){
          pid_priority=locks_table[i].max_process_priority;
        }
    }
  }
  if(proctab[currpid].pprio<pid_priority){
    proctab[currpid].pinh=pid_priority;
  }else{
    proctab[currpid].pinh=0;
  }
}
void  wait_lock(int ldes1, int  type,  int priority){
  proctab[currpid].waiting_in_lock=ldes1;
  if(type ==READ)
    locks_table[ldes1].reader_waiting++;
  if(type ==WRITE)
    locks_table[ldes1].writer_waiting++;
  locks_table[ldes1].lock_type=type;
  locks_table[ldes1].lock_priority=priority;
  enqueue_c(ldes1,   type,   priority,&locks_table[ldes1].head_cirQ,&locks_table[ldes1].tail_cirQ);
  inherit_priority( ldes1);
  proctab[currpid].pwaitret= PRWAIT;

}
//If a process is going to be added to waitlist, its priority if >
//than process currently holding the locks, their priority should be upgraded
void inherit_priority(int ldes1){
  int i;
  int maxp=proctab[currpid].pprio;
  for(i=0;i<NPROC;i++){
    if(  locks_table[ldes1].process_locked[i] == LOCKED){
        if(proctab[currpid].pprio>proctab[i].pprio){
            proctab[i].pinh=maxp;
            locks_table[ldes1].max_process_priority=maxp;
        }
    }
  }
}
