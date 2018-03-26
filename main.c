#include <os.h>
#include <SDL/SDL_config.h>
#include <SDL/SDL.h>

SDL_Surface *screen;
nSDL_Font *font;

int width = 320;
int height = 240;
int selectedx = 0;
int selectedy = 0;
int page = 0;
int memoffset = 0;
int ShowOps = 0;
int ShowHex = 1;
int speedratio = 5;
uint lastTime = 0;
int lastINum = 0;
int ips = 0;

Uint32 CWhite;
Uint32 CBlack;
Uint32 CGray;

unsigned char memoryOCs[256];
unsigned char memoryDT[256];

int running = 0;
uint out = 0;

unsigned char pc = 0;
int fle = 0;
int flgt = 0;
int fllt = 0;
int flgte = 0;
int fllte = 0;
unsigned char Areg = 0;
unsigned char Breg = 0;
unsigned char Creg = 0;
unsigned char MIreg = 0;
unsigned char MOreg = 0;
unsigned char retaddress = 0;

uint getTime() {
	uint *time = (uint *)0x90090000;
	return *time;
}

char * OPtoName(uint op) {
	switch(op) {
		case 0b00000000:
			return "NOP"; //NOP
		case 0b00000001: //LDA
			return "LDA";
		case 0b00000010: //LDB
			return "LDB";
		case 0b00000011: //LDC
			return "LDC";
		case 0b00000100: //LDM
			return "LDM";
		case 0b00000101: //LDO
			return "LDO";
		case 0b00000110: //LAA
			return "LAA";
		case 0b00000111: //LAB
			return "LAB";
		case 0b00001000: //LAC
			return "LAC";
		case 0b00001001: //LAM
			return "LAM";
		case 0b00001010: //LAO
			return "LAO";
		case 0b00001011: //ADD
			return "ADD";
		case 0b00001100: //SUB
			return "SUB";
		case 0b00001101: //INC
			return "INC";
		case 0b00001110: //DEC
			return "DEC";
		case 0b00001111: //MOA
			return "MOA";
		case 0b00010000: //MOB
			return "MOB";
		case 0b00010001: //MOC
			return "MOC";
		case 0b00010010: //MOM
			return "MOM";
		case 0b00010011: //MMO
			return "MMO";
		case 0b00010100: //CMP
			return "CMP";
		case 0b00010101: //JMP
			return "JMP";
		case 0b00010110: //JME
			return "JME";
		case 0b00010111: //JNE
			return "JNE";
		case 0b00011000: //JGT
			return "JGT";
		case 0b00011001: //JLT
			return "JLT";
		case 0b00011010: //GTE
			return "GTE";
		case 0b00011011: //LTE
			return "LTE";
		case 0b11111101: //CAL
			return "CAL";
		case 0b11111110: //RET
			return "RET";
		case 0b11111111:  //HLT
			return "HLT";
		default:
			return "???";
		}
}
char getkeypressed() {
	wait_key_pressed();
	if (isKeyPressed(KEY_NSPIRE_ENTER))
		return '\n';
	if (isKeyPressed(KEY_NSPIRE_DEL))
		return '\b';
	if (isKeyPressed(KEY_NSPIRE_A))
		return 'a';
	if (isKeyPressed(KEY_NSPIRE_B))
		return 'b';
	if (isKeyPressed(KEY_NSPIRE_C))
		return 'c';
	if (isKeyPressed(KEY_NSPIRE_D))
		return 'd';
	if (isKeyPressed(KEY_NSPIRE_E))
		return 'e';
	if (isKeyPressed(KEY_NSPIRE_F))
		return 'f';
	if (isKeyPressed(KEY_NSPIRE_G))
		return 'g';
	if (isKeyPressed(KEY_NSPIRE_H))
		return 'h';
	if (isKeyPressed(KEY_NSPIRE_I))
		return 'i';
	if (isKeyPressed(KEY_NSPIRE_J))
		return 'j';
	if (isKeyPressed(KEY_NSPIRE_K))
		return 'k';
	if (isKeyPressed(KEY_NSPIRE_L))
		return 'l';
	if (isKeyPressed(KEY_NSPIRE_M))
		return 'm';
	if (isKeyPressed(KEY_NSPIRE_N))
		return 'n';
	if (isKeyPressed(KEY_NSPIRE_O))
		return 'o';
	if (isKeyPressed(KEY_NSPIRE_P))
		return 'p';
	if (isKeyPressed(KEY_NSPIRE_Q))
		return 'q';
	if (isKeyPressed(KEY_NSPIRE_R))
		return 'r';
	if (isKeyPressed(KEY_NSPIRE_R))
		return 's';
	if (isKeyPressed(KEY_NSPIRE_T))
		return 't';
	if (isKeyPressed(KEY_NSPIRE_U))
		return 'u';
	if (isKeyPressed(KEY_NSPIRE_V))
		return 'v';
	if (isKeyPressed(KEY_NSPIRE_W))
		return 'w';
	if (isKeyPressed(KEY_NSPIRE_X))
		return 'x';
	if (isKeyPressed(KEY_NSPIRE_Y))
		return 'y';
	if (isKeyPressed(KEY_NSPIRE_Z))
		return 'z';
	if (isKeyPressed(KEY_NSPIRE_1))
		return '1';
	if (isKeyPressed(KEY_NSPIRE_2))
		return '2';
	if (isKeyPressed(KEY_NSPIRE_3))
		return '3';
	if (isKeyPressed(KEY_NSPIRE_4))
		return '4';
	if (isKeyPressed(KEY_NSPIRE_5))
		return '5';
	if (isKeyPressed(KEY_NSPIRE_6))
		return '6';
	if (isKeyPressed(KEY_NSPIRE_7))
		return '7';
	if (isKeyPressed(KEY_NSPIRE_8))
		return '8';
	if (isKeyPressed(KEY_NSPIRE_9))
		return '9';
	if (isKeyPressed(KEY_NSPIRE_0))
		return '0';
	if (isKeyPressed(KEY_NSPIRE_SPACE))
		return ' ';
	if (isKeyPressed(KEY_NSPIRE_PERIOD))
		return '.';
	if (isKeyPressed(KEY_NSPIRE_COMMA))
		return ',';
	if (isKeyPressed(KEY_NSPIRE_ESC))
		return -1;
	return 0;
}

