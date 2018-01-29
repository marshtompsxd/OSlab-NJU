#include "x86.h"
#include "device.h"
#include "common.h"

static volatile int key_code = 0;

static int letter_code[] = {
	30, 48, 46, 32, 18, 33, 34, 35, 23, 36,
	37, 38, 50, 49, 24, 25, 16, 19, 31, 20,
	22, 47, 17, 45, 21, 44
};

static int num_code[] = {
	11, 2, 3, 4, 5, 6, 7, 8, 9, 10
};

static int enter_code = 28;

static int space_code = 57;

char press_key(int scan_code) {
	int i;
	for (i = 0; i < 26; i ++) {
		if (letter_code[i] == scan_code) {
			return 'a' + i;
		}
	}

	for(i = 0; i < 10; i ++) {
		if (num_code[i] == scan_code) {
			return '0' + i;
		}
	}

	if(enter_code == scan_code)return '\n';

	if(space_code == scan_code)return ' ';

	return '\0';
}


char keyboard_event() {
	key_code = inByte(0x60);
	return press_key(key_code);
}