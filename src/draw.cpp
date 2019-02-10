#include "draw.h"

#define DRAW_MODE_CLOCK           0
#define DRAW_MODE_SECONDS         1
#define DRAW_MODE_TEMP            2

#define DRAW_FONT_HEIGTH          7
#define DRAW_FONT_WIDTH           5

#define DRAW_FONT_XPOS1           0
#define DRAW_FONT_XPOS2           LED_MATRIX_COLS - DRAW_FONT_WIDTH
    
#define DRAW_FONT_YPOS_TOP        0    
#define DRAW_FONT_YPOS_CENTER     (LED_MATRIX_ROWS - DRAW_FONT_HEIGTH) / 2    
#define DRAW_FONT_YPOS_BOTTOM     LED_MATRIX_ROWS - DRAW_FONT_HEIGTH    
    
#define DRAW_CLOCK_REGION_WEST    0
#define DRAW_CLOCK_REGION_OST     1
#define DRAW_CLOCK_REGION_POTT    2 

#define DRAW_CLOCK_ITIS_OFF       0
#define DRAW_CLOCK_ITIS_HOURNHALF 1
#define DRAW_CLOCK_ITIS_PERMA     2


//Config
static uint8 drawMode                   = DRAW_MODE_CLOCK; //TODO aus config

static uint8 drawClockRegion            = 2; //TODO aus config
static uint8 drawClockItIsMode          = 1; //TODO aus config
static uint8 drawClockFadeMode          = 1; //TODO aus config

//Worker Vars
static uint8 drawClockLastSeconds       = 0;
static uint8 drawClockLastMinutes       = 0;
static uint8 drawClockLastHour          = 0;

static uint8 drawLastTemp               = 0;

static Ticker DrawTimer(DrawUpdate, 1000);  //TODO Fraktal for faster draw?

void DrawInit(){
    DrawTimer.start();
    DrawUpdate();
}

void DrawTick(){
    DrawTimer.update();
}

void DrawUpdate(){
    if(drawMode == DRAW_MODE_CLOCK){
        if(TimeHours() != drawClockLastHour || TimeMinutes() != drawClockLastMinutes){   
            Serial.println(TimeHours()); //TODO Debug
            Serial.println(TimeMinutes()); //TODO Debug         
            DrawUpdateClock(TimeHours(), TimeMinutes());
        }
    }

    if(drawMode == DRAW_MODE_SECONDS){
        if(TimeSeconds() != drawClockLastSeconds){
            Serial.println(TimeSeconds()); //TODO Debug
            DrawUpdateSeconds(TimeSeconds());
        }
    }

    if(drawMode == DRAW_MODE_TEMP){
        if(21 != drawLastTemp){            
            DrawUpdateTemp(21);
        }
    }
}

