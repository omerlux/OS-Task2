#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX 10

struct task
{
    int id;
    int priority;
    int arrival_time;
    int burst_time;
};

// -- QUEUE IMPLEMENTATION --
int intArray[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

void Qinit(){
    front = 0;
    rear = -1;
    itemCount = 0;
}

int Qpeek() {
    return intArray[front];
}

bool QisEmpty() {
    return itemCount == 0;
}

bool QisFull() {
    return itemCount == MAX;
}

int Qsize() {
    return itemCount;
}

void Qinsert(int data) {

    if(!QisFull()) {

        if(rear == MAX-1) {
            rear = -1;
        }

        intArray[++rear] = data;
        itemCount++;
    }
}

int QremoveData() {
    int data = intArray[front++];

    if(front == MAX) {
        front = 0;
    }

    itemCount--;
    return data;
}

int QremovePriorData(struct task * tasks){
    // this is liek removeData, only that we get the higher priority FIRST, and fix the queue if needed
    int highestPrior = 0;
    for (int i=front; i!=(rear+1)%MAX; i=(i+1)%MAX){
        int taskIndex = intArray[i];
        if (tasks[taskIndex].priority > highestPrior)
            highestPrior = tasks[taskIndex].priority;
    }
    // now we got the highest priority in highestPrior
    int queueIndex = front;
    // searching for the first task with priority = highestPrior:
    while(tasks[intArray[queueIndex]].priority != highestPrior){
        queueIndex = (queueIndex + 1)%MAX;
    }
    int taskIndex = intArray[queueIndex];
    // now we got the index of the task we want to return

    // fixing the array (from front to the task index)
    for (int i=(queueIndex-1)%MAX; i!=(front-1)%MAX; i=(i-1)%MAX){
        intArray[(i+1)%MAX] = intArray[i];
    }
    front = (front+1)%MAX;
    itemCount--;

    return taskIndex;
}
// -- END OF QUEUE IMPLEMENTATION --

// building Task structure for a string
struct task Build(char *line){
    char string[strlen(line)];
    strcpy(string, line);

    const char delim[3] = ",\n";          // define the delimiter for the line
    char *ptr;

    ptr = strtok(string, delim);

    int id = atoi(ptr);                 // Getting the ID
    ptr = strtok(NULL, delim);
    int priority = atoi(ptr);           // Getting the priority
    ptr = strtok(NULL, delim);
    int arrival_time = atoi(ptr);       // Getting the arrival time
    ptr = strtok(NULL, delim);
    int burst_time = atoi(ptr);         // Getting the burst time

    struct task task;                  // creating the task object
    task.id = id;
    task.priority = priority;
    task.arrival_time = arrival_time;
    task.burst_time = burst_time;
    return task;
}

// creating a Table of Tasks from a given file
struct task * Table (char *file_name){
    struct task* array = malloc(sizeof(struct task) * MAX);

    int bufferLength = 255;
    char buffer[bufferLength];

    char string[strlen(file_name)];
    strcpy(string, file_name);
    // Opening the file
    FILE* filePointer;
    filePointer = fopen(string, "r+");

    int i = 0;
    while(fgets(buffer, bufferLength, filePointer)) {
//        tasks[i] = Build(buffer);
        struct task tmp = Build(buffer);
        array[i].id = tmp.id;
        array[i].priority = tmp.priority;
        array[i].arrival_time = tmp.arrival_time;
        array[i].burst_time = tmp.burst_time;
        i++;
        //printf("%s\n", buffer);   //note: for testing
    }
    for(int j=i; j<MAX; j++){
        array[j].id = -1;
        array[j].priority = 0;
        array[j].arrival_time = 0;
        array[j].burst_time = 0;
    }
    fclose(filePointer);

    return array;
}

// copying Table of Tasks to a new instance
struct task * copyTable (struct task * tasks){
    struct task* copy = malloc(sizeof(struct task) * MAX);
    for(int i=0; i<MAX; i++){   // copying task table to edit it
        copy[i].id = tasks[i].id;
        copy[i].priority = tasks[i].priority;
        copy[i].arrival_time = tasks[i].arrival_time;
        copy[i].burst_time = tasks[i].burst_time;
    }
    return copy;
}

// printing array of Tasks
void Display(struct task* array){
    printf("===============PROCESS===TABLE===================\n");
    printf(" ID     | Priority | Arrival Time  | Burst Time \n");
    printf("--------+----------+---------------+------------\n");
    for(int i=0; i<MAX; i++){
        if(array[i].id==-1)
            break;
        printf(" %5d  | %7d  | %12d  | %8d  \n", array[i].id, array[i].priority, array[i].arrival_time, array[i].burst_time);
    }
    printf("=================================================\n");
}

enum Algorithm{
    First_Come_First_Serve=1,
    Shortest_Job_First=2,
    Priority=3,
    Round_Robin=4,
    Priority_With_Round_Robin=5
};

// get the number of tasks in a Table of Tasks
int getNumTasks(struct task* tasks){
    int j = 0;
    for(int i=0; i<MAX; i++){
        if(tasks[i].id!=-1) {
            j ++;
        }
    }
    return j;
}

// printing first arrived time task in tasks array
void printFirstCome(struct task* tasks){
    int min_arrival_time = 999999;
    int index = -1;
    for(int i=0; i<MAX; i++){       // finding the most recent arrival time
        if(tasks[i].arrival_time < min_arrival_time && tasks[i].id!=-1) {
            min_arrival_time = tasks[i].arrival_time;
            index = i;
        }
    }
    if(index >= 0){
        printf("<P%d,%d>", tasks[index].id, tasks[index].burst_time);
        tasks[index].id = -1;   // deactivate the task for future use
    }
}

// printing the shortest job in the tasks table for the current cpu time
int printShortFirst(struct task* tasks, int curr_time){
    int min_burst_time = 999999999;
    int index = -1;
    while((index == -1) && (curr_time <= 999999999)) {
        for (int i = 0; i < MAX; i++) {
            if (tasks[i].arrival_time <= curr_time && tasks[i].id != -1 &&  // first check if we can use do this job
                min_burst_time > tasks[i].burst_time) {                     // taking shortest job
                min_burst_time = tasks[i].burst_time;
                index = i;
            }
        }
        if (index >= 0) {
            printf("<P%d,%d>", tasks[index].id, tasks[index].burst_time);
            tasks[index].id = -1;                   // deactivate the task for future use
            curr_time += tasks[index].burst_time;   // updating the current cpu time by this task burst time
        } else {      // index is -1, then there arent any tasks RIGHT NOW...
            curr_time += 1; // clock is ticking
        }
    }
    return curr_time;
}

// printing the most prioritized job in the tasks table for the current cpu time
int printPriorityFirst(struct task* tasks, int curr_time){
    int max_priority = -1;
    int index = -1;
    while((index == -1) && (curr_time <= 999999999)) {
        for (int i = 0; i < MAX; i++) {
            if (tasks[i].arrival_time <= curr_time && tasks[i].id != -1 &&  // first check if we can use do this job
                max_priority < tasks[i].priority) {                     // taking max priority first
                max_priority = tasks[i].priority;
                index = i;
            }
        }
        if (index >= 0) {
            printf("<P%d,%d>", tasks[index].id, tasks[index].burst_time);
            tasks[index].id = -1;                   // deactivate the task for future use
            curr_time += tasks[index].burst_time;   // updating the current cpu time by this task burst time
        } else {      // index is -1, then there arent any tasks RIGHT NOW...
            curr_time += 1; // clock is ticking
        }
    }
    return curr_time;
}

// printing the task in a round robin manner
void printRoundRobin(struct task* tasks, int curr_time, int quantumTime, bool withPrior){
    int ntasks = getNumTasks(tasks);
    Qinit();
    bool busy = false;
    int busy_time = -1;
    int busy_index = -1;
    while( (!QisEmpty()) || (ntasks >0) || busy ){     // there are tasks in the table or in the queue that did not run yet or running right now...
        // adding tasks to the queue by curr_time
        for (int i = 0; i < MAX; i++) {
            if (tasks[i].arrival_time == curr_time && tasks[i].id != -1){  // adding the tasks in the curr_time tick
                Qinsert(i);         // in the Q - the indices of the tasks (the table)
                ntasks--;           // this task is gone
            }
        }
        // running job did not finished
        if(busy == true && busy_time > 0){
            curr_time ++;                                // continue ticking...
            busy_time --;                                // 1 time ticked for busy curr_time
        }
            // running job finished
        else if(busy == true && busy_time == 0){
            if(tasks[busy_index].burst_time > 0){       // the busy task did not end yet
                Qinsert(busy_index);    // inserting Queue
            }// else: busy task is over - do not add it back to the queue
            busy = false;
            // NOW - it will try to accomplish the busy==true cases
        }

        // no running job, and no one is waiting.
        if(busy == false && QisEmpty()){
            curr_time ++;           // continue ticking...
        }
        // no running job but there is one waiting
        else if(busy == false){
            // getting the next index in the queue
            if (withPrior){
                busy_index = QremovePriorData(tasks);   // removing priority task from queue
            }else {
                busy_index = QremoveData(); // removing from queue
            }
            busy = true;
            busy_time = (quantumTime < tasks[busy_index].burst_time) ? (quantumTime) : (tasks[busy_index].burst_time);
            tasks[busy_index].burst_time -= busy_time;   // updating the task
            printf("<P%d,%d>", tasks[busy_index].id, busy_time);
            curr_time ++;                                // continue ticking...
            busy_time --;                                // 1 time ticked for busy curr_time
        }
    }
}

void Schedule(struct task* tasks_orig, enum Algorithm alg, int Q){
    struct task* tasks = copyTable(tasks_orig);
    int curr_time = 0;                  // cpu current time
    int ntasks = getNumTasks(tasks);    // getting the number of tasks
    switch (alg) {
        case First_Come_First_Serve:
            // First_Come_First_Serve Algorithm
            printf("Scheduling Tasks - First Come First Serve Algorithm:\n");
            for(int i=0; i<ntasks; i++){
                printFirstCome(tasks);  // here we dont need to know curr_time. we need only burst time
            }
            printf("\n");
            break;
        case Shortest_Job_First:
            // Shortest_Job_First Algorithm
            printf("Scheduling Tasks - Shortest Job First Algorithm:\n");
            for(int i=0; i<ntasks; i++){
                curr_time = printShortFirst(tasks, curr_time);    // iterativly searching for the shortest job, in the times that are minimal than curr_time
            }
            printf("\n");
            break;
        case Priority:
            // Priority Algorithm - NON-PREEMPTIVE (static priority)
            printf("Scheduling Tasks - Priority (Non-Preemptive) Algorithm:\n");
            for(int i=0; i<ntasks; i++){
                curr_time = printPriorityFirst(tasks, curr_time);    // iterativly searching for the shortest job, in the times that are minimal than curr_time
            }
            printf("\n");
            break;
        case Round_Robin:
            // Round Robin Algorithm - NON-PREEMPTIVE (static priority)
            printf("Scheduling Tasks - Round Robin (Non-Preemptive) Algorithm:\n");
            printRoundRobin(tasks, curr_time, Q, false);    // false is for no priority
            printf("\n");
            break;
        case Priority_With_Round_Robin:
            // Priority with Round Robin Algorithm - NON-PREEMPTIVE (static priority)
            printf("Scheduling Tasks - Priority with Round Robin (Non-Preemptive) Algorithm:\n");
            printRoundRobin(tasks, curr_time, Q, true);     // true is for priority
            printf("\n");
            break;
    }
}

int main() {
    // TEST FOR BUILD
    char *line = "15, 13, 2, 5";
    struct task task = Build(line);
    printf("id '%d', priority '%d', arrival_time '%d', burst_time '%d'\n",
           task.id, task.priority, task.arrival_time, task.burst_time);

    // TESST FOR TABLE
    char *file_name = "../processes.txt";
    struct task *tasks = Table(file_name);

    // TEST FOR DISPLAY
    Display(tasks);

    // TEST FOR SCHEDULER
    file_name = "../processes.txt";
    tasks = Table(file_name);
    Schedule(tasks, First_Come_First_Serve, -1);
    file_name = "../processes_SJF.txt";
    tasks = Table(file_name);
    Schedule(tasks, Shortest_Job_First, -1);
    file_name = "../processes_prior.txt";
    tasks = Table(file_name);
    Schedule(tasks, Priority, -1);
    file_name = "../processes_roundrobin.txt";
    tasks = Table(file_name);
    Schedule(tasks, Round_Robin, 2);
    file_name = "../processes_priorroundrobin.txt";
    tasks = Table(file_name);
    Schedule(tasks, Priority_With_Round_Robin, 2);
    return 0;
}
