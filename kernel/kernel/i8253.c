#include "x86.h"
#include "common.h"

#define TIMER_PORT 0x40
#define FREQ_8253 1193182
#define HZ 100

void 
initTimer(void) {
    int counter = FREQ_8253 / HZ;
    assert(counter < 65536);
    outByte(TIMER_PORT + 3, 0x34);
    outByte(TIMER_PORT + 0, counter % 256);
    outByte(TIMER_PORT + 0, counter / 256);
}