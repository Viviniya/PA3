#include <stdio.h>
#include <proc.h>
#include <lock.h>

int linit(){
  STATWORD ps;
  disable(ps);
  int i;

  for(i=0;i< NLOCKS;i++){
    locks_table[i].lock_state=LFREE;
    locks_table[i].lock_count=0;
    locks_table[i].lock_type=0;
    locks_table[i].num_reader=0;
    locks_table[i].reader_waiting=0;
    locks_table[i].writer_waiting=0;
    locks_table[i].max_process_priority=0;
    int j;
    for (j=0;j<NPROC;j++)
    locks_table[i].process_locked[j]=-1;
    locks_table[i].head_cirQ=NULL;
    locks_table[i].tail_cirQ=NULL;
    locks_table[i].lock_priority=-1;
    locks_table[i].process_list=NULL;
    //locks_table[i].wait_list=NULL;
  }
  restore(ps);
  return OK;
}
