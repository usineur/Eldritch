#include <stdio.h>

#include "core.h"
#include "idatastream.h"
#include "memorystream.h"
#include "util.h"

byte* ConvertBGRA2RGBA(int Width, int Height, byte* texture)
{
	byte* ret = (byte*) malloc(Width * Height * 4);
	GLuint tmp;
	byte* dest = ret;
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			tmp = *(const GLuint*)texture;
			*(GLuint*)dest = (tmp & 0xFF00FF00) | ((tmp & 0x00FF0000) >> 16) | ((tmp & 0x000000FF) << 16);
			texture += 4;
			dest += 4;
		}
	}
	return ret;
}

byte* ConvertShader(byte* pBuffer) {
	byte* newptr;
	newptr=(byte*)strstr((char*)pBuffer, "#version");
	if (!newptr)
		newptr = pBuffer;
	else {
		while(*newptr != 0x0a) newptr++;
	}
	const char* GLESHeader = "#version 330\nprecision mediump float;\n";
	auto Tmp = new byte[strlen((char*)newptr) + strlen(GLESHeader) + 100];
	strcat(strcpy((char*)Tmp, GLESHeader), (char*)newptr);
	// now check to remove trailling "f" after float, as it's not supported too
	newptr = Tmp;
	int state = 0;
	while (*newptr != 0x00) {
		switch(state) {
			case 0:
				if ((*newptr >= '0') && (*newptr <= '9'))
					state = 1;	// integer part
				else if (*newptr == '.')
					state = 2;	// fractional part
				else if ((*newptr == ' ') || (*newptr == 0x0d) || (*newptr == 0x0a) || (*newptr == '-') || (*newptr == '+') || (*newptr == '*') || (*newptr == '/') || (*newptr == '(') || (*newptr == ')' || (*newptr == '>') || (*newptr == '<')))
					state = 0; // separator
				else
					state = 3; // something else
				break;
			case 1: // integer part
				if ((*newptr >= '0') && (*newptr <= '9'))
					state = 1;	// integer part
				else if (*newptr == '.')
					state = 2;	// fractional part
				else if ((*newptr == ' ') || (*newptr == 0x0d) || (*newptr == 0x0a) || (*newptr == '-') || (*newptr == '+') || (*newptr == '*') || (*newptr == '/') || (*newptr == '(') || (*newptr == ')' || (*newptr == '>') || (*newptr == '<')))
					state = 0; // separator
				else if ((*newptr == 'f')) {
					// remove that f
					memmove(newptr, newptr + 1, strlen((char*)newptr + 1) + 1);
					newptr--;
				} else
					state = 3;
					break;
			case 2: // fractionnal part
				if ((*newptr >= '0') && (*newptr <= '9'))
					state = 2;
				else if ((*newptr == ' ') || (*newptr == 0x0d) || (*newptr == 0x0a) || (*newptr == '-') || (*newptr == '+') || (*newptr == '*') || (*newptr == '/') || (*newptr == '(') || (*newptr == ')' || (*newptr == '>') || (*newptr == '<')))
					state = 0; // separator
				else if ((*newptr == 'f')) {
					// remove that f
					memmove(newptr, newptr + 1, strlen((char*)newptr + 1) + 1);
					newptr--;
				} else
					state = 3;
					break;
			case 3:
				if ((*newptr == ' ') || (*newptr == 0x0d) || (*newptr == 0x0a) || (*newptr == '-') || (*newptr == '+') || (*newptr == '*') || (*newptr == '/') || (*newptr == '(') || (*newptr == ')' || (*newptr == '>') || (*newptr == '<')))
					state = 0; // separator
				else
					state = 3;
					break;
		}
		newptr++;
	}
	return Tmp;
}