Uint32 color(char r, char g, char b) {
	return SDL_MapRGB(screen->format, r, g, b);
}

void fillRect(int x, int y, int l, int w, Uint32 drawColor) {
	SDL_Rect r = {x, y, l, w};
	SDL_FillRect(screen, &r, drawColor);
}

void clearScreen() {
	SDL_FillRect(screen, NULL, color(255, 255, 255));
}

void initScreen() {
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(width, height, has_colors ? 16 : 8, SDL_SWSURFACE);
	font = nSDL_LoadFont(NSDL_FONT_TINYTYPE, 29, 43, 61);

	CWhite = color(255,255,255);
	CBlack = color(0,0,0);
	CGray = color(200,200,200);

	clearScreen();
}
int pcnum = 0;
int mrinum = 1;
int ipsnum = 2;

int anum = 4;
int bnum = 5;
int cnum = 6;
int minum = 7;
int monum = 8;
int onum = 9;

int spdnum = 16;
int stepnum = 17;
int togglenum = 18;

void displayScreen() {
	clearScreen();
	fillRect(20, 20, width/2-40, height-40, CGray);
	if (page == 0) {
		fillRect(20, 20, 5, 5,CBlack);
	}
	fillRect(width/2+20, 20, width/2-40, height-40, CGray);
	if (page == 1) {
		fillRect(width/2+20, 20, 5, 5, CBlack);
	}
	for (int i = 0; i < 19; ++i) {
		if (i+memoffset > 255) 
			break;
		if (i == selectedy && page == 0) {
			if (selectedx == 0) {
				fillRect(25, 25+(i*10), 20, 8, CWhite);
			} else {
				fillRect((25*selectedx)+10+40, 25+(i*10), 20, 8, CWhite);
			}
		}
		if (i+memoffset == pc) {
			nSDL_DrawString(screen, font, 20, 25+(i*10), ">", i+memoffset);
		}
		if (ShowHex == 1) {
			nSDL_DrawString(screen, font, 25, 25+(i*10), "0x%x", i+memoffset);
		} else {
			nSDL_DrawString(screen, font, 25, 25+(i*10), "%i", i+memoffset);
		}
		if (ShowOps == 0) {
			if (ShowHex == 0) {
				nSDL_DrawString(screen, font, 25+35, 25+(i*10), "- %i %i", memoryOCs[i+memoffset],memoryDT[i+memoffset]);
			} else {
				nSDL_DrawString(screen, font, 25+35, 25+(i*10), "- 0x%x 0x%x", memoryOCs[i+memoffset],memoryDT[i+memoffset]);
			}
		} else {
			if (ShowHex == 0) {
				nSDL_DrawString(screen, font, 25+35, 25+(i*10), "- %s %i", OPtoName(memoryOCs[i+memoffset]),memoryDT[i+memoffset]);
			} else {
				nSDL_DrawString(screen, font, 25+35, 25+(i*10), "- %s 0x%x", OPtoName(memoryOCs[i+memoffset]),memoryDT[i+memoffset]);
			}
			
		}
	}

	for (int i = 0; i < 19; ++i) {
		if (i == selectedy && page == 1) {
			{
				if (selectedx == 0) {
					fillRect(width/2+25, 25+(i*10), 20, 8, CWhite);
				} else {
					fillRect(((width/2+25)*selectedx)+10+40, 25+(i*10), 20, 8, CWhite);
				}
			}
		}
		if (i == pcnum) {
			if (ShowHex == 0) {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "PC:%i", pc);
			} else {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "PC:0x%x", pc);
			}
		} else if (i == mrinum) {
			nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "MRI:%s", OPtoName(memoryOCs[pc]));
		} else if (i == ipsnum) {
			nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "IPS:%i", ips);
		} else if (i == stepnum) {
			nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "Step");
		} else if (i == togglenum) {
			if (running == 1) {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "Stop");
			} else {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "Go");
			}
		} else if (i == spdnum) {
			nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "SPD: 1/%i",speedratio);
		} else if (i == anum) { //a
			if (ShowHex == 0) {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "A: %i", Areg);
			} else {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "A: 0x%x", Areg);
			}
		} else if (i == bnum) { //b
			if (ShowHex == 0) {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "B: %i", Breg);
			} else {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "B: 0x%x", Breg);
			}
		} else if (i == cnum) { //c
			if (ShowHex == 0) {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "C: %i", Creg);
			} else {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "C: 0x%x", Creg);
			}
		} else if (i == minum) { //mi
			if (ShowHex == 0) {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "MI: %i", MIreg);
			} else {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "MI: 0x%x", MIreg);
			}
		} else if (i == monum) { //mo
			if (ShowHex == 0) {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "MO: %i", MOreg);
			} else {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "MO: 0x%x", MOreg);
			}
		} else if (i == onum) { //out
			if (ShowHex == 0) {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "OUT: %i", out);
			} else {
				nSDL_DrawString(screen, font, width/2+25, 25+(i*10), "OUT: 0x%x", out);
			}
		}
	}
	SDL_Flip(screen);
}

