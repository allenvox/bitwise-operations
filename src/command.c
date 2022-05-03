#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "command.h"
#include "coder.h"

int encode_file(const char *in_file_name, const char *out_file_name)
{
	FILE *in = fopen(in_file_name, "r"); 
	if (in == NULL) {
		return -1;
	}
	FILE *out = fopen(out_file_name, "w+");
	if (out == NULL) {
		return -1;
	}
	char buffer[8];
	while (fgets(buffer, 8, in) != NULL) {
		buffer[strcspn(buffer, "\n")] = '\0';
		uint32_t code_point;
		sscanf(buffer, "%" SCNx32, &code_point); // scan 32-bit hexadecimal в code_point
		CodeUnits code_unit;
		encode(code_point, &code_unit); // кодируем hexadecimal в UTF8, сохраняем в codeunit
		write_code_unit(out, &code_unit); // записываем codeunit в файл out
	}
	return 0;
}

int decode_file(const char *in_file_name, const char *out_file_name)
{
	FILE *in = fopen(in_file_name, "r"); 
	if (in == NULL) {
		return -1;
	}
	FILE *out = fopen(out_file_name, "w+");
	if (out == NULL) {
		return -1;
	}
	CodeUnits code_unit;
	while (!read_next_code_unit(in, &code_unit)) { // пока codeunit'ы не закончились
		uint32_t buffer = decode(&code_unit);
		fprintf(out, "%" PRIx32 "\n", buffer); // print hexadecimal 32 num
	}
	fclose(in);
	fclose(out);
	return 0;
}