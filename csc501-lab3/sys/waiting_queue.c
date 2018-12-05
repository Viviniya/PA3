#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>

extern struct lqueue *head_cirQ;
extern struct lqueue *tail_cirQ;

void init() {
    STATWORD ps;
    disable(ps);
    head_cirQ= NULL;
    tail_cirQ = NULL;
    restore(ps);
}
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

void enqueue_c(int ldes1, int  type,  int priority) {
    STATWORD ps;
    disable(ps);
    int sizeQueue=sizeof (struct lqueue);
    struct lqueue *framed = (struct lqueue*) getmem(sizeQueue);
    init_data(&framed, ldes1,   type,   priority);
    if (head_cirQ == NULL && tail_cirQ == NULL) {
 kprintf("Enqueued first call: %d \n",ldes1 );
        head_cirQ = tail_cirQ= framed;
    } else {
        tail_cirQ->next = framed;
        tail_cirQ = framed;

 kprintf("Enqueued second call: %d \n",ldes1 );
    }
	 kprintf("Enqueued: %d \n",ldes1 );
    restore(ps);
}

int delete_frame(lqueue *previous, int id) {
    STATWORD ps;
    disable(ps);
    int sizeQueue=sizeof (struct lqueue);
    struct lqueue *current = (struct lqueue*) getmem(sizeQueue);

    if (head_cirQ != NULL) {
	kprintf("delete_frame: %d \n",id );
      if(previous ==NULL){ // if the head is to be removed
        current= head_cirQ;
        if(current->frametab_id == id){
          head_cirQ= head_cirQ->next;
          freemem(current, sizeQueue);
        }
      }
      else{
            current = previous->next;
          if(current->frametab_id == id){
            previous->next=current->next;
            freemem(current, sizeQueue);
          }
          else{
          //  freemem(current, sizeQueue);
            restore(ps);
            return SYSERR;
          }


      }
      restore(ps);
      return OK;
    } else {
        restore(ps);
        return SYSERR;
    }
}
void delete_fromQ_withID(int pid){
  int sizeQueue=sizeof (struct lqueue);
  int loop=1;
  if (head_cirQ == NULL) {
    return SYSERR;
  }
  struct lqueue *current = (struct lqueue*) getmem(sizeQueue);
  struct lqueue *previous = (struct lqueue*) getmem(sizeQueue);
  while(loop !=0){
	kprintf("delete_deomQ: %d \n",pid );
      current= head_cirQ;
      if(current->frametab_id == pid){
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
}
