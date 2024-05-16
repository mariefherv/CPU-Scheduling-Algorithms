// Lab 6: CPU Scheduling algorithms by Villanueva, Mariefher Grace Z.

#include <stdio.h>
#include <stdlib.h> // for qsort, malloc, realloc
#include <stdbool.h> // for booleans

struct Process {
    int arrival, burst, turnaround, wait, priority, start;
    char name[10];
    bool finished;
    int *burst_times; // dynamic array
    int *end_times; // dynamic array
    int burst_count;  // size of dynamic array
    int end_count;  // size of dynamic array
};

// function that compares arrival time of processes
int compareArrival(const void *a, const void *b) {
    const struct Process *processA = (const struct Process *)a;
    const struct Process *processB = (const struct Process *)b;
    return processA->arrival - processB->arrival;
}

// function to get max burst times
int getMaxBurst(struct Process* proc, int num_processes) {
    int max_burst = 0;
    for (int i = 0; i < num_processes; i++) {
        if (proc[i].burst > max_burst) {
            max_burst = proc[i].burst;
        }
    }
    return max_burst + 1;
}

// function to get lowest priority
int getLowestPriority(struct Process* proc, int num_processes) {
    int lowest_priority = 0;
    for (int i = 0; i < num_processes; i++) {
        if (proc[i].priority > lowest_priority) {
            lowest_priority = proc[i].priority;
        }
    }
    return lowest_priority + 1;
}

// search for burst time in array burst_times
int searchBurstTime(struct Process *p, int burst_time) {
    for (int i = 0; i < p->burst_count; i++) {
        if (p->burst_times[i] == burst_time) {
            return 1; // return 1 if the element is found
        }
    }
    return 0; // return 0 if the element is not found
}

