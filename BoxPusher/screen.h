inline void spritesToLine(std::uint8_t* line, std::uint32_t y, bool skip){

    if(spriteLine[y]==0) return;
    auto scanLine = &Pokitto::Display::palette[32]; // start 32 pixels in because of underflow
    #define width 32
    #define height 32
    
    int y2 = y;
    
    spriteLine[y]=0;
    if(spriteCount>=0){
        for(uint32_t spriteIndex = 1; spriteIndex<=spriteCount; spriteIndex++){
            auto &sprite = sprites[spriteIndex];

//    printf("%03d,",sprite.y); // here sprite.y can still be -16

            if((int)y >= sprite.y){

                if(sprite.y<0) printf("%03d,",sprite.y); // here sprite.y can still be <0

                if((int)y < sprite.y + height){
    
                if(sprite.y<0) printf("Still here!"); // here sprite.y can not be <0
                
                
//                if(spriteIndex==3)printf("%d,%d,%d\n",spriteIndex, sprite.x, sprite.y);
                if(sprite.x>-width && sprite.x<PROJ_LCDWIDTH){
                    uint32_t so = 2+(width * (y2-sprite.y));
                    auto si = &sprite.imageData[so];
                    auto sl = &scanLine[sprite.x];
                    auto palette = sprite.paletteData;
                    #define midLoop()\
                        if(auto pixel = *si) *sl = palette[pixel];\
                        si++; sl++;
//                    for(auto slEnd = sl + width; sl != slEnd; ){
//                        midLoop();
                        midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); 
                        midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); 
                        midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); 
                        midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); midLoop(); 
//                    }
                } // if X
            } // if Y
            } // if Y
        } // for spriteCount
    } // sprite count >1
    printf("\n");
}



inline void GUILine(std::uint8_t* line, std::uint32_t y, bool skip){

    if(skip)return;
    if(guiLineHasText[y/8]==0) return;

    int32_t x = 0;

    int32_t tileIndex = (y/8) * 27;
    int32_t lineOffset;
    int32_t alpha;
    int32_t temp;
    auto &tileRef = guiFont[0];
    auto scanLine = &Pokitto::Display::palette[32]; // start 32 pixels in because of underflow

    for(int d=0; d<27; d++){
        lineOffset = 2 + ((y%8)*4) + guiBG[tileIndex++]*34;
        for(int c=0; c<4; c++){
            temp = tileRef[lineOffset]>>4;
            if(temp) scanLine[x] = guiFont_pal[temp];
            x++;
            temp = tileRef[lineOffset++]&15;
            if(temp) scanLine[x] = guiFont_pal[temp];
            x++;
        }
    }

}


inline void myBGFiller(std::uint8_t* line, std::uint32_t y, bool skip){

    if(y==0){
        for(uint32_t x=0; x<220; ++x){
            line[x]=x+32;
        }        
    }

    #define TILEWIDTH 32
    #define TILEHEIGHT 32

    // how far off screen should we render the map
    int x = -(xScroll%TILEWIDTH)+TILEWIDTH;
    // find current tile in the map
    int tileIndex = (xScroll/TILEWIDTH) + ((y+yScroll)/TILEHEIGHT) * levWidth;
    // Find first pixel in current line of the tile
    int jStart = 2+((y+yScroll)%TILEHEIGHT)*TILEWIDTH;

    auto lineP = &Pokitto::Display::palette[x];
    auto tilesP = &tiles[0];

    #define bgTileLine()\
        lineP = &Pokitto::Display::palette[x];\
        tilesP = &tiles[(curLevel[tileIndex++]*1024) + jStart];\
        x+=32;\
        *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++];\
        *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++];\
        *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++];\
        *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++];\
        *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++];\
        *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++];\
        *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++];\
        *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++];\
        *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++];\
        *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++];\
        *lineP++ = tiles_pal[*tilesP++]; *lineP++ = tiles_pal[*tilesP++];

    #define bgHalfTileLine()\
        lineP = &Pokitto::Display::palette[x];\
        tilesP = &tiles[(curLevel[tileIndex++]*1024) + jStart];\
        while(x++ < 252){\
            *lineP++ = tiles_pal[*tilesP++];\
        }

    bgTileLine(); bgTileLine(); bgTileLine(); bgTileLine();
    bgTileLine(); bgTileLine(); bgTileLine();
    bgHalfTileLine();

    //GUILine(line, y, 0);
    //spritesToLine(y);
    //flushLine(Pokitto::Display::palette, line);
}


