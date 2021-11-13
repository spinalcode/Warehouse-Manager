// =APP.customSetVariables({maxLogLength:100000})

#include <Pokitto.h>
#include <File>
#include <LibAudio>
#include "globals.h"
#include "graphics.h"
#include "fonts.h"
#include "buttonhandling.h"
#include "screen.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;
using PB = Pokitto::Buttons;


void drawSprite(int x, int y, const uint8_t *imageData,const uint16_t *paletteData, bool hFlip, uint8_t bit){

//    printf("%d\n",y); // -16

    // if out of screen bounds, don't bother
    if(x<-32 || x>220) return;
    if(y<-32 || y>175) return;

//    printf("%d\n",y); // -16


    if(++spriteCount>NUMSPRITES-1)spriteCount=NUMSPRITES-1; // don't overflow the sprite max

    sprites[spriteCount].x = x;
    sprites[spriteCount].y = y;
    sprites[spriteCount].imageData = imageData;
    sprites[spriteCount].paletteData = paletteData;
    sprites[spriteCount].hFlip = hFlip;
    sprites[spriteCount].bit = bit;

//    printf("%d\n",sprites[spriteCount].y); // -16

    // make sure we know there are sprites on these lines
    int y1 = y;
    int y2 = y+sprites[spriteCount].imageData[1];
    if(y2>176)y2=176;
    if(y1>=0 && y1 <=175){
        for(int t=y1; t<y2; t++){
            spriteLine[t]=1;
        }
    }
}

// print text
void guiPrint(char x, char y, const char* text) {
  //printf(text);
  //printf("\n");
  guiLineHasText[y] = 1;
  uint8_t numChars = strlen(text);
  int charPos = x + 27 * y;
  for (int t = 0; t < numChars;) {
    guiBG[charPos++] = text[t++];
  }
}




void loadSokLev(int lev){
    
    int ldOffset = 10; // how far in to the level data to place the new level to avoid over-scrolling the screen
    
    //notYet=10; // delay level completion by 10 frames
    int x,y;
    for(y=0; y<levHeight; y++){
		for(x=0; x<levWidth; x++){
            curLevel[x+levWidth*y] = 0;
        }
    }
    printf("Cleaned level data\n");
    //numBoxes = 0;
    //numButtons = 0;
    //playerD = 0; // direction
    //playerF = 0;
    //lastDirection=0;
    //walking = 0;
    //steps = 0;
    //playerF = 0;
    //currentMove = 0;
    
    //NSString *path = [[NSBundle mainBundle] bundlePath];
    char levelPath[64]={0};
    //strcpy(levelPath, [path UTF8String]);
    strcat(levelPath, "boxpusher/1.txt");
    //printf("%s\n",levelPath);
    
    char line[80]={0};
    File fr;
    if(fr.openRO(levelPath)){
        //printf("File did open\n");
        int levCount = 0;
        // find current level...
        while(levCount < lev){
            // read line
            int x = 0;
            bool completeLine = false;
            bool failed = false;
            while(completeLine == false){
                line[x+1]=0;
                if(fr.read(&line[x],1)){
                    if(line[x] == 0x0A){
                        completeLine = true;
                    }
                }else {
                    failed = true;
                }
                x++;
            }
            
            if(failed == false){
                if(line[0] == ';')levCount++;
                //printf("%s",line);
                // Reset level
                int nt = levHeight * levWidth;
                for(y=0; y<nt; y++){
                    curLevel[nt] = 0;
                }
                // remove boxes
                numBoxes=0;
            }
        }
        
        int spaceCount = 0;
        // read current level...
        while(spaceCount <2){
            int x=0;
            bool completeLine = false;
            bool failed = false;
            while(completeLine == false && failed == false){
                line[x+1]=0;
                if(fr.read(&line[x],1)){
                    if(line[x] == 0x0A){
                        completeLine = true;
                    }
                }else {
                    failed = true;
                }
                x++;
            }
            
            if(failed == false){
                if(strlen(line)<2){
                    spaceCount++; // surely blank line cant be more than 2 characters!
                    y = 0;
                }else{
                    //printf("length:%d\n", strlen(line));
                    for(x = 0; x< strlen(line); x++){
                        switch(line[x]){
                            case '#': // wall
                                curLevel[ldOffset+x+levWidth*(ldOffset+y)] = 1;
                                break;
                            case '.': // goal
                                curLevel[ldOffset+x+levWidth*(ldOffset+y)] = 2;
                                break;
                            case '$': // box
                                curLevel[ldOffset+x+levWidth*(ldOffset+y)] = 3;
                                boxes[numBoxes].x = tileSize * (x+ldOffset);
                                boxes[numBoxes].y = tileSize * (y+ldOffset);
                                boxes[numBoxes].frame = random(5);
                                numBoxes++;
                                break;
                            case '*': // box on goal
                                curLevel[ldOffset+x+levWidth*(ldOffset+y)] = 4;
                                boxes[numBoxes].x = tileSize * (x+ldOffset);
                                boxes[numBoxes].y = tileSize * (y+ldOffset);
                                boxes[numBoxes].frame = random(5);
                                numBoxes++;
                                break;
                            case '@': // player
                                curLevel[ldOffset+x+levWidth*(ldOffset+y)] = 5;
                                player.x = (ldOffset+x) * 32;
                                player.y = (ldOffset+y) * 32;
                                break;
                            case '+': // player on goal
                                curLevel[ldOffset+x+levWidth*(ldOffset+y)] = 6;
                                player.x = (ldOffset+x) * 32;
                                player.y = (ldOffset+y) * 32;
                                break;
                        } // switch
                    } // x
                    y++;
                } // len line < 2
                printf("%s",line);
            } // if read file
        } // if space < 2
        fr.close();

    } // if file open
}


