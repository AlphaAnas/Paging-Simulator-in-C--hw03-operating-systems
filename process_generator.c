#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Generates a single random byte and writes it to both the text file and the
 * binary file.
 *
 * @param text_file The text file to write the byte to in hex format.
 * @param binary_file The binary file to write the byte to in its raw form.
 */
void generate_random_byte(FILE *text_file, FILE *binary_file) {
    unsigned char byte = rand() % 256;
    fprintf(text_file, "%02X ", byte);   // Write byte in hex to text file
    fwrite(&byte, sizeof(unsigned char), 1, binary_file);  // Write byte to binary file
}
void generate_process(FILE *text_file, FILE *binary_file, int code_size, int data_size) {
    // Generate and write a random Process ID
    unsigned char process_id = rand() % 256;
    fprintf(text_file, "%02X ", process_id);  // Write Process ID to text file in hex format where %02X is used to print 2 digit hex number
    fwrite(&process_id, sizeof(unsigned char), 1, binary_file);  // Write Process ID to binary file

    // Write Code segment size in two bytes
    fprintf(text_file, "%02X %02X ", (code_size >> 8) & 0xFF, code_size & 0xFF); // Write code size to text file
    unsigned char code_size_bytes[2] = {(code_size >> 8) & 0xFF, code_size & 0xFF};
    fwrite(code_size_bytes, sizeof(unsigned char), 2, binary_file);  // Write code size to binary file

    // Write Code segment data as random bytes
    for (int i = 0; i < code_size; i++) {
        generate_random_byte(text_file, binary_file); 
    }

    // Write Data segment size in two bytes
    fprintf(text_file, "%02X %02X ", (data_size >> 8) & 0xFF, data_size & 0xFF); // Write data size to text file
    unsigned char data_size_bytes[2] = {(data_size >> 8) & 0xFF, data_size & 0xFF};
    fwrite(data_size_bytes, sizeof(unsigned char), 2, binary_file);  // Write data size to binary file

    // Write Data segment data as random bytes
    for (int i = 0; i < data_size; i++) {
        generate_random_byte(text_file, binary_file); 
    }

    // Write End of process marker
    fprintf(text_file, "FF\n");  // Write end marker to text file
    unsigned char end_marker = 0xFF;
    fwrite(&end_marker, sizeof(unsigned char), 1, binary_file);  // Write end marker to binary file
}
int main() {
    srand(time(NULL));

    FILE *text_file = fopen("p1.txt", "w");  // w is write
    FILE *binary_file = fopen("p1.proc", "wb"); // wb is write binary
    
	if (text_file == NULL || binary_file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    int code_size = 30;         // Code segment size (in bytes)
    int data_size = 60;         // Data segment size (in bytes)

    // Generate a single process and create 2 in both files
    generate_process(text_file, binary_file, code_size, data_size);

    fclose(text_file);
    fclose(binary_file);

    return 0;
}