void step() {
	switch(memoryOCs[pc]) {
		case 0b00000000: //NOP
			pc++;
			break;
		case 0b00000001: //LDA
			Areg = memoryDT[memoryDT[pc]];
			pc++;
			break;
		case 0b00000010: //LDB
			Breg = memoryDT[memoryDT[pc]];
			pc++;
			break;
		case 0b00000011: //LDC
			Creg = memoryDT[memoryDT[pc]];
			pc++;
			break;
		case 0b00000100: //LDM
			MIreg = memoryDT[memoryDT[pc]];
			pc++;
			break;
		case 0b00000101: //LDO
			out = memoryDT[memoryDT[pc]];
			pc++;
			break;
		case 0b00000110: //LAA
			Areg = memoryDT[pc];
			pc++;
			break;
		case 0b00000111: //LAB
			Breg = memoryDT[pc];
			pc++;
			break;
		case 0b00001000: //LAC
			Creg = memoryDT[pc];
			pc++;
			break;
		case 0b00001001: //LAM
			MIreg = memoryDT[pc];
			pc++;
			break;
		case 0b00001010: //LAO
			MOreg = memoryDT[pc];
			pc++;
			break;
		case 0b00001011: //ADD
			MOreg = Areg + MIreg;
			pc++;
			break;
		case 0b00001100: //SUB
			MOreg = Areg - MIreg;
			pc++;
			break;
		case 0b00001101: //INC
			Areg = Areg + memoryDT[pc];
			pc++;
			break;
		case 0b00001110: //DEC
			Areg = Areg - memoryDT[pc];
			pc++;
			break;
		case 0b00001111: //MOA
			memoryDT[memoryDT[pc]] = Areg;
			pc++;
			break;
		case 0b00010000: //MOB
			memoryDT[memoryDT[pc]] = Breg;
			pc++;
			break;
		case 0b00010001: //MOC
			memoryDT[memoryDT[pc]] = Creg;
			pc++;
			break;
		case 0b00010010: //MOM
			memoryDT[memoryDT[pc]] = MIreg;
			pc++;
			break;
		case 0b00010011: //MMO
			memoryDT[memoryDT[pc]] = MOreg;
			pc++;
			break;
		case 0b00010100: //CMP
			fle = MIreg == Areg;
			flgt = MIreg > Areg;
			fllt = MIreg < Areg;
			flgte = MIreg >= Areg;
			fllte = MIreg <= Areg;
			pc++;
			break;
		case 0b00010101: //JMP
			pc = Areg;
			break;
		case 0b00010110: //JME
			if (fle)
				pc = Areg;
			else
				pc++;
			break;
		case 0b00010111: //JNE
			if (!fle)
				pc = Areg;
			else
				pc++;
			break;
		case 0b00011000: //JGT
			if (flgt)
				pc = Areg;
			else
				pc++;
			break;
		case 0b00011001: //JLT
			if (fllt)
				pc = Areg;
			else
				pc++;
			break;
		case 0b00011010: //GTE
			if (flgte)
				pc = Areg;
			else
				pc++;
			break;
		case 0b00011011: //LTE
			if (fllte)
				pc = Areg;
			else
				pc++;
			break;
		case 0b11111101: //CAL
			retaddress = pc;
			pc = memoryDT[pc];
			break;
		case 0b11111110: //RET
			pc = retaddress;
			break;
		case 0b11111111:  //HLT
			running = 0;
			pc = 0;
			break;
		default: //??
			running = 0;
			pc = 0;
			break;
		}
		displayScreen();
		lastINum++;
}

