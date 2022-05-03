#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "coder.h"

int encode(uint32_t code_point, CodeUnits *code_units) { // encode hex to UTF8
	// 3F = 00111111 ; FC0 = 111111000000; 3F000 = 00111111000000000000
	uint8_t count = 0;
	for (uint32_t i = code_point; i > 0; i >>= 1) { // считаем количество значащих битов
		count++;
	}
	//printf("value %x, bits %d\n", code_point, count);
	if (count <= 7) { // 7 значащих бит, шаблон 0xxxxxxx
		code_units->code[0] = code_point; // записываем как есть (00000000 + 7 значащих)
		code_units->length = 1;
		return 0;
	} else if (count <= 11) { // 11 знач. бит, шаблон 110xxxxx 10xxxxxx
		code_units->code[0] = 0xC0 | (code_point >> 6); // 11000000 + сдвинутые
		code_units->code[1] = 0x80 | (code_point & 0x3F); // 10000000 + оставшиеся
		code_units->length = 2;
		return 0;
	} else if (count <= 16) { // 16 знач. бит, шаблон 1110xxxx 10xxxxxx 10xxxxxx
		code_units->code[0] =  0xE0 | (code_point >> 12); // 11100000 + сдвинутые
		code_units->code[1] =  0x80 | ((code_point & 0xFC0) >> 6); // 10000000 + сдвинутые
		code_units->code[2] =  0x80 | (code_point & 0x3F); // 10000000 + оставшиеся
		code_units->length = 3;
		return 0;
	} else if (count <= 21) { // 21 знач. бит, шаблон 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		code_units->code[0] = 0xF0 | (code_point >> 18); // 11110000 + сдвинутые
		code_units->code[1] = 0x80 | ((code_point & 0x3F000) >> 12); // 10000000 + сдвинутые
		code_units->code[2] = 0x80 | ((code_point & 0xFC0) >> 6); // 10000000 + сдвинутые
		code_units->code[3] = 0x80 | (code_point & 0x3F); // 10000000 + оставшиеся
		code_units->length = 4;
		return 0;
	}
	return -1;
}

uint32_t decode(CodeUnits *code_unit) {
	uint32_t code_point;
	if ((code_unit->code[0] >> 7) == 0) { // если только 7 значащих бит, 1 закод. байт
		return (code_point = code_unit->code[0]);
	} else if (code_unit->code[0] <= 0xDF) { // если первый байт в пределах 11011111, 2 закод. байта
		return (code_point = (((code_unit->code[0] & 0x1F) << 6) | (code_unit->code[1] & 0x3F)));
	} else if (code_unit->code[0] <= 0xEF) { // если первый байт в пределах 11101111 - 3 закод. байта
		return (((code_unit->code[0] & 0xF) << 12) | ((code_unit->code[1] & 0x3F) << 6) | (code_unit->code[2] & 0x3F));
	} else if (code_unit->code[0] <= 0xF7) { // если первый байт в пределах 11110111 - 4 закод. байта
		return (((code_unit->code[0] & 0x7) << 18) | ((code_unit->code[1] & 0x3F) << 12) | ((code_unit->code[2] & 0x3F) << 6) | (code_unit->code[3] & 0x3F));
	}
	return 0;
}

int read_next_code_unit(FILE *in, CodeUnits *code_unit) {
    uint8_t buffer[4];
    code_unit->length = 0;
    size_t t = fread(&buffer[0], 1, 1, in); // считываем 1 байт в количестве 1 в buffer из потока файла in
    if (t != 1) {   // если был считан не один байт
        return -1;
    }
    if (buffer[0] >> 7 == 0) { 					// если старший бит = 0
        code_unit->length = 1;					// длина UTF8 - 1 байт
        code_unit->code[0] = buffer[0]; 		// первый (единственный) code unit = байт из буфера
    } else {				   					// иначе, если старший бит = 1
		for (int i = 5; i >= 3; i--) {
            if (buffer[0] >> (8 - i) == (pow(2, i) - 2)) { // если байт не битый (смещение на 3 бита = 30, на 4 бита = 14, на 5 битов = 6)
                int count = 0;
                uint8_t y = buffer[0] >> (8 - i + 1);
                while (y) { // считаем количество закодированных байт
                    y >>= 1;
                    count++;
                }
                for (int j = 1; j < count; j++) { // считываем следующие байты
                    size_t t = fread(&buffer[j], 1, 1, in);
                    if (!t) { // если считался не 1 байт за раз
                        return -1;
                    }
                    if (buffer[j] >> 6 != 2) { // если байт битый, читаем следующий, пока не найдём корректный лидирующий
                        return read_next_code_unit(in, code_unit);
                    }
                }
                for (int j = 0; j < count; j++) {
                    code_unit->code[j] = buffer[j]; // записываем в code unit'ы валидные UTF8 байты
                }
                code_unit->length = count;
                break;
            }
        }
    }
    if (code_unit->length == 0) { // если не зашёл в цикл - байт битый (10xxxxxx)
        return read_next_code_unit(in, code_unit);
    }
    return 0;
}

int write_code_unit(FILE *out, CodeUnits *code_unit) {
	if (fwrite(code_unit->code, 1, code_unit->length, out) != code_unit->length) { // записываем 1 код UTF8 размера length (от 1 до 4) в поток файла out
		return -1; // если количество записанных элементов не то, которое должно быть
	}
	return 0;
}