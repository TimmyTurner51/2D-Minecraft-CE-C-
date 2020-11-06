
////////////////////////////////////////////////////////////////////////////////
//  2D Minecraft CE (C)
//  Authors: TimmyTurner51, LogicalJoe, and Beckadamtheinventor
//  License: ??
//  Description: A 2D Minecraft clone made for the TI-84 Plus CE, written in C.
////////////////////////////////////////////////////////////////////////////////

	static uint24_t numSprites;
	static uint24_t sprites;
  
void main(void);
void LoadBlocks(void);

//first, run LoadBlocks(), then, draw the main menu...

void main(void) {
	gfx_Begin();
	gfx_SetPalette(myPalette, sizeof_myPalette, 0);
  LoadBlocks();
  DrawMenu();
  
}

void DrawMenu(void) {
	int x, y, CursorY;
	gfx_SetDrawBuffer();
  for (x = 0; x < 20; x++) {
  	for (y = 0; y < 15; y++) {
    	gfx_SpriteNoClip(dirt, x * 16, y * 16);
    }
  }
  
	while !(kb_IsDown(kb_Key2nd)) {  
  	kb_Scan();
  	
  	
    gfx_BlitBuffer();
  }
  
}

					will this site thing stay up for more than a day, or a session, or whatever?

void LoadBlocks(void) {
	int x, spriteSize;
  ti_var_t appvar;
  
	numSprites = 64;
	spriteSize = (16 * 16) + 2;
	appvar = ti_Open("pack", "r");
	Pointer = ti_GetDataPtr(appvar);
	ti_CloseAll();
	data = numSprites * 3;
	sprites = ti_mAlloc(N);
	for (x = 0) {
		sprites[x] = Pointer;
		Pointer += spriteSize;
		x += 3;
		}

}