// function to add dynamic array burst_times
void addBurstTime(struct Process *p, int burst_time) {
    // reallocate memory to hold the new element
    p->burst_times = realloc(p->burst_times, (p->burst_count + 1) * sizeof(int));
    // check if memory allocation was successful
    if (p->burst_times == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    // ddd the new burst time to the array
    p->burst_times[p->burst_count] = burst_time;
    // add burst_count (size)
    p->burst_count++;
}

// function to add dynamic array end_times
void addEndTimes(struct Process *p, int end_time) {
    // reallocate memory to hold the new element
    p->end_times = realloc(p->end_times, (p->end_count + 1) * sizeof(int));
    // check if memory allocation was successful
    if (p->end_times == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    // ddd the new burst time to the array
    p->end_times[p->burst_count] = end_time;
    // add burst_count (size)
    p->end_count++;
}

// to free the dynamically allocated memory used in dynamic arrays
void freeProcessBurst(struct Process *p) {
    free(p->burst_times); 
}

void freeProcessEnd(struct Process *p) {
    free(p->end_times);
}

// function declarations
void get_details(struct Process* proc, int num_processes, int choice);
void FCFS(struct Process* proc, int num_processes);
void SJF(struct Process* proc, int num_processes);
void SRTF(struct Process* proc, int num_processes);
void Priority(struct Process* proc, int num_processes);
void RoundRobin(struct Process* proc, int num_processes, int time_slice);


int main() {
    printf("\n\n\t--- SCHEDULING ALGORITHMS ---\n");
    printf("\t1. FCFS\n");
    printf("\t2. SJF (Nonpreemptive)\n");
    printf("\t3. SRTF (SJF Preemptive)\n");
    printf("\t4. Priority Based (Preemptive)\n");
    printf("\t5. Round Robin (Preemptive)\n");
    printf("\t6. EXIT\n\n");
    
    int choice;
    printf("\tEnter your choice: ");
    scanf("%d", &choice);
    
    int num_processes;
    printf("\tEnter number of processes: ");
    scanf("%d", &num_processes);
    
    struct Process proc[num_processes];
    struct Process* sptr = proc;        // pointer for array of processes (proc)
    
    get_details(sptr, num_processes, choice);

    // execute functions based on choice
    if (choice == 1) {
        FCFS(sptr, num_processes);
    } else if (choice == 2) {
        SJF(sptr, num_processes);
    } else if (choice == 3) {
        SRTF(sptr, num_processes);
    } else if (choice == 4) {
        Priority(sptr, num_processes);
    } else if (choice == 5) {
        int time_slice;
        printf("\n\tEnter time slice value: ");
        scanf("%d", &time_slice);
        RoundRobin(sptr, num_processes, time_slice);
    }

    // Free dynamically allocated memory for each process
    for (int i = 0; i < num_processes; i++) {
        freeProcessBurst(&proc[i]);
        freeProcessEnd(&proc[i]);
    }

    return 0;
}

// Method to get arrival and burst times
void get_details(struct Process* proc, int num_processes, int choice) {
    printf("\n\tENTER THE VALUES\n");
    printf("\tEnter for each process: [Arrival Time] [Burst Time]\n");
    
    for (int i = 0; i < num_processes; i++) {
        printf("\tEnter for Process %d: ", i);
        scanf("%d %d", &proc[i].arrival, &proc[i].burst);
        sprintf(proc[i].name, "P%d", i);

        // initialization of values and dynamic arrays
        proc[i].finished = false;
        proc[i].burst_times = NULL;
        proc[i].burst_count = 0;   
        proc[i].end_times = NULL; 
        proc[i].end_count = 0;    
    }

    if (choice == 4) {
        printf("\n\n\t--- Prioritize processes from 1 (highest priority) to 5 (lowest priority) ---\n");
        for (int i = 0; i < num_processes; i++) {
            printf("\tEnter priority number for %s: ", proc[i].name);
            scanf("%d", &proc[i].priority);
        }
    }
}

// to display gantt chart for nonpreemptive
void display(struct Process* proc, int num_processes) {
    int total_wait = 0;
    int total_turnaround = 0;

    for (int i = 0; i < num_processes; i++) {
        total_wait += proc[i].wait;
        total_turnaround += proc[i].turnaround;
    }

    // calculate for average wait and average turnaround time
    float ave_wait = total_wait / (float)num_processes;
    float ave_turnaround = total_turnaround / (float)num_processes;

	printf("\n\t||  Average turnaround time: %.2f  ||\n\t||  Average waiting time: %.2f  ||\n", ave_turnaround, ave_wait);

    
    // Display the gantt chart
    printf("\n\tGANTT CHART\n");
    for (int i = 0; i < num_processes; i++) {
        printf("\t%s", proc[i].name);
    }
    printf("\n");
    
    for (int i = 0; i < num_processes; i++) {
        printf("\t %d", proc[i].start);
        if (i == num_processes - 1) {
            printf("\t %d", proc[i].start + proc[i].burst);
        }
    }
    printf("\n\n");
}

// to display gantt chart for preemptive
void displayPreemptive(struct Process* proc, int num_processes, int turnaround) {
	int total_wait = 0;
    int total_turnaround = 0;

    for (int i = 0; i < num_processes; i++) {
        total_wait += proc[i].wait;
        total_turnaround += proc[i].turnaround;
    }

	// calculate for average wait and average turnaround time
    float ave_wait = total_wait / (float)num_processes;
    float ave_turnaround = total_turnaround / (float)num_processes;

	printf("\n\t||  Average turnaround time: %.2f  ||\n\t||  Average waiting time: %.2f  ||\n", ave_turnaround, ave_wait);

    printf("\n\tGANTT CHART\n");
    for (int i = 0; i < turnaround; i++) {
        for (int j = 0; j < num_processes; j++) {
            if (searchBurstTime(&proc[j], i)) {
                printf("\t%s", proc[j].name);
            }
        }
    }
    printf("\n");
    for (int i = 0; i < turnaround; i++) {
        for (int j = 0; j < num_processes; j++) {
            if (searchBurstTime(&proc[j], i)) {
                printf("\t%i", i);
            }
        }
		if(i == turnaround-1) {
			printf("\t%i", i+1); // for the final turnaround time
		}
    }

    printf("\n\n");
}

// First-come first-served scheduling implementation w/ arrival time
void FCFS(struct Process* proc, int num_processes) {
    printf("\n\t--- First Come First Serve ---\n");

    qsort(proc, num_processes, sizeof(struct Process), compareArrival); // sort all processes first by their arrival time

    // Find each process's waiting time and turnaround time
    int prev_end = 0;
    int total_wait = 0;
    int total_turnaround = 0;
    
    for (int i = 0; i < num_processes; i++) {
        proc[i].start = prev_end;
        proc[i].wait = prev_end - proc[i].arrival;
        prev_end += proc[i].burst;
        proc[i].turnaround = prev_end - proc[i].arrival;
        total_wait += proc[i].wait;
        total_turnaround += proc[i].turnaround;
    }

    display(proc, num_processes);
}


// Implement Shortest Job First (nonpreemptive)
void SJF(struct Process* proc, int num_processes) {

	printf("\n\t--- Shortest Job First (Nonpreemptive) ---\n");

	qsort(proc, num_processes, sizeof(struct Process), compareArrival); //sort all processes first by their arrival time
	
	// Find each process's waiting time and turnaround time
	int prev_end = 0;
	int i, j;
	
	for (i = 0; i < num_processes; i++){
		// sort arrived processes by shortest job
		for(j = num_processes-1; j > i; j--) {
			// if process j has arrived and has less burst time than the process before
			if(proc[j].arrival <= prev_end && proc[j].burst < proc[j-1].burst) {
				struct Process temp = proc[j];
				proc[j] = proc[j-1];
				proc[j-1] = temp;
			}
		}

		// set new values for process i
		proc[i].start = prev_end;
		proc[i].wait = prev_end - proc[i].arrival;
		prev_end += proc[i].burst;
		proc[i].turnaround = prev_end - proc[i].arrival;
	}

	display(proc, num_processes);
};

// Implement Shortest Remaining Time First (preemptive)
void SRTF(struct Process* proc, int num_processes) {
    printf("\n\t--- Shortest Time Remaining First (Preemptive) ---\n");

    qsort(proc, num_processes, sizeof(struct Process), compareArrival); // sort all processes first by their arrival time
    
    int prev_end = 0;
    int i, j;
    int success = 0;
    int index = -1;
    int prev_index;

    while (success < num_processes) {
        // set max burst time (+1) as initial shortest burst time
        int shortest_burst = getMaxBurst(proc, num_processes);
        prev_index = index;

        // find the shortest job that has arrived
        for (i = 0; i < num_processes; i++) {
            // if process i has arrived and has less burst time than the process before
            if (proc[i].arrival <= prev_end && proc[i].burst < shortest_burst && !proc[i].finished) {
                shortest_burst = proc[i].burst;
                index = i;
            }
        }

        // if there is context switch
        if (index != prev_index) {
            if(prev_index != -1) addEndTimes(&proc[prev_index], prev_end); // set end time of previous process that bursted
            addBurstTime(&proc[index], prev_end); // add new burst time to current process
        }

        // subtract burst times
        proc[index].burst--;
		prev_end++;
		
        if (proc[index].burst == 0) { // process reaches 0 burst
            proc[index].finished = true;
            proc[index].turnaround = prev_end - proc[index].arrival;

			// if process has many burst counts
            if (proc[index].burst_count > 1) {
                proc[index].wait = proc[index].burst_times[0] - proc[index].arrival; // initial wait time from arrival to first burst
                for (int i = 1; i < proc[index].burst_count; i++) { // starting from index 1
                    proc[index].wait += (proc[index].burst_times[i] - (proc[index].end_times[i])); // calculate wait time between bursts
                }
            } else { // if process only had a one time burst
                proc[index].wait = proc[index].burst_times[0] - proc[index].arrival;
            }
        success++;
        }
    }

    displayPreemptive(proc, num_processes, prev_end);
}


// Priority Based (preemptive)
void Priority(struct Process* proc, int num_processes) {

	printf("\n\t--- Priority Based (Preemptive) ---\n");

    qsort(proc, num_processes, sizeof(struct Process), compareArrival); // sort all processes first by their arrival time
    
    int prev_end = 0;
    int i, j;
    int success = 0;
    int index = -1;
    int prev_index;

    while (success < num_processes) {
        // set lowest priority (+1) as initial priority
        int highest_priority = getLowestPriority(proc, num_processes);
		int shortest_burst = getMaxBurst(proc, num_processes);
        prev_index = index;

        // find the shortest job that has arrived
        for (i = 0; i < num_processes; i++) {
            // if process i has arrived and has less burst time than the process before
            if (proc[i].arrival <= prev_end && proc[i].burst < shortest_burst && !proc[i].finished && proc[i].priority < highest_priority) {
                shortest_burst = proc[i].burst;
				highest_priority = proc[i].priority;
                index = i;
            }
        }

        // if there is context switch
        if (index != prev_index) {
            if(prev_index != -1) addEndTimes(&proc[prev_index], prev_end); // set end time of previous process that bursted
            addBurstTime(&proc[index], prev_end); // add new burst time to current process
        }

        // subtract burst times
        proc[index].burst--;
		prev_end++;
		
        if (proc[index].burst == 0) { // process reaches 0 burst
            proc[index].finished = true;
            proc[index].turnaround = prev_end - proc[index].arrival;
            
            // if process has many burst counts
            if (proc[index].burst_count > 1) {
                proc[index].wait = proc[index].burst_times[0] - proc[index].arrival; // initial wait time from arrival to first burst
                for (int i = 1; i < proc[index].burst_count; i++) { // starting from index 1
                    proc[index].wait += (proc[index].burst_times[i] - (proc[index].end_times[i])); // calculate wait time between bursts
                }
            } else { // if process only had a one time burst
                proc[index].wait = proc[index].burst_times[0] - proc[index].arrival;
            }
        success++;
        }
    }
    displayPreemptive(proc, num_processes, prev_end);
};

// Round robin (preemptive)
void RoundRobin(struct Process* proc, int num_processes, int time_slice) {

	printf("\n\t--- Round Robin (Preemptive) ---\n");
    
    int prev_end = 0;
    int i;
    int success = 0;
    int index = -1;
    int prev_index;

    while (success < num_processes) {
    
        // iterate through each process
        for (i = 0; i < num_processes; i++) {
            prev_index = index;

            // if process i has arrived and not yet finished
            if (proc[i].arrival <= prev_end && !proc[i].finished) {
                index = i;

                // if there is context switch
                if (index != prev_index) {
                    if(prev_index != -1) addEndTimes(&proc[prev_index], prev_end); // set end time of previous process that bursted
                    addBurstTime(&proc[index], prev_end); // add new burst time to current process
                }

                // subtract burst times
                if(proc[index].burst >= time_slice) {
                    proc[index].burst -= time_slice;
                    prev_end += time_slice;
                } else {
                    prev_end += proc[index].burst;
                    proc[index].burst = 0;
                }                

                if (proc[index].burst == 0) { // process reaches 0 burst
                    proc[index].finished = true;
                    proc[index].turnaround = prev_end - proc[index].arrival;
                    success++;

                    // if process has many burst counts
                    if (proc[index].burst_count > 1) {
                        proc[index].wait = proc[index].burst_times[0] - proc[index].arrival; // initial wait time from arrival to first burst
                        for (int i = 1; i < proc[index].burst_count; i++) { // starting from index 1
                            proc[index].wait += (proc[index].burst_times[i] - (proc[index].end_times[i])); // calculate wait time between bursts
                        }
                    } else { // if process only had a one time burst
                        proc[index].wait = proc[index].burst_times[0] - proc[index].arrival;
                    }
                }
            }
        }
    }

    displayPreemptive(proc, num_processes, prev_end);
};