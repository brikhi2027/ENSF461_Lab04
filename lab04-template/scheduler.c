#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>
#define min(a,b) (((a)<(b))?(a):(b))

// total jobs
int numofjobs = 0;

struct job {
    // job id is ordered by the arrival; jobs arrived first have smaller job id, always increment by 1
    int id;
    int arrival; // arrival time; safely assume the time unit has the minimal increment of 1
    int length;
    int tickets; // number of tickets for lottery scheduling
    // TODO: add any other metadata you need to track here
    int execution_time_remaining;
    int start_time;
    int completion_time;
    struct job *next;
};

// the workload list
struct job *head = NULL;


void append_to(struct job **head_pointer, int arrival, int length, int tickets, int id_number){

    // TODO: create a new job and init it with proper data
    struct job* new_job = (struct job*)malloc(sizeof(struct job));
    new_job -> id = id_number;
    new_job -> arrival = arrival;
    new_job -> length = length;
    new_job -> tickets = tickets;
    new_job -> execution_time_remaining = length;
    new_job -> start_time = -1;
    new_job -> completion_time = -1;
    new_job -> next = NULL;

    if (*head_pointer == NULL){
        *head_pointer = new_job;
	numofjobs++;
        return;
    }
    struct job* tail_pointer = *head_pointer;
    while (tail_pointer -> next != NULL){
        tail_pointer = tail_pointer->next;
    }
    tail_pointer->next = new_job;
    numofjobs++;

    return;
}


void read_job_config(const char* filename)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int tickets  = 0;
    int id_number = 0;

    char* delim = ",";
    char *arrival = NULL;
    char *length = NULL;

    // TODO, error checking
    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    // TODO: if the file is empty, we should just exit with error
    //if ((read = getline(&line, &len, fp)) == -1){
        //fclose(fp);
        //free(line);
        //exit(EXIT_FAILURE);
    // }
    while ((read = getline(&line, &len, fp)) != -1)
    {
        if( line[read-1] == '\n' )
            line[read-1] =0;
        arrival = strtok(line, delim);
        length = strtok(NULL, delim);
        tickets += 100;

        append_to(&head, atoi(arrival), atoi(length), tickets, id_number);
	id_number++;
    }

    fclose(fp);
    if (line) free(line);
}


void policy_SJF()
{
    printf("Execution trace with SJF:\n");
    // TODO: implement SJF policy
    
    int timer = 0;
    struct job* temp = head;
    struct job* timeFinder = head;
    int compIDs[numofjobs];
    compIDs[0] = -1;
    int jobsFin = 0;
    int shortestTime;
    bool start = false;
    shortestTime = -1;
    
    while (jobsFin < numofjobs)
    {
        if (jobsFin != 0){

            // id checker
            bool notfinished = true;
            int length = sizeof(compIDs) / sizeof(compIDs[0]);
            for (int i = 0; i < length; i++)
            {
                if (compIDs[i] == temp->id){
                    notfinished = false;
                    break;
                }
            }
            if (notfinished){
                if (temp->arrival <= timer){
                    if (shortestTime > temp->length || shortestTime == -1){
                        shortestTime = temp->length;
                        timeFinder = temp;
                    }
                } 
            }
        } else {
            if (temp->arrival <= timer){
                if (shortestTime > temp->length || shortestTime == -1){
                    shortestTime = temp->length;
                    timeFinder = temp;
                }
            }
        }
        if (start){
            if (shortestTime == -1){
                timer++;
                start = false;
                temp = head;
                continue;
            }
            temp = head;
            // printf("instart\n");
            while(temp != NULL){
                if (temp->id == timeFinder->id){
                    printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", timer, temp->id, temp->arrival, temp-> length);
                    timer += temp->length;
                    compIDs[jobsFin] = temp->id;
                    // printf("%d" , compIDs[jobsFin]);
                    jobsFin++;
                    start = false;
                    temp = head;
                    shortestTime = -1;
                    break;
                }
                temp = temp->next;
            }
        } else {
            temp = temp->next;
            if (temp == NULL){
                temp = head;
                start = true;
            }
        }
    }
    
        
    

    printf("End of execution with SJF.\n");

}


