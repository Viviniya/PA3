#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>


int releaseall(int numlocks, ...){
    STATWORD ps;
    disable(ps);

    int locks[numlocks];
    int results[numlocks];
    unsigned long *a = (unsigned long *)(&numlocks) + (numlocks);
    int i;
    for (i=0 ; i < numlocks ; i++){
        locks[i] = *a--;
        results[i]=0;
    }

    for (i=0;i<numlocks;i++){
      int ldes=locks[i];
      int j;

      if(locks_table[ldes].process_locked[currpid]==-1) // the process doesnt hold the lock specified
      {
        results[i]=-1;
      }
      if(results[i] == 0){  // process holds the lock, so release it
         if(locks_table[ldes].lock_type == READ){
           locks_table[ldes].num_reader--;
           if(locks_table[ldes].num_reader==0){
             //provide lock to another process from waitlist
              transfer_lock(ldes1);
           }
           else{
             // still readers holding lock. nothing to do
           }

         }
         else if(locks_table[ldes].lock_type == WRITE){
              //provide lock to another process from waitlist
                transfer_lock(ldes1);
         }
         reset_locks( currpid);
      }

    }
    int k, result=OK;
    for (k=0;k<numlocks;k++){
      if(result[k]== -1){
        result=SYSERR;
        break;
      }
    }
   restore(ps);
   resched();
   return(result);
}
int transfer_lock(int ldes1){
  //provide lock to another process from waitlist
  int max_pid=-1;
  int maxprio=-1001;
  int type=-1;
  int time_max=0
  if (locks_table[ldes1].valid != -1) {
     struct	lqueue	*lque= &locks_table[ldes1].process_list;
     while( lque != NULL)  // other checks might be needed
      {

           if(lque->priority > maxprio){
               maxprio=lque->priority;
               max_pid=lque->pid;
               type=lque->type;
               time_max=lque->queuing_delay;
           }
           else if (lque->priority == maxprio ){
              if(time_max< lque->queuing_delay){
                maxprio=lque->priority;
                max_pid=lque->pid;
                type=lque->type;
                time_max=lque->queuing_delay;
              }
              else if (time_max ==lque->queuing_delay){
                  if(type == WRITE){
                    maxprio=lque->priority;
                    max_pid=lque->pid;
                    type=lque->type;
                    time_max=lque->queuing_delay;
                  }
                  else{
                    maxprio=lque->priority;
                    max_pid=lque->pid;
                    type=lque->type;
                    time_max=lque->queuing_delay;
                  }
              }
           }


         lque= lque->next;
      }
  }else{ //nothing in wait list
    locks_table[ldes1].lock_state=LFREE;
    locks_table[ldes1].lock_count=0;
    locks_table[ldes1].lock_type=0;
    locks_table[ldes1].num_reader=0;
    locks_table[ldes1].reader_waiting=0;
    locks_table[ldes1].writer_waiting=0;
    locks_table[ldes1].max_process_priority=0;
    int j;
    for (j=0;j<NPROC;j++)
    locks_table[ldes1].process_locked[j]=-1;
    locks_table[ldes1].lock_priority=-1;
    locks_table[ldes1].valid=-1;
  }
  if(max_pid >-1){
    locks_table[ldes1].lock_state=LUSED;
    locks_table[ldes1].lock_type=type;
    if(type == READ){
     locks_table[ldes1].num_reader++;
     locks_table[ldes1].reader_waiting--;
     }
     else{
       locks_table[ldes1].writer_waiting--;
     }
     locks_table[ldes1].process_locked[max_pid]=LOCKED;
     proctab[max_pid].lock_details[ldes1]=LOCKED;
  }
    adjust_priority(max_pid);

}
