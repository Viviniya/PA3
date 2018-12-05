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
    locks_table[i].lock_priority=-1;
    locks_table[i].process_list=NULL;
    //locks_table[i].wait_list=NULL;
  }
  restore(ps);
  return OK;
}
