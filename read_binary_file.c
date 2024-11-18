#include <stdio.h>
#include <stdlib.h>


int main(){

    unsigned char buffer[10];
    FILE *ptr;

    ptr = fopen("p1.proc","rb");  // r for read, b for binary
    if (ptr == NULL){
        printf("Error opening file");
        return 1;
    }
    size_t result = 0;
    int b = 0;
    // 0xFF marks the end of the file.
    while ((result = fread(buffer,1, sizeof(buffer),ptr)) > 0){
        
      
        for(size_t i = 0; i < result; i++){

            printf("Byte %zu:  hex=0x%02X\n", b, buffer[i]);
            b++;

            if (buffer[i] == 0xFF){
                printf("End of file reached\n");
                fclose(ptr);
                return 0;
            }
            
        
        }
    }

    fclose(ptr);

    return 0;

}