//#################
//## Uhr functions
//#################
void DrawUpdateClock(uint8 hour,uint8 minutes){
    drawClockLastHour  = hour;
    drawClockLastMinutes= minutes;
    uint8 onMode = !drawClockFadeMode;
    
    //### 12H Mod
    if(hour>12) hour = hour - 12;
    if(hour==0) hour = 12;

    //### Clear
    LedClear(onMode);

    //### Startdraw
    if( (drawClockItIsMode == DRAW_CLOCK_ITIS_PERMA) || 
        (drawClockItIsMode == DRAW_CLOCK_ITIS_HOURNHALF && minutes >= 0 && minutes < 5) ||
        (drawClockItIsMode == DRAW_CLOCK_ITIS_HOURNHALF && minutes >= 30 && minutes < 35) ){
        LedSetEsIst(onMode);
    }

    if(minutes < 5){
        LedSetUhr(onMode);
    }else if(minutes < 10){
        LedSetFuenfMin(onMode);
        LedSetNach(onMode);
    }else if(minutes < 15){
        LedSetZehnMin(onMode);
        LedSetNach(onMode);
    }else if(minutes < 20){        
        LedSetViertelMin(onMode);        
        if(drawClockRegion != DRAW_CLOCK_REGION_OST){
            LedSetNach(onMode);
        }
    }else if(minutes < 25){
        if(drawClockRegion == DRAW_CLOCK_REGION_POTT){
            LedSetZwanzigMin(onMode);
            LedSetNach(onMode);
        }else{
            LedSetZehnMin(onMode);
            LedSetVor(onMode);
            LedSetHalbMin(onMode);
        }
    }else if(minutes < 30){
        LedSetFuenfMin(onMode);
        LedSetVor(onMode);
        LedSetHalbMin(onMode);
    }else if(minutes < 35){
        LedSetHalbMin(onMode);
    }else if(minutes < 40){
        LedSetFuenfMin(onMode);
        LedSetNach(onMode);
        LedSetHalbMin(onMode);
    }else if(minutes < 45){
        if(drawClockRegion == DRAW_CLOCK_REGION_POTT){
            LedSetZwanzigMin(onMode);
            LedSetVor(onMode);
        }else{
            LedSetZehnMin(onMode);
            LedSetNach(onMode);
            LedSetHalbMin(onMode);
        }
    }else if(minutes < 50){           
        if(drawClockRegion == DRAW_CLOCK_REGION_OST){
            LedSetDreiviertelMin(onMode);
        }else{
            LedSetViertelMin(onMode);     
            LedSetVor(onMode);
        }
    }else if(minutes < 55){
        LedSetZehnMin(onMode);
        LedSetVor(onMode);
    }else if(minutes < 60){
        LedSetFuenfMin(onMode);
        LedSetVor(onMode);
    }

    uint8 minuteDots = minutes % 5;
    for(uint8 i = 0; i < minuteDots; i++){
        LedSetMinDot(i,onMode);
    }

    //Stunden Shift je nach Regionslayout
    if(
        (drawClockRegion == DRAW_CLOCK_REGION_POTT && minutes >= 25)||
        (drawClockRegion == DRAW_CLOCK_REGION_WEST && minutes >= 20)||
        (drawClockRegion == DRAW_CLOCK_REGION_OST  && minutes >= 15)
      ){
        hour++;
        if(hour > 12){
            hour = 1;
        }
    }

    if(hour == 1){
        if(minutes < 5){
            LedSetEinHour(onMode);
        }else{
            LedSetEinsHour(onMode);
        }
    } else if(hour == 2){
        LedSetZweiHour(onMode);
    } else if(hour == 3){
        LedSetDreiHour(onMode);
    } else if(hour == 4){
        LedSetVierHour(onMode);
    } else if(hour == 5){
        LedSetFuenfHour(onMode);
    } else if(hour == 6){
        LedSetSechsHour(onMode);
    } else if(hour == 7){
        LedSetSiebenHour(onMode);
    } else if(hour == 8){
        LedSetAchtHour(onMode);
    } else if(hour == 9){
        LedSetNeunHour(onMode);
    } else if(hour == 10){
        LedSetZehnHour(onMode);
    } else if(hour == 11){
        LedSetElfHour(onMode);
    } else if(hour == 12){
        LedSetZwoelfHour(onMode);
    } 
}

//#################
//## text functions
//#################
void drawFontAt(uint8 character, uint8 atCol, uint8 atRow, uint8 hard){    
    uint8 mode = hard ? LED_STATIS_ON : LED_STATIS_FADEIN;
    for(uint8 fontCol=0; fontCol < DRAW_FONT_WIDTH;fontCol++){
        for(uint8 fontRow=0; fontRow < DRAW_FONT_HEIGTH; fontRow++){
            uint8 row = atRow + fontRow;
            uint8 col = atCol + fontCol;
            if (font_7x5[character][fontCol] & (1 << fontRow)) {                  
                LedSetStatusMatrix(row,col,mode);                
            }
        }
    }
}

void DrawUpdateSeconds(uint8 seconds){
    drawClockLastSeconds = seconds;

    uint8 char1 = seconds / 10;
    uint8 char2 = seconds % 10;

    LedClear(1);

    drawFontAt(char1+48,DRAW_FONT_XPOS1,DRAW_FONT_YPOS_CENTER,1);
    drawFontAt(char2+48,DRAW_FONT_XPOS2,DRAW_FONT_YPOS_CENTER,1);     
    
    //TODO fade?      
} 

void DrawUpdateTemp(uint8 temperature){
    uint8 char1 = temperature / 10;
    uint8 char2 = temperature % 10;
    
    LedClear(1);

    drawFontAt(char1+48,DRAW_FONT_XPOS1,DRAW_FONT_YPOS_CENTER,1);
    drawFontAt(char2+48,DRAW_FONT_XPOS2,DRAW_FONT_YPOS_CENTER,1); 

    //celsius Punkt
    LedSetMinDot(0,1);

    //TODO fade?
}