#include "x86.h"
#include "device.h"
#include "common.h"


void move_cursor(){
   	uint16_t cursorLocation = cursor_y * 80 + cursor_x;
   	outByte(0x3D4, 14);                  
   	outByte(0x3D5, cursorLocation >> 8); 
   	outByte(0x3D4, 15);                  
   	outByte(0x3D5, cursorLocation);     
}

void screen_put(char ch, uint8_t foreColour, uint8_t backColour){
  
	uint8_t  colorByte = ColorAttr(foreColour, backColour);
   	uint16_t attribute = colorByte << 8;
   	uint16_t *location;
  
   	if (ch == 0x08 && cursor_x){
       	cursor_x--;
   	}
   	else if (ch == 0x09){
       	cursor_x = (cursor_x+8) & ~(8-1);
   	}
   	else if (ch == '\r'){
       	cursor_x = 0;
   	}
   	else if (ch == '\n'){
       	cursor_x = 0;
       	cursor_y++;
   	}
   	else if(ch >= ' '){
       	location = vm_addr + (cursor_y*80 + cursor_x);
       	*location = ch | attribute;
       	cursor_x++;
   	}

  
   	if (cursor_x >= 80){
       	cursor_x = 0;
       	cursor_y ++;
   	}

   	move_cursor();
} 

void screen_clear(){
   	uint8_t  colorByte = ColorAttr(white, black);
   	uint16_t blank = 0x20 | (colorByte << 8);

   	int i;
   	for (i = 0; i < 80*25; i++){
       	vm_addr[i] = blank;
   	}

   	cursor_x = 0;
   	cursor_y = 0;
   	move_cursor();
} 
