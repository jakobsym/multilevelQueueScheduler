#include <stdlib.h>
#include <string.h>

#include "multilevelQueueScheduler.h"

void attemptPromote( schedule *ps );
int min( int x, int y );

static const int STEPS_TO_PROMOTION = 50;
static const int FOREGROUND_QUEUE_STEPS = 5;

/* createSchedule
 * input: none
 * output: a schedule
 *
 * Creates and return a schedule struct.
 */
schedule* createSchedule( ) {
    schedule *sp = (schedule*)malloc(sizeof(schedule));
    sp->foreQueue = createQueue();
    sp->backQueue = createQueue();

    sp->steps = 0;
    sp->testCount = 0;
    sp->processCount = 50;
    sp->trackSteps = 5;


    /* malloc space for a new schedule and initialize the data in it, returning that schedule */

    return sp;
}

/* isScheduleUnfinished
 * input: a schedule
 * output: bool (true or false)
 *
 * Check if there are any processes still in the queues.
 * Return TRUE if there is.  Otherwise false.
 */
bool isScheduleUnfinished( schedule *ps ) {

    /* if front/rear of foreQueue/backQueue is NULL( nothing in either queues ), then we return false */
    if(isEmpty(ps->foreQueue) == true && isEmpty(ps->backQueue) == true){
        return false;
    }
        /* else queue is not empty we return True (as our schedule is unfinished) */
    else
    {
        return true;
    }
    /* check if there are any process still in a queue.  Return TRUE if there is. */
}

/* addNewProcessToSchedule
 * input: a schedule, a string, a priority
 * output: void
 *
 * Create a new process with the provided name and priority.
 * Add that process to the appropriate queue
 */
void addNewProcessToSchedule( schedule *ps, char *processName, priority p ) {
    process* setProcess = (process*) malloc(sizeof(process));

    setProcess->Data = initializeProcessData(processName);
    setProcess->processName = processName;
    setProcess->foreOrback = p;

    /* p = 0 --> foreground queue */
    if(p == FOREGROUND){
        enqueue(ps->foreQueue, setProcess);
        ps->testCount++;

    }
        /* p = 1 --> background queue */
    else if(p == BACKGROUND){
        enqueue(ps->backQueue, setProcess);
        ps->testCount++;
    }

}

/* runNextProcessInSchedule
 * input: a schedule
 * output: a string
 *
 * Use the schedule to determine the next process to run and for how many time steps.
 * Call "runProcess" to attempt to run the process.  You do not need to print anything.
 * You should return the string received from "runProcess".  You do not need to use/modify this string in any way.
 */
char* runNextProcessInSchedule( schedule *ps ) {
    /* TODO: complete this function. */

    process *setProcess = NULL;
    char *ret = NULL;
    int numSteps;


    /* if foreQueue not empty, then process at front is set to run */
    if(!isEmpty(ps->foreQueue)){
        setProcess = getNext(ps->foreQueue);
    }
    /* if backQueue not empty, then process at front is set to run */
    else if(!isEmpty(ps->backQueue)){
        setProcess = getNext(ps->backQueue);
    }

    loadProcessData(setProcess->Data);

    numSteps = min(FOREGROUND_QUEUE_STEPS, ps->trackSteps);

    attemptPromote(ps);

    bool b = runProcess(setProcess->processName, &ret, &numSteps);

    ps->steps+=numSteps; // Number steps since start
    ps->processCount-=numSteps; // Counts down from 50 to track steps until promotion

    if(ps->processCount < 5){
        ps->processCount = 50;
    }

    if(ps->trackSteps <= 1){
        ps->trackSteps = 5;
    }

    /* Foreground Queue */
    if( !isEmpty(ps->foreQueue) ) {
        /* if Process Complete, remove from queue and free data */
        if(b == true){
            //printf("Entered deletion.\n");
            dequeue(ps->foreQueue);
            freeProcessData();
            free(setProcess->processName);
            free(setProcess);
            ps->trackSteps =5;
        }
        /* if process doesn't run for full 5 steps, then we set it at front of queue */
        else{

            if(numSteps < 5){
                setProcess = getNext(ps->foreQueue);
            }
            /* after 5 time steps, we remove from front, and put at rear of queue to be ran until completion */
            else{
                dequeue(ps->foreQueue);
                enqueue(ps->foreQueue, setProcess); // MOVING TO BACK OF QUEUE
            }
        }
    }

    /* Background Queue */
    else if( !isEmpty(ps->backQueue) ) {
        // if process complete, we remove from queue/free process
        if(b == true){
            dequeue(ps->backQueue);
            freeProcessData();
            free(setProcess->processName);
            free(setProcess);
        }

        else{
            if(b == false){
                dequeue(ps->backQueue);
                enqueue(ps->backQueue, setProcess);
            }
        }
    }


    return ret; /* This will be the char* that runProcess stores in ret when you call it. */
}

/* attemptPromote
 * input: a schedule
 * output: none
 *
 * Promote every background process that has been waiting for 50 time steps.
 */
void attemptPromote( schedule *ps ) {
    if(ps->processCount == STEPS_TO_PROMOTION){
        process *setProcess = dequeue(ps->backQueue);
        promoteProcess(setProcess->processName, setProcess->Data);
        enqueue(ps->foreQueue, setProcess);
    }
}

/* freeSchedule
 * input: a schedule
 * output: none
 *
 * Free all of the memory associated with the schedule.
 */
void freeSchedule( schedule *ps ) {

    freeQueue(ps->foreQueue);
    freeQueue(ps->backQueue);
    free(ps);
}

int min( int x, int y ){
    if( x<y )
        return x;
    return y;
}