void policy_STCF()
{
    printf("Execution trace with STCF:\n");

    // TODO: implement STCF policy
    int curr_time = 0;
    int completed_jobs = 0;
    while (completed_jobs < numofjobs){
        struct job* curr_job = head;
        struct job* shortest_job = NULL;
        while (curr_job != NULL){
            if (curr_job->arrival <= curr_time && curr_job->execution_time_remaining > 0){
                if (shortest_job == NULL || curr_job->execution_time_remaining < shortest_job->execution_time_remaining){
                    shortest_job = curr_job;
                }
            }
            curr_job = curr_job->next;
        }
        if (shortest_job == NULL){
            curr_job = head;
            int next_job_arrival = 10000;
            while (curr_job != NULL){
                if (curr_job->arrival > curr_time && curr_job->execution_time_remaining > 0){
                    if (curr_job->arrival < next_job_arrival){
                        next_job_arrival = curr_job->arrival;
                    }
                }
                curr_job = curr_job->next;
            }
            curr_time = next_job_arrival;
            continue;
        }

        struct job* next_job = shortest_job->next;
        int running_time = shortest_job->execution_time_remaining;
        if (next_job != NULL && next_job->arrival > curr_time){
            int time_until_next_job = next_job->arrival - curr_time;
            if (time_until_next_job < running_time){
                running_time = time_until_next_job;
            }
        }
        if (shortest_job->start_time == -1){
            shortest_job->start_time = curr_time;
        }
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", curr_time, shortest_job->id, shortest_job->arrival, running_time);
        curr_time += running_time;
        shortest_job->execution_time_remaining -= running_time;
        if (shortest_job->execution_time_remaining == 0){
            shortest_job->completion_time = curr_time;
            completed_jobs++;
        }
        
    }
    

    printf("End of execution with STCF.\n");
}


void policy_RR(int slice) {
    printf("Execution trace with RR:\n");

    int current_time = 0;
    struct job* ready_queue[numofjobs];
    int queue_start = 0;
    int queue_end = 0;

    // Load initial jobs that arrive at time 0
    struct job* current_job = head;
    while (current_job != NULL) {
        if (current_job->arrival <= current_time) {
            ready_queue[queue_end++] = current_job;
        }
        current_job = current_job->next;
    }

    while (queue_start < queue_end) {
        struct job* job = ready_queue[queue_start++];

        // Set start time if this is the first execution of the job
        if (job->start_time == -1) {
            job->start_time = current_time;
        }

       
        
        

        // Run the job for a time slice or the remaining time, whichever is less
        int execution_time = min(slice, job->execution_time_remaining);
        job->execution_time_remaining -= execution_time;
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", 
               current_time, job->id, job->arrival, execution_time);
        current_time += execution_time;

        // Add any new arrivals during the execution to the end of the queue
        current_job = head;
        while (current_job != NULL) {
            if (current_job->arrival <= current_time &&
                current_job->execution_time_remaining > 0 &&
                current_job->start_time == -1) {
                bool in_queue = false;
                for (int i = queue_start; i < queue_end; i++) {
                    if (ready_queue[i] == current_job) {
                        in_queue = true;
                        break;
                    }
                }
                if (!in_queue) {
                    ready_queue[queue_end++] = current_job;
                }
            }
            current_job = current_job->next;
        }

        // If job has remaining time, re-enqueue it for another round
        if (job->execution_time_remaining > 0) {
            ready_queue[queue_end++] = job;
        } else {
            // Mark completion for jobs that finish
            job->completion_time = current_time;
        }

        // If the queue becomes empty, idle until the next job arrives
        if (queue_start >= queue_end) {
            int next_arrival = INT_MAX;
            current_job = head;
            while (current_job != NULL) {
                if (current_job->arrival > current_time &&
                    current_job->execution_time_remaining > 0) {
                    next_arrival = min(next_arrival, current_job->arrival);
                }
                current_job = current_job->next;
            }
            if (next_arrival != INT_MAX) {
                current_time = next_arrival;
                current_job = head;
                while (current_job != NULL) {
                    if (current_job->arrival <= current_time &&
                        current_job->execution_time_remaining > 0 &&
                        current_job->start_time == -1) {
                        ready_queue[queue_end++] = current_job;
                    }
                    current_job = current_job->next;
                }
            }
        }
    }

    printf("End of execution with RR.\n");
}


