#include "coder.h"
#include "command.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void printHelp(){
    printf("\n Usage:\n");
    printf("\tcoder encode <in-file-name> <out-file-name>\n");
    printf("\tcoder decode <in-file-name> <out-file-name>\n\n");
}

int main(int argc, char **argv) {
    if(argc != 4) {
        printHelp();
        return 0;
    }
    if(strcmp(argv[1], "encode") == 0 && strstr(argv[2], ".txt") && strstr(argv[3], ".bin")) {
        if(encode_file(argv[2], argv[3])) {
            printf("Error while encoding\n");
        }
    } else if(strcmp(argv[1], "decode") == 0 && strstr(argv[2], ".bin") && strstr(argv[3], ".txt")) {
        if(decode_file(argv[2], argv[3])) {
            printf("Error while decoding\n");
        }
    }
    return 0;
}