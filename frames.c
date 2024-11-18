// Define Data Structures:
// • Create a PageTable structure with entries that map logical pages to physical frames.
// • Each entry should store the frame number and a valid bit.
// • The Page Table for each process will be maintained in the respective PCB (see Process
// Management section of this file).


//dirty bit 
// When the dirty bit is:

// 0: Page hasn't been modified (clean)
// 1: Page has been modified (dirty) and needs to be written back to disk before replacement


typedef struct PTE {
    int frame_number;
    int valid;
    int dirty;
} PTE;