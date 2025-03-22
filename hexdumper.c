#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// gcc hexdumper.c -Wall -Werror -pedantic -O2 -flto -o cexdumpy
void HexDump(char* filename, unsigned char* data, uint64_t size){
    char spacedHex[192+1] = {0};
    char joinedHex[160+1] = {0};
    unsigned char bufferHex[48+1] = {0};
    unsigned char buffer[16+1] = {0};
    unsigned short offset = 0;
    for(short i = 0; i < 8; ++i){
        offset += sprintf(spacedHex+offset,"\x1B[%dm0%x\x1B[0m ", 30+i, i);
    }
    spacedHex[offset] = ' ';
    offset += 1;
    for(short i = 0; i < 8; ++i){
        offset += sprintf(spacedHex+offset,"\x1B[%dm0%x\x1B[0m ", 90+i, i+8);
    }
    spacedHex[192] = '\0';
    offset = 0;
    for(short i = 0; i < 8; ++i){
        offset += sprintf(joinedHex+offset,"\x1B[%dm%x\x1B[0m", 30+i, i);
    }
    for(short i = 0; i < 8; ++i){
        offset += sprintf(joinedHex+offset,"\x1B[%dm%x\x1B[0m", 90+i, i+8);
    }
    printf("┌──────────┬──────────────────────────────────────────────────┬──────────────────┐\n│  \x1B[94;1moffset\x1B[0m  │ %s │ %s │\n├──────────┼──────────────────────────────────────────────────┼──────────────────┤\n", spacedHex, joinedHex);
    for(uint64_t i = 0; i < size; i += 16){
        memcpy(buffer, data + i, 16);
        offset = 0;
        for(short j = 0; j < 8; ++j){
            offset += sprintf((char*)bufferHex+offset, "%02x ", buffer[j]);
        }
        bufferHex[offset] = ' ';
        offset += 1;
        for(short j = 0; j < 8; ++j){
            offset += sprintf((char*)bufferHex+offset, "%02x ", buffer[j+8]);
        }
        bufferHex[48] = '\0';
        for(short chr = 0; chr < sizeof(buffer); ++chr){
            if(!(32 <= buffer[chr] && buffer[chr] < 127)){
                buffer[chr] = '.';
            }
            if(size - i < 16 && chr >= size - i){
                buffer[chr] = ' ';
            }
        }
        for(short j = 0; j < 48; ++j){
            if(j/3 + i >= size){            // pray that the compiler optimizes the division
                bufferHex[j] = ' ';
            }
        }
        printf("│ \x1B[94;1m%08llx\x1B[0m │ \x1b[1m%.48s\x1B[0m │ \x1b[1m%-16.16s\x1B[0m │\n", i, bufferHex, buffer);
    }
    printf("├──────────┼──────────────────────────────────────────────────┼──────────────────┤\n│ \x1B[94;1m%08llx\x1B[0m │ %s │ %s │\n└──────────┴──────────────────────────────────────────────────┴──────────────────┘\n", size, spacedHex, joinedHex);
}
int main(int argc, char** argv){
    if(argc < 2){
        printf("Usage: %s {path/to/file}\n", argv[0]);
        return -1;
    }
    FILE* filePointer = fopen(argv[1], "rb");
    if(!filePointer){
        printf("[-] Unable to open %s\n", argv[1]);
        return -1;
    }
    if(fseek(filePointer, 0, SEEK_END)){
        fclose(filePointer);
        perror("[-] Unable to modify file pointer");
        return -1;
    }
    uint64_t size = ftell(filePointer);
    if(size == (uint64_t)-1){
        fclose(filePointer);
        perror("[-] Failed to retrieve file size\n");
        return -1;
    }
    rewind(filePointer);
    unsigned char* buffer = (unsigned char*)calloc(size, sizeof(char));
    if(!buffer){
        fclose(filePointer);
        perror("[-] Unable to allocate buffer");
        return -1;
    }
    if(fread(buffer, sizeof(char), size, filePointer) < size){
        free(buffer);
        fclose(filePointer);
        perror("[-] Unable to read file buffer");
        return -1;
    }
    fclose(filePointer);
    HexDump(argv[1], buffer, size);
    free(buffer);
}
/*
@   reduced 1 call to memcpy and 14 printf specifiers 
@   but added 3 loops (2-8iters + 1-48iters) and 3 if
@   statements and a bigger bufferHex by 32 bytes
*/
