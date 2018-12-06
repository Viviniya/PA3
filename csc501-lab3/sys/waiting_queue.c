#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>




void init_data(lqueue *framed,int ldes1, int  type,  int priority) {
    STATWORD ps;
    disable(ps);
    kprintf("Enqueued init_data(): %d \n",currpid );
    framed->pid = currpid;
    framed->priority = priority;
    framed->lock_state = WAITING;
    framed->wait_state = WAITING;
    framed->type = type;
    framed->queuing_delay = 0;
    framed->next = NULL;
    restore(ps);
}

void enqueue_c(int ldes1, int  type,  int priority,struct lqueue *head_cirQ, struct lqueue *tail_cirQ) {
    STATWORD ps;
    disable(ps);
    int sizeQueue=sizeof (struct lqueue);
    struct lqueue *framed = (struct lqueue*) getmem(sizeQueue);
    init_data(&framed, ldes1,   type,   priority);
    if (head_cirQ == NULL ) {
    kprintf("Enqueued first call: %d \n",ldes1 );
        head_cirQ = *tail_cirQ= framed;
    } else {
        tail_cirQ->next = framed;
        tail_cirQ = framed;

 kprintf("Enqueued second call: %d \n",ldes1 );
    }
	 kprintf("Enqueued: %d \n",ldes1 );
    restore(ps);
}

void delete_fromQ_withID(int pid, struct lqueue *head_cirQ){
  int sizeQueue=sizeof (struct lqueue);
  int loop=1;
  if (*head_cirQ == NULL) {
    return SYSERR;
  }
  struct lqueue *current = (struct lqueue*) getmem(sizeQueue);
  struct lqueue *previous = (struct lqueue*) getmem(sizeQueue);
  while(loop !=0){
	kprintf("delete_deomQ: %d \n",pid );
      current= *head_cirQ;
      if(framed->pid  == pid){
          if(previous == NULL){
          head_cirQ= head_cirQ->next;
          freemem(current, sizeQueue);
          freemem(previous, sizeQueue);

          return OK;
        }
        else{
            current = current->next;
            previous->next=current;
            freemem(current, sizeQueue);
            freemem(previous, sizeQueue);

            return OK;
      }
    }
    previous=current;
    current=current->next;

  }
  return SYSERR;
}
