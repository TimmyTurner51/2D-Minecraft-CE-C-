
#ifndef __defines_h__
#define __defines_h__

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
int natureBlocks[13] = {GRASS, DIRT, SMOOTHSTONE, COBBLESTONE, SAND, GRAVEL, OAKLOGS, OAKLEAVES, BEDROCK, COALORE, IRONORE, GOLDORE, LAPIZORE};
int buildingBlocks[3] = {OAKPLANK, GLASS, SPONGE};
int redstoning[3] = {REDSTONEDUSTOFF, NOTEBLOCK, REGULARPISTONRIGHTOFF};
int toolsEtc[1] = { BEDBACK };

int typesvalues[4] = {13, 3, 3, 1};



uint8_t foundCount; // used to stop the code from finding too many appvars
uint8_t dayColors[5] = {191, 158, 125, 51, 9};

uint8_t user_input(char *buffer, size_t length); //user input subroutine. src/asm/user_input.asm

void compressAndWrite(void *data, int len, ti_var_t fp); //this routine compresses using zx7_Compression

//these may need to stay static, used in both main/playMenu (for new world setup) and WorldEngine...
int24_t worldHeight = 200;
int24_t worldLength = 200;
int24_t worldType = 0;
int24_t hotbarSel = 0;

char worldNameStr[20] = "My World";
char seedStr[20] = "Random Seed";
char oldNameStr[20] = "My World";
char oldSeedStr[20] = "Random Seed";
//char *str;

#endif
