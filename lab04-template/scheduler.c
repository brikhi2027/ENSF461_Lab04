#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <time.h>

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
    }
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

    printf("End of execution with SJF.\n");

}


void policy_STCF()
{
    printf("Execution trace with STCF:\n");

    // TODO: implement STCF policy

    printf("End of execution with STCF.\n");
}


void policy_RR(int slice)
{
    printf("Execution trace with RR:\n");

    // TODO: implement RR policy

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
    }
    else if (strcmp(pname, "STCF") == 0)
    {
        // TODO
    }
    else if (strcmp(pname, "RR") == 0)
    {
        // TODO
    }
    else if (strcmp(pname, "LT") == 0)
    {
        // TODO
    }

	exit(0);
}
