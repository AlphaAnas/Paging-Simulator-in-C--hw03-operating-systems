#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Process Management
// • The simulator must maintain a dynamic list of PCBs (process control blocks) which will
// have at least the following attributes:
// – Process id
// – Process size (code + data)
// – Process file name
// – Page table (must be a list of struct PTE)
// • When a process file is to be loaded, create a new PCB and add it to the ready queue.
// • Hint: A ready queue can simply be a dynamic array as the max no. of processes will be
// known when the simulator runs (through the command line arguments count.

// ready_queue is a dynamic array of PCBs
int len = 100;
PCB *ready_queue;

typedef struct PCB {
    int process_id;
    int process_size;
    char *process_file_name;
    PTE *page_table;
} PCB;
typedef struct PTE {
    int frame_number;
    int valid;
    int dirty;
} PTE;


int main() {
    ready_queue = (PCB *)malloc(len * sizeof(PCB));
    // Your code here

    return 0;
}