void policy_LT(int slice)
{
    printf("Execution trace with LT:\n");

    // Leave this here, it will ensure the scheduling behavior remains deterministic
    srand(42);

    // In the following, you'll need to:
    // Figure out which active job to run first
    // Pick the job with the shortest remaining time
    // Considers jobs in order of arrival, so implicitly breaks ties by choosing the job with the lowest ID

    // To achieve consistency with the tests, you are encouraged to choose the winning ticket as follows:
    // int winning_ticket = rand() % total_tickets;
    // And pick the winning job using the linked list approach discussed in class, or equivalent

    printf("End of execution with LT.\n");

}


void policy_FIFO(){
    printf("Execution trace with FIFO:\n");

    // TODO: implement FIFO policy
    struct job* curr_job = head;
    int curr_time = 0;

    while (curr_job != NULL){
        if (curr_job->arrival > curr_time){
            curr_time = curr_job->arrival;
        }
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", curr_time, curr_job->id, curr_job->arrival, curr_job->length);
        curr_time += curr_job->length; 
        curr_job = curr_job->next;
    }

    printf("End of execution with FIFO.\n");
}


int main(int argc, char **argv){

    static char usage[] = "usage: %s analysis policy slice trace\n";

    int analysis;
    char *pname;
    char *tname;
    int slice;


    if (argc < 5)
    {
        fprintf(stderr, "missing variables\n");
        fprintf(stderr, usage, argv[0]);
		exit(1);
    }

    // if 0, we don't analysis the performance
    analysis = atoi(argv[1]);

    // policy name
    pname = argv[2];

    // time slice, only valid for RR
    slice = atoi(argv[3]);

    // workload trace
    tname = argv[4];

    read_job_config(tname);

    if (strcmp(pname, "FIFO") == 0){
        policy_FIFO();
        if (analysis == 1){
            // TODO: perform analysis
	    printf("Begin analyzing FIFO:\n");
            struct job* curr_job = head;
            int curr_time = 0;
            int response_time;
            int turnaround;
            int wait_time;
            int response_time_sum = 0;
            int turnaround_sum = 0;
            int wait_time_sum = 0;
            while (curr_job != NULL){
                if (curr_job->arrival > curr_time){
                    curr_time = curr_job->arrival;
                    response_time = 0;
                    turnaround = curr_job->length;
                    wait_time = response_time;
                }
                else{
                    response_time = curr_time - curr_job->arrival;
                    turnaround = curr_time + curr_job->length - curr_job->arrival;
                    wait_time = response_time;
                }
                response_time_sum += response_time;
                turnaround_sum += turnaround;
                wait_time_sum += wait_time;
                printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", curr_job->id, response_time, turnaround, wait_time);
                curr_time += curr_job->length; 

                curr_job = curr_job->next;
            }
            printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n", (float)response_time_sum/numofjobs, (float)turnaround_sum/numofjobs, (float)wait_time_sum/numofjobs);
            printf("End analyzing FIFO.\n");
        }
    }
    else if (strcmp(pname, "SJF") == 0)
    {
        // TODO
        policy_SJF();
        if (analysis == 1){
            printf("Begin analyzing SJF:\n");
            struct job* temp = head;
            int timer = 0;
            int currentJob = 0;
            int response_time[numofjobs];
            int turnaround[numofjobs];
            int wait_time[numofjobs];
            int response_time_sum = 0;
            int turnaround_sum = 0;
            int wait_time_sum = 0;

            struct job* timeFinder = head;
            int compIDs[numofjobs];
            compIDs[0] = -1;
            int jobsFin = 0;
            int shortestTime;
            bool start = false;
            shortestTime = -1;

            while (jobsFin < numofjobs)
            {
                if (jobsFin != 0){

                    // id checker
                    bool notfinished = true;
                    int length = sizeof(compIDs) / sizeof(compIDs[0]);
                    for (int i = 0; i < length; i++)
                    {
                        if (compIDs[i] == temp->id){
                            notfinished = false;
                            break;
                        }
                    }
                    if (notfinished){
                        if (temp->arrival <= timer){
                            if (shortestTime > temp->length || shortestTime == -1){
                                response_time[currentJob] = timer-temp->arrival;
                                turnaround[currentJob] = temp->length + response_time[currentJob];
                                wait_time[currentJob] = response_time[currentJob];
                                shortestTime = temp->length;
                                timeFinder = temp;
                            }
                        } 
                    }
                } else {
                    if (temp->arrival <= timer){
                        if (shortestTime > temp->length || shortestTime == -1){
                            response_time[currentJob] = timer-temp->arrival;
                            turnaround[currentJob] = temp->length + response_time[currentJob];
                            wait_time[currentJob] = response_time[currentJob];
                            shortestTime = temp->length;
                            timeFinder = temp;
                        }
                    }
                }
                if (start){
                    if (shortestTime == -1){
                        timer++;
                        start = false;
                        temp = head;
                        continue;
                    }
                    temp = head;
                    while(temp != NULL){
                        if (temp->id == timeFinder->id){
                            timer += temp->length; 
                            compIDs[jobsFin] = temp->id;
                            jobsFin++;
                            start = false;
                            temp = head;
                            shortestTime = -1;
                            currentJob++;
                            break;
                        }
                        temp = temp->next;
                    }
                } else {
                    temp = temp->next;
                    if (temp == NULL){
                        temp = head;
                        start = true;
                    }
                }
            }

            for (int i = 0; i < numofjobs; i++)
            {
                response_time_sum += response_time[i];
            }
            for (int i = 0; i < numofjobs; i++)
            {
                turnaround_sum += turnaround[i];
            }
            for (int i = 0; i < numofjobs; i++)
            {
                wait_time_sum += wait_time[i];
            }

            for (int i = 0; i < numofjobs; i++)
            {
                for (int  j = 0; j < numofjobs; j++)
                {
                    if (i == compIDs[j]){
                        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", i, response_time[j], 
                        turnaround[j], wait_time[j]);
                        break;
                    }
                    
                }
            }  
            printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n", (float)response_time_sum/numofjobs, (float)turnaround_sum/numofjobs, (float)wait_time_sum/numofjobs);
            printf("End analyzing SJF.\n");

        }
    }
    else if (strcmp(pname, "STCF") == 0)
    {
        // TODO
	policy_STCF();
        if (analysis == 1){
            printf("Begin analyzing STCF:\n");
            struct job* curr_job = head;
            int response_time;
            int turnaround;
            int wait_time;
            int response_time_sum = 0;
            int turnaround_sum;
            int wait_time_sum;
            while (curr_job != NULL){
                response_time = curr_job->start_time - curr_job->arrival;
                turnaround = curr_job->completion_time - curr_job->arrival;
                wait_time = turnaround - curr_job->length;
                
                response_time_sum += response_time;
                turnaround_sum += turnaround;
                wait_time_sum += wait_time;

                printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", curr_job->id, response_time, turnaround, wait_time);
                curr_job = curr_job->next;
            }
            printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n", (float)response_time_sum/numofjobs, (float)turnaround_sum/numofjobs, (float)wait_time_sum/numofjobs);
            printf("End analyzing STCF.\n");

        }
    }
    else if (strcmp(pname, "RR") == 0) {
        policy_RR(slice);
        if (analysis == 1) {
            printf("Begin analyzing RR:\n");

            struct job* curr_job = head;
            int response_time;
            int turnaround;
            int wait_time;
            int response_time_sum = 0;
            int turnaround_sum = 0;
            int wait_time_sum = 0;

            while (curr_job != NULL) {
            // Response time is the first time the job runs minus its arrival time
                response_time = curr_job->start_time - curr_job->arrival;
                if (response_time < 0) response_time = 0;  // Ensure no negative response times

            // Turnaround time is the job's completion time minus its arrival time
                turnaround = curr_job->completion_time - curr_job->arrival;

            // Wait time is turnaround time minus the job's length
                wait_time = turnaround - curr_job->length;

                response_time_sum += response_time;
                turnaround_sum += turnaround;
                wait_time_sum += wait_time;

                printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
                   curr_job->id, response_time, turnaround, wait_time);

                curr_job = curr_job->next;
            }

            printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n",
               (float)response_time_sum / numofjobs,
               (float)turnaround_sum / numofjobs,
               (float)wait_time_sum / numofjobs);
            printf("End analyzing RR.\n");
        }
    }
    else if (strcmp(pname, "LT") == 0)
    {
        // TODO
    }

	exit(0);
}
