#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

size_t encode_varint(uint32_t value, uint8_t* buf)
{
    assert(buf != NULL);
    uint8_t* cur = buf;
    while (value >= 0x80) {
        const uint8_t byte = (value & 0x7f) | 0x80;
        *cur = byte;
        value >>= 7;
        ++cur;
    }
    *cur = value;
    ++cur;
    return cur - buf;
}

uint32_t decode_varint(const uint8_t** bufp)
{
    const uint8_t* cur = *bufp;
    uint8_t byte = *cur++;
    uint32_t value = byte & 0x7f;
    size_t shift = 7;
    while (byte >= 0x80) {
        byte = *cur++;
        value += (byte & 0x7f) << shift;
        shift += 7;
    }
    *bufp = cur;
    return value;
}
/*
char* getBinary(int n, int len)
{
    char* binary = (char*)malloc(sizeof(char) * len);
    int k = 0;
    for (unsigned i = (1 << len - 1); i > 0; i = i / 2) {
        binary[k++] = (n & i) ? '1' : '0';
    }
    binary[k] = '\0';
    return binary;
}*/

char* printByte(const uint8_t byte)
{
	char* out = malloc(sizeof(char) * 9);

	uint8_t mask = 1;
	uint8_t tmp = 0;

	int i = 0;
	for (; i < 8; ++i) {
		tmp = (byte >> i) & mask;
		out[7 - i] = tmp + '0';
	}

	out[8] = '\0';
	return out;
}

int fromBinary(const char *s) {
    return (int) strtol(s, NULL, 2);
}

int main()
{
    FILE *f = fopen("bin/uncompressed.dat", "r");
    uint32_t numberArray[100000];
    for (int i = 0; i < 100000; i++) {
        fscanf(f, "%d", &numberArray[i]);
    }
    fclose(f);

    FILE *writing = fopen("bin/compressed.dat", "w");
    for(int i = 0; i < 100000; i++) {
        uint32_t n = numberArray[i];
        uint8_t buf[4] = {0,0,0,0};
        size_t bytes = encode_varint(n, buf);
        uint8_t *encodedBuf = malloc(bytes * sizeof(uint8_t));
        for(int i = 0; i < bytes; i++) {
            encodedBuf[i] = buf[i];
        }
        for(int i = 0; i < bytes; i++) {
            fprintf(writing, "%s", printByte(encodedBuf[i]));
            if(i == bytes - 1) {
                fprintf(writing, "\n");
            } else {
                fprintf(writing, " ");
            }
        }
    }
    fclose(writing);

    FILE *compressed = fopen("bin/compressed.dat", "r");
    printf("original\tdecoded\tbytes\n");
    for(int i = 0; i < 100000; i++) {
        char str[37];
        fgets(str, 37, compressed);
        str[strcspn(str, "\n")] = 0;
        char* byteStr = strtok(str, " ");
        uint8_t buf[4] = {0,0,0,0};
        int bytesCount = 0;
        for(int i = 0; i < 4; i++) {
            buf[i] = (uint8_t) fromBinary(byteStr);
            byteStr = strtok(NULL, " ");
            if(byteStr == NULL) {
                bytesCount = i+1;
                break;
            }
        }
        uint8_t *finalBuf = malloc(bytesCount * sizeof(uint8_t));
        for(int i = 0; i < bytesCount; i++) {
            finalBuf[i] = buf[i];
        }
        uint32_t result = decode_varint(&finalBuf);
        printf("%d\t%u\t%d\n", numberArray[i], result, bytesCount);
        if(numberArray[i] != result) {
            printf("^ not equal values\n");
            break;
        }
    }
    printf("finished\n");
    fclose(compressed);
    return 0;
}