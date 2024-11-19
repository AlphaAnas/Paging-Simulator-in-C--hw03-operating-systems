#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>





// Page Table Entry
typedef struct _PTE {
    int frame_number;
    int valid;
    // int dirty; // 0 = clean, 1 = dirty
} PTE;
// Process Control Block
typedef struct _PCB {
    int process_id;
    int process_size;
    char *process_file_name;
    PTE *page_table;
    int num_pages;
} PCB;

PCB *pcb_list;

typedef struct _FreeFrameList {
    int *frames;
    int count;
} FreeFrameList;


void 
load_process(PCB *pcb, unsigned char *loader_buffer, FILE *file, int page_size) {

// – Process id (byte 0)
    unsigned char process_id = loader_buffer[0];
// – Code Segment size (bytes 1-2):
    unsigned short code_segment_size = loader_buffer[1] << 8 | loader_buffer[2]; // << 8 is equivalent to * 256
    printf("Code Segment size: %d bytes\n", code_segment_size);

// ∗ Next 48 bytes will be loaded in the code segment
    
// – Data Segment (Following the data segment size bytes)
    unsigned char code_segment[code_segment_size];
        for (int i = 0; i < code_segment_size; i++) {
            code_segment[i] = loader_buffer[3 + i];
        }

// – Data Segment size (2 bytes following the end of code segment):
    unsigned short data_segment_size = loader_buffer[3 + code_segment_size] << 8 | loader_buffer[4 + code_segment_size];
    printf("Data Segment size: %d bytes\n", data_segment_size);

    unsigned char data_segment[data_segment_size];
        for (int i = 0; i < data_segment_size; i++) {
            data_segment[i] = loader_buffer[5 + code_segment_size + i];
        }



// – End of Process: 0xFF marks the end of the file. If after the data segment bytes 0xFF

    pcb->process_id = process_id;
    pcb->process_size = code_segment_size + data_segment_size;
    pcb->num_pages = (pcb->process_size + page_size - 1) / page_size;  // Round up
    pcb->page_table = malloc(pcb->num_pages * sizeof(PTE));

    for (int i = 0; i < pcb->num_pages; ++i) {
        pcb->page_table[i].frame_number = -1;  // Initially no frame
        pcb->page_table[i].valid = 0;  // Initially mark as invalid
    }



   
};
void allocate_frames(PCB *pcb, int page_size, FreeFrameList *free_frames) {

// Map each logical page to a physical frame in memory.
    for (int i = 0; i < pcb->num_pages; ++i) {
        if (free_frames->count > 0) {
            //  Update the page table accordingly.//
            free_frames->count--;
            pcb->page_table[i].frame_number = free_frames->frames[free_frames->count];
            pcb->page_table[i].valid = 1;  // Mark as valid
        } else {
            fprintf(stderr, "Error: No free frames available.\n");
            exit(EXIT_FAILURE);
        }
    }
}

int calculate_internal_fragmentation(PCB *pcb, int page_size) {
    
    int total_size = pcb->process_size;
    int last_page_size = total_size % page_size;

    if (last_page_size == 0) {
        return 0; // No internal fragmentation if perfectly divisible
    }

    // Internal fragmentation is the unused space in the last page
    return page_size - last_page_size;
}

void print_buffer(unsigned char *buffer, size_t size) {
    printf("Content loaded into buffer:\n");
    for (size_t i = 0; i < size; ++i) {
        // convert to decimal: 
        printf("Byte %zu:  hex=0x%02X, dec=%d\n", i, buffer[i], buffer[i]);
    
    }
}
int main(int argc, char *argv[]) {

    if (argc < 5) {
            fprintf(stderr, "Usage: %s <physical memory size> <logical address size> <page size> <process files...>\n", argv[0]);
            return 1;
        }
    int p_mem_size = atoi(argv[1]);
    int logical_addr_size = atoi(argv[2]);
    int page_size = atoi(argv[3]);
    int num_processes = argc - 4;

    // • Divide the process address space into pages based on the given page size.
    int num_pages = logical_addr_size / page_size;
    // Divide the physical memory into frames based on the given frame size.
    int num_frames = p_mem_size / page_size;
    // • Maintain a list of all free frames - not being used by any process.
    FreeFrameList free_frames ;
    free_frames.frames = (int *)malloc(num_frames * sizeof(int));
    free_frames.count = num_frames;
    // memset(free_frames, 0, num_frames * sizeof(int)); // 0 means free
    for (int i = 0; i < num_frames; ++i) {
        free_frames.frames[i] = i; // Initialize free frames
    }

    int total_internal_fragmentation = 0;

    pcb_list = (PCB *)malloc(num_processes * sizeof(PCB));

        // read from the binary files
    for (int i_process = 0; i_process < num_processes; i_process++){
            printf("Process %d/%d\n", i_process+1, num_processes);
            // unsigned char buffer[10];
            unsigned char loader_buffer[95];
                FILE *ptr;
                char filename[25];
                sprintf(filename, "p%d.proc", i_process+1); 
                ptr = fopen(filename, "rb");  // r for read, b for binary
                if (ptr == NULL){
                    printf("Error opening file");
                    return 1;
                }

                // copy the file into the buffer
                fread(loader_buffer, 1, sizeof(loader_buffer), ptr);
                printf("File %s loaded into buffer\n", filename);
                // print_buffer(loader_buffer, sizeof(loader_buffer));
               
                // load the process

                pcb_list[i_process].process_file_name = filename;
                load_process(&pcb_list[i_process], loader_buffer, ptr, page_size);
                fclose(ptr);

//                Allocate Frames and Internal Fragmentation:
                allocate_frames(&pcb_list[i_process], page_size, &free_frames);

//              Calculate Internal Fragmentation
                int fragmentation = calculate_internal_fragmentation(&pcb_list[i_process], page_size);
                printf("Internal Fragmentation for Process ID %d: %d bytes\n", pcb_list[i_process].process_id, fragmentation);
                total_internal_fragmentation += fragmentation;


    }

   // Output results
    printf("Memory Dump:\n");
    for (int i = 0; i < num_processes; ++i) {
        printf("Process ID: %d, File: %s\n", pcb_list[i].process_id, pcb_list[i].process_file_name);
        for (int j = 0; j < pcb_list[i].num_pages; ++j) {
            printf("  Page %d -> Frame %d (Valid: %d)\n", j, pcb_list[i].page_table[j].frame_number, pcb_list[i].page_table[j].valid);
        }
    }

    printf("\nFree Frame List:\n");
    for (int i = 0; i < free_frames.count; ++i) {
        printf("  Frame %d  ", free_frames.frames[i]);
    }

    printf("\nTotal Internal Fragmentation: %d bytes\n", total_internal_fragmentation);

    // Clean up memory
    for (int i = 0; i < num_processes; ++i) {
        free(pcb_list[i].page_table);
    }
    free(pcb_list);
    free(free_frames.frames);



    return 0;
}
