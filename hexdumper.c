#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// gcc hexdumper.c -Wall -Werror -pedantic -O2 -flto
void HexDump(char* filename, unsigned char* data, uint64_t size){
    char spacedHex[192+1] = {0};
    char joinedHex[160+1] = {0};
    short offset = 0;
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
    printf("┌──────────┬──────────────────────────────────────────────────┬──────────────────┐\n│  \x1B[94moffset\x1B[0m  │ %s │ %s │\n├──────────┼──────────────────────────────────────────────────┼──────────────────┤\n", spacedHex, joinedHex);
    unsigned char buffer[16+1] = {0};
    unsigned char bufferHex[16+1] = {0};
    for(uint64_t i = 0; i < size; i += 16){
        memcpy(bufferHex, data + i, 16);
        memcpy(buffer, bufferHex, 16);
        for(short chr = 0; chr < sizeof(buffer); ++chr){
            buffer[chr] = (32 <= buffer[chr] && buffer[chr] < 127) ? buffer[chr] : (short)'.';
        }
        printf("│ \x1B[94m%08llx\x1B[0m │ %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x │ %-16.16s │\n", i, bufferHex[0],bufferHex[1], bufferHex[2], bufferHex[3], bufferHex[4], bufferHex[5], bufferHex[6], bufferHex[7], bufferHex[8], bufferHex[9], bufferHex[10], bufferHex[11], bufferHex[12], bufferHex[13],bufferHex[14], bufferHex[15], buffer);
    }
    printf("├──────────┼──────────────────────────────────────────────────┼──────────────────┤\n│ \x1B[94m%08llx\x1B[0m │ %s │ %s │\n└──────────┴──────────────────────────────────────────────────┴──────────────────┘\n", size, spacedHex, joinedHex);
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
    if(fseek(filePointer, 0, SEEK_SET)){
        fclose(filePointer);
        perror("[-] Unable to remodify file pointer");
        return -1;
    }
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
