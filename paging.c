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
// ∗ Next 48 bytes will be loaded in the code segment
    fseek(file, code_segment_size, SEEK_CUR); // skip the code segment

// – Data Segment size (2 bytes following the end of code segment):
    unsigned short data_segment_size = loader_buffer[3] << 8 | loader_buffer[4]; // << 8 is equivalent to * 256

// – Data Segment (Following the data segment size bytes)

    fseek(file, data_segment_size, SEEK_CUR); // skip the data segment
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
            unsigned char buffer[10];
            unsigned char loader_buffer[95];
                FILE *ptr;
                char filename[25];
                sprintf(filename, "p%d.proc", i_process+1); 
                ptr = fopen(filename, "rb");  // r for read, b for binary
                if (ptr == NULL){
                    printf("Error opening file");
                    return 1;
                }
                size_t result = 0;
                int b = 0;
                // 0xFF marks the end of the file.
                int counter = 0;
                bool signal = false;
                while ((result = fread(buffer,1, sizeof(buffer),ptr)) > 0){

      
                    for(size_t i = 0; i < result; i++){

                        printf("Byte %d:  hex=0x%02X\n", b, buffer[i]);
                    // copy the buffer into main loader buffer
                        loader_buffer[b] = buffer[i];
                        b++;

                        if (buffer[i] == 0xFF){
                            printf("End of file reached\n");
                            // fclose(ptr);
                            signal = true;
                   
                            break; // exit the for loop
                        }
                        
                    }
                    counter++;
                    if (signal == true){
                        printf("EOF reached...BREAKING..\n");
                        break; // exit the while loop
                    }
                    else if(counter > 95){
                              fprintf(stderr, "Error: Process file %s is malformed.\n", filename);
                              exit(EXIT_FAILURE);
                    }
                }
                if (!signal){
                    printf("Exception: 0xFF not found\n");
                }

                fclose(ptr);
                // load the process
                pcb_list[i_process].process_file_name = filename;
                load_process(&pcb_list[i_process], loader_buffer, ptr, page_size);




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
        printf("  Frame %d\n", free_frames.frames[i]);
    }

    printf("\nTotal Internal Fragmentation: %d bytes\n", total_internal_fragmentation);

    // Clean up memory
    for (int i = 0; i < num_processes; ++i) {
        free(pcb_list[i].page_table);
    }
    free(pcb_list);
    free(free_frames.frames);
    // free(free_frames);


    return 0;
}