void playLevel(){

        //drawSprite(32, 32, paint, paint_pal, 0, 8);
        spriteCount=0;
        if(player.walking==0){
            if (   _Up_But[HELD]){player.walking=true; player.direction=0; player.steps=0; } else
            if ( _Down_But[HELD]){player.walking=true; player.direction=1; player.steps=0; } else
            if ( _Left_But[HELD]){player.walking=true; player.direction=2; player.steps=0; } else
            if (_Right_But[HELD]){player.walking=true; player.direction=3; player.steps=0; };
        }
        
        if(player.walking){
            switch (player.direction){
                case 0:
                    player.y--;
                    player.steps++;
                    break;
                case 1:
                    player.y++;
                    player.steps++;
                    break;
                case 2:
                    player.x--;
                    player.steps++;
                    break;
                case 3:
                    player.x++;
                    player.steps++;
                    break;
            }
        }
        if(player.steps==32){
            player.steps=0;
            player.walking=false;
        }
        yScroll = player.y - 80;
        xScroll = player.x - 47;
        
        // Draw player
        drawSprite(player.x - xScroll, player.y - yScroll, hero[(player.direction * 4)+(player.steps/8)], hero_pal, 0, 8);
        
        // Draw Boxes
        for(int t=0; t<numBoxes; t++){
            drawSprite(boxes[t].x - xScroll, boxes[t].y - yScroll, box[boxes[t].frame], box_pal, 0, 8);
        }
        
}


int main() {

	PC::begin();
	PD::persistence=1;
    PD::adjustCharStep=0;
    PD::adjustLineStep=0;
    PD::fixedWidthFont = true;

    Pokitto::Display::lineFillers[0] = myBGFiller;
    Pokitto::Display::lineFillers[1] = spritesToLine;
    Pokitto::Display::lineFillers[2] = GUILine;

    loadSokLev(1);

    char tempText[15];
    uint32_t frameCount=0;
    uint8_t line[256];

    while (1) {
        
        if(!PC::update())continue;
        //PC::update();
        updateButtons();

        playLevel();
        
        sprintf(tempText,"[%d]FPS",fpsCount);
        //printf(tempText);
        guiPrint(0,0, tempText);

        fpsCounter++;
        if(PC::getTime() >= lastMillis+1000){
            lastMillis = PC::getTime();
            fpsCount = fpsCounter;
            fpsCounter = 0;
        }

    for(int t=22; t; t--){
        guiLineHasText[t] = 0;
    }

    }

    return 0;
}