int not(int in) {
	if (in == 0) {
		return 1;
	} else {
		return 0;
	}
}

char tex[50];
char htext[5];
int getint(int length, int hex, int cv) {
	for (uint i = 0; i < sizeof(tex); ++i) {
		tex[i] = 0;
	}
	for (int i = 0; i < length; ++i) {
		tex[i] = '_';
	}

	strcpy(htext , "");
	if (hex == 1) {
		strcpy(htext , " hex");
	}
	clearScreen();
	nSDL_DrawString(screen, font, 5, 5, "Enter up to %i%s digits: %s", length, htext, tex);
	SDL_Flip(screen);
	for (int i = 0; i < length; ++i) {
		int in = getkeypressed();
		if ((in >= '0' && in <= '9') || (hex == 1 && in >= 'a' && in <= 'f')) {
			tex[i] = in;
			clearScreen();
			nSDL_DrawString(screen, font, 5, 5, "Enter up to %i%s digits: %s", length, htext, tex);
			SDL_Flip(screen);
		} else if (in == '\b' && i > 0) {
			tex[i-1] = '_';
			i -= 2;
			clearScreen();
			nSDL_DrawString(screen, font, 5, 5, "Enter up to %i%s digits: %s", length, htext, tex);
			SDL_Flip(screen);
		} else if (isKeyPressed(KEY_NSPIRE_ESC)) {
			while (isKeyPressed(KEY_NSPIRE_ESC));
			return cv;
		} else {
			i = length;
		}
	}
	if (hex == 1) {
		return strtoll(tex, NULL, 16);
	}
	return atoi(tex);
}
void DispMSG(char* msg) {
	while (isKeyPressed(KEY_NSPIRE_ENTER));
	clearScreen();
	nSDL_DrawString(screen, font, 5, 5, msg);
	SDL_Flip(screen);
	wait_key_pressed();
	while (isKeyPressed(KEY_NSPIRE_ENTER));
}
char fname[50+11+8];
void LSD() {
	for (uint i = 0; i < sizeof(fname); ++i) {
		fname[i] = 0;
	}
	int lors = 0;
	int going = 1;
	while (going == 1) {
		clearScreen();
		fillRect(5, 5, 4*7, 8, CWhite);
		if (lors == 0) {
			fillRect(5, 5, 4*7, 8, CGray);
		} else {
			fillRect(7*7+5, 5, 4*7, 8, CGray);
		}
		nSDL_DrawString(screen, font, 5, 5, "Load or Save");
		SDL_Flip(screen);
		if (isKeyPressed(KEY_NSPIRE_LEFT)) {
			lors = 0;
		} else if (isKeyPressed(KEY_NSPIRE_RIGHT)) {
			lors = 1;
		} else if (isKeyPressed(KEY_NSPIRE_ENTER)) {
			going = 0;
			while (isKeyPressed(KEY_NSPIRE_ENTER));
		} else if (isKeyPressed(KEY_NSPIRE_ESC)) {
			while (isKeyPressed(KEY_NSPIRE_ESC));
			return;
		}
	}
	going = 1;
	while (!isKeyPressed(KEY_NSPIRE_ENTER)) {
		while (!isKeyPressed(KEY_NSPIRE_ENTER) && sizeof(fname) > 1) {
			clearScreen();
			fillRect(5, 5, 4*7, 8, CWhite);
			nSDL_DrawString(screen, font, 5, 5, "File Name: (%i remaining)\n%s.bin", sizeof(fname)-strlen(fname)-(11+8), fname);
			SDL_Flip(screen);
			if (isKeyPressed(KEY_NSPIRE_ESC)) {
				while (isKeyPressed(KEY_NSPIRE_ESC));
				return;
			}
			char in = getkeypressed();
			if (in == '\b') {
				fname[strlen(fname)-1] = 0;
			} else if(in != '\n' && (int)(sizeof(fname)-(11+8)-strlen(fname)) > 0) {
				fname[strlen(fname)] = in;
			}
		}
		while (isKeyPressed(KEY_NSPIRE_ENTER));

		clearScreen();
		fillRect(5, 5, 4*7, 8, CWhite);
		nSDL_DrawString(screen, font, 5, 5, "\"%s.bin\" OK? (Press [enter])",fname);
		SDL_Flip(screen);
		wait_key_pressed();
		if (isKeyPressed(KEY_NSPIRE_ESC)) {
			while (isKeyPressed(KEY_NSPIRE_ESC));
			return;
		}
	}
	while (isKeyPressed(KEY_NSPIRE_ENTER));

	strncat (fname, ".bin.tns", 8);
	char buf[50+11+8] = "/documents/";
	strncat (buf, fname, 11);
	strcpy(fname, buf);

	if (lors == 0) {
		//read
		FILE *ptr;
		ptr = fopen(fname,"rb");
		if (ptr == NULL) {
			DispMSG("Unable to read file");
			return;
		}
		fread(memoryOCs,1,256,ptr);
		fread(memoryDT,1,256,ptr); 
		DispMSG("Opened");
		fclose(ptr);
	} else {
		//write
		FILE *ptr;
		ptr = fopen(fname,"wb");
		if (ptr == NULL) {
			DispMSG("Unable to write to file");
			return;
		}
		fwrite(memoryOCs,1,256,ptr);
		fwrite(memoryDT,1,256,ptr);
		fclose(ptr);
		DispMSG("Saved");
	}
	return;
}

