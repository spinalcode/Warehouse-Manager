#define levWidth 128
#define levHeight 128
uint8_t curLevel[levWidth*levHeight];
int xScroll = 0;
int yScroll = 0;
#define tileSize 32

uint8_t guiBG[27*22]; // GUI background layer
uint8_t guiLineHasText[22];

long int fpsCount;
long int fpsCounter;
long int lastMillis;

struct PLAYER_DATA {
    int x;  // x postition
    int y;  // y position
    int steps;
    int walking;
    int direction;
} player;

struct BOX_DATA {
    int x;  // x postition
    int y;  // y position
    int steps;
    int walking;
    int direction;
    int frame;
} boxes[40];
int numBoxes = 0;


// for my own sprite renderer
#define NUMSPRITES 48
struct SPRITE_DATA {
    const uint16_t *paletteData; // palette data
    const uint8_t *imageData; // image data
    int x;  // x postition
    int y;  // y position
    int hFlip;
    int offset; // tile render pixel offset
    uint8_t bit;
} sprites[NUMSPRITES];
int spriteCount = -1;
uint8_t spriteLine[176];

