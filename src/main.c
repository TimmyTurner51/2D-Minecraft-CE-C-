////////////////////////////////////////////////////////////////////////////////
//  2D Minecraft CE (C)
//  Authors: TimmyTurner51, LogicalJoe, and Beckadamtheinventor
//  License: ??
//  Description: A 2D Minecraft clone made for the TI-84 Plus CE, written in C.
////////////////////////////////////////////////////////////////////////////////

#define usb_callback_data_t usb_device_t

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>

#include <srldrvce.h>
#include <usbdrvce.h>

#include <debug.h>

//no need for this unless there's actual gfx data that needs to be written to the program :P
#include "gfx/gfx.h"

#include "compressor.h" //zx7 compression routines

//static char list[20][9]; memcpy(list[found++], name, 9);
//no longer needed...we have length and height vars now... #define world_ground_height 64

gfx_sprite_t *sprites[254];
gfx_sprite_t dummy_sprite = {1, 1, 0};

//define the world data list
char WorldData[200 * 200] = {0};
char *world_file = "MCCESAVE";
//hopefully a static 'y' will fix some menu bugs...
int16_t x, y, pos;
//{ name of world 1, name of world 2, etc. }
char WorldsList[20][9];
//{ name of server 1, name of server 2, etc. }
char Servers[20][20];
char FriendsList[20][20];

//handle these annoying statics later...
int24_t timeofday;
int24_t playerX;
int24_t playerY;
int24_t curX;
int24_t curY;
int24_t curPos;
int24_t hotbar[5] = {0};
int24_t hotbarCur;
int24_t blockSel;
bool loaded_world = 0;
gfx_TempSprite(logo, 16, 16);

uint8_t foundCount; // used to stop the code from finding too many appvars
uint8_t dayColors[5] = {191, 158, 125, 51, 9};

#define SMOOTHSTONE 1
#define GRASS 2
#define DIRT 3
#define COBBLESTONE 4
#define OAKPLANK 5
#define OAKSAPLING 6
#define BEDROCK 7
#define WATER 8
#define NOTHING1 9
#define LAVA 10
#define NOTHING2 11
#define SAND 12
#define GRAVEL 13
#define GOLDORE 14
#define IRONORE 15
#define COALORE 16
#define OAKLOGS 17
#define OAKLEAVES 18
#define SPONGE 19
#define GLASS 20
#define LAPIZORE 21
#define LAPIZBLOCK 22
#define DISPENSER 23
#define SANDSTONE 24
#define NOTEBLOCK 25
#define BEDBACK 26
#define BEDFRONT 27
#define REDSTONEDUSTOFF 28
#define REDSTONEDUSTON 29
#define REGULARPISTONRIGHTOFF 30
#define COBWEB 31

void LoadBlocks(const char *appvar);			 //now handled in an assembly function
uint8_t user_input(char *buffer, size_t length); //user input subroutine. src/asm/user_input.asm

void compressAndWrite(void *data, int len, ti_var_t fp); //this routine compresses using zx7_Compression

//these may need to stay static, used in both main/playMenu (for new world setup) and WorldEngine...
static int24_t worldHeight = 200;
static int24_t worldLength = 200;
static int24_t worldType = 0;
static int24_t hotbarSel = 0;

char worldNameStr[20] = "My World";
char seedStr[20] = "Random Seed";
char oldNameStr[20] = "My World";
char oldSeedStr[20] = "Random Seed";
//static char *str;

void main(void)
{
	ti_var_t appvar;
	gfx_Begin(); //This sets the default palette, no need to set the palette again
	ti_CloseAll();
	gfx_SetClipRegion(-17, -17, 337, 257);
	LoadBlocks("MCEDEFT");
	appvar = ti_Open("MC2DDAT", "r");
	logo = ti_GetDataPtr(appvar);
	ti_Close(appvar);
	y = 125;
	gfx_SetDrawBuffer();
	MainMenu();
}

//Compress data via zx7 compression
void compressAndWrite(void *data, int len, ti_var_t fp)
{
	//	int i, j, k, num, num_i, new;
	//	uint8_t b, m;
	//	int *huffman_table = (void*)0xD52C00;
	//	uint8_t *huffman_table_2 = &huffman_table[256];
	//	uint8_t *huffman_output = &huffman_table_2[256];
	//	void *back_buffer = (void*)0xD52C00;
	//	uint8_t *ptr;
	int new_len;
	*((int *)0xE30010) = 0xD40000; //force the lcd to use the first half of VRAM so we can use the second half.
	gfx_SetDrawScreen();
	drawDirtBackground(0);
	gfx_SetTextFGColor(0xDF);
	gfx_PrintStringXY("Saving World...", 113, 110);

	/* Maybe do huffman coding at some point
	//start by counting the occurences of each unique byte in the data
	ptr = data;
	for (i=0; i<len; i++){
		huffman_table[*ptr++]++;
	}
	//then sort by highest number of occurences
	k = 1;
	for (i=0; i<256; i++){
		num = num_i = 0;
		for (j=0; j<256; j++) {
			if ((new=huffman_table[j]) > num) {
				num = new;
			}
		}
		if (!num) break;
		huffman_table_2[num_i] = k++;
		huffman_table[j] = 0;
	}
	ptr = data;
	m = 1;
	for (i=0; i<len; i++){
		b = huffman_table_2[*ptr++];
		k = 256;
		while (k>>=1){
			if (b & k){
				while (b) {
					
				}
			}
		}
	}
	*/

	zx7_Compress((void *)0xD52C00, data, &new_len, len);
	ti_Write((void *)0xD52C00, new_len, 1, fp);
	main();
}