int main(int argc, char *argv[]) {
	if (argc||argv) {}
	for (uint i = 0; i < sizeof(memoryOCs); ++i)
	{
		memoryOCs[i] = 0;
	}
	for (uint i = 0; i < sizeof(memoryDT); ++i)
	{
		memoryDT[i] = 0;
	}
	lastTime = getTime();
	initScreen();
	displayScreen();
	uint p = 1; 
	while (!isKeyPressed(KEY_NSPIRE_ESC)) {
		p++;
		int redraw = 0;
		int del = 1;
		if (isKeyPressed(KEY_NSPIRE_UP)) {
			redraw = 1;
			del = 0;
			if (selectedy > 0)
				selectedy--;
			else if (memoffset != 0 && page == 0) 
				memoffset--;
			else if (page == 1)
				selectedy = 18;
			while (isKeyPressed(KEY_NSPIRE_UP));
		} else if (isKeyPressed(KEY_NSPIRE_DOWN)) {
			redraw = 1;
			del = 0;
			if (selectedy < 18)
				selectedy++;
			else if (memoffset != 255 && page == 0)
				memoffset++;
			else if (page == 1)
				selectedy = 0;
			while (isKeyPressed(KEY_NSPIRE_DOWN));
		} else if (isKeyPressed(KEY_NSPIRE_LEFT)) {
			redraw = 1;
			del = 0;
			if (selectedx > 0 && page == 0)
				selectedx--;
			while (isKeyPressed(KEY_NSPIRE_LEFT));
		} else if (isKeyPressed(KEY_NSPIRE_RIGHT)) {
			redraw = 1;
			del = 0;
			if (selectedx < 2 && page == 0)
				selectedx++;
			while (isKeyPressed(KEY_NSPIRE_RIGHT));
		} else if (isKeyPressed(KEY_NSPIRE_ENTER)) {
			redraw = 0;
			del = 0;
			if (page == 1) {
				if (selectedy == stepnum) {
					step();
				} else if (selectedy == togglenum) {
					running = not(running);
				} else if (selectedy == spdnum) {
					int p = 0;
					if (isKeyPressed(KEY_NSPIRE_SHIFT)) {
						p = not(p);
					}
					speedratio = getint(3, p, speedratio);
					if (speedratio == 0)
						speedratio = 1;
					redraw = 1;
				} else if (selectedy == pcnum) {
					int p = 1;
					if (isKeyPressed(KEY_NSPIRE_SHIFT)) {
						p = not(p);
					}
					pc = getint(2, p, pc);
					redraw = 1;
				} else if (selectedy == anum) {
					int p = 1;
					if (isKeyPressed(KEY_NSPIRE_SHIFT)) {
						p = not(p);
					}
					Areg = getint(2, p, Areg);
					redraw = 1;
				}  else if (selectedy == bnum) {
					int p = 1;
					if (isKeyPressed(KEY_NSPIRE_SHIFT)) {
						p = not(p);
					}
					Breg = getint(2, p, Breg);
					redraw = 1;
				} else if (selectedy == cnum) {
					int p = 1;
					if (isKeyPressed(KEY_NSPIRE_SHIFT)) {
						p = not(p);
					}
					Creg = getint(2, p, Creg);
					redraw = 1;
				}
			} else {
				if (selectedx == 0) {
					int p = 1;
					if (isKeyPressed(KEY_NSPIRE_SHIFT)) {
						p = not(p);
					}
					memoffset = getint(2, p, memoffset);
					selectedy = 0;
					redraw = 1;
				} else if (selectedx == 1) {
					int p = 1;
					if (isKeyPressed(KEY_NSPIRE_SHIFT)) {
						p = not(p);
					}
					memoryOCs[memoffset+selectedy] = getint(2, p, memoryOCs[memoffset+selectedy]);
					redraw = 1;
				} else {
					int p = 1;
					if (isKeyPressed(KEY_NSPIRE_SHIFT)) {
						p = not(p);
					}
					memoryDT[memoffset+selectedy] = getint(2, p, memoryDT[memoffset+selectedy]);
					redraw = 1;
				}
			}
			while (isKeyPressed(KEY_NSPIRE_ENTER));
		} else if (isKeyPressed(KEY_NSPIRE_DEL)) {
			redraw = 1;
			del = false;
			if (page == 0) {
				if (selectedx == 1) {
					memoryOCs[memoffset+selectedy] = 0;
				} else if (selectedx == 2) {
					
					memoryDT[memoffset+selectedy] = 0;
				}
			}
			while (isKeyPressed(KEY_NSPIRE_DEL));
		} else if (isKeyPressed(KEY_NSPIRE_TAB)) {
			selectedx = 0;
			selectedy = 0;
			redraw = 1;
			del = 0;
			page = not(page);
			while (isKeyPressed(KEY_NSPIRE_TAB));
		} else if (isKeyPressed(KEY_NSPIRE_PLUS)) {
			redraw = 1;
			if (memoffset != 255) 
				memoffset++;
		} else if (isKeyPressed(KEY_NSPIRE_MINUS)) {
			redraw = 1;
			if (memoffset != 0) 
				memoffset--;
		} else if (isKeyPressed(KEY_NSPIRE_FLAG)) {
			redraw = 1;
			if (ShowOps == 0) {
				ShowOps = 1;
			} else {
				ShowOps = 0;
			}
			del = 0;
			while (isKeyPressed(KEY_NSPIRE_FLAG));
		} else if (isKeyPressed(KEY_NSPIRE_EE)) {
			redraw = 1;
			if (ShowHex == 0) {
				ShowHex = 1;
			} else {
				ShowHex = 0;
			}
			del = 0;
			while (isKeyPressed(KEY_NSPIRE_EE));
		} else if (isKeyPressed(KEY_NSPIRE_DOC)) {
			LSD();
			redraw = 1;
			del = 0;
			while (isKeyPressed(KEY_NSPIRE_DOC));
		}
		if (redraw == 1) {
			displayScreen();
			if (del == 1) {
				sleep(100);
			}
		}
		if (running == 1 && (p % speedratio == 1 || speedratio == 1)) {
			step();
			if (lastTime != getTime()) {
				ips = lastINum;
				lastINum = 0;
				lastTime = getTime();
			}
		}
	}
	SDL_Quit();
	return 0;
}