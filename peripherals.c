#include "EVE.h"
#include "LP5018.h"
#include "board.h"
#include <stdarg.h>

#define GRAY  	0x919191UL
#define BLACK  	0x222222UL
#define WHITE  	0xFFFFFFUL
#define LAYOUT_Y1 120

extern volatile bool peripheralThreadStop;

char printBuf[1024];
char timeString[10];
char timeString2[2];
char dateString[20];
int sec;
int min;
int hour;
bool speakerOn;

void LED_init(){
	bool retVal = false;
	I2C_init();
	/* Create I2C for usage */
	I2C_Params_init(&i2cParams);
	i2cParams.bitRate = I2C_100kHz;
	i2cHandle = I2C_open(Board_I2C1, &i2cParams);
	LP5018_init();
	LP5018_setAllBrightness(0);
	LP5018_setAllColor(0,128,0);
}

void LED_on(int nLed){
	LP5018_setBrightness(nLed, 128);
}

void LED_allOn(void){
	LP5018_setAllBrightness(128);
}

void LED_off(int nLed){
	LP5018_setBrightness(nLed, 0);
}

void LED_allOff(void){
	LP5018_setAllBrightness(0);
}

void Screen_background(){
	EVE_cmdBGColor(BLACK);
	EVE_cmd(VERTEX_FORMAT(0));
	// Divide the screen
	EVE_cmd(DL_BEGIN | LINES);
	EVE_cmd(LINE_WIDTH(1*16));
	EVE_colorRGB(GRAY);
	EVE_cmd(VERTEX2F(0,LAYOUT_Y1-2));
	EVE_cmd(VERTEX2F(HSIZE,LAYOUT_Y1-2));
	EVE_cmd(DL_END);
}

void Screen_init(){
    SPI_init();
    SPI_Params_init(&spiParams);
    spiParams.bitRate = 1000000;
    spiHandle = SPI_open(Board_SPI4, &spiParams);
	EVE_init();
	EVE_initFlash();
    // Loading screen while waiting for wifi
	EVE_startBurst();
	EVE_cmd(CMD_DLSTART);
    EVE_cmd(DL_CLEAR_RGB | BLACK);
    EVE_cmd(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
    EVE_cmdSpinner(400,240,1,1);
	EVE_cmd(DL_DISPLAY);
	EVE_cmd(CMD_SWAP);
	EVE_sendBurst();
}

void Screen_printf(const char *format, ...){
	va_list args;
	va_start(args, format);
	vsprintf(printBuf, format, args);
	va_end(args);
}

void Screen_printMedInfo(char *MedInfo){
	sprintf(printBuf, MedInfo);
}

void Screen_reset(){
	memset(printBuf,0,sizeof(printBuf));
	Screen_update();
}

void Screen_updateTime(int Hour, int Min){
	// Assuming 24 hour input, convert to 12 hour
	sprintf(timeString2, Hour > 11 && Hour < 24 ? "PM":"AM");
	Hour = Hour > 12 ? Hour-12:Hour;
	// Zero pad single digit min
	sprintf(timeString, min < 10 ? "%d:0%d":"%d:%d", Hour, Min);
}

void Screen_updateDate(char *Date){
	sprintf(dateString, Date);
}

void Screen_update(){
	EVE_startBurst();
	// Clear screen
	EVE_cmd(CMD_DLSTART);
	EVE_cmd(DL_CLEAR_RGB | BLACK);
	EVE_cmd(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
	// Static background
	Screen_background();
	// Display date/time
	EVE_cmdText(580, 20, 31, 0, timeString);
	EVE_cmdText(750, 40, 28, 0, timeString2);
	EVE_cmdText(580, 65, 29, 0, dateString);
	// Display medication info / other info
	EVE_cmdText(10, 150, 30, 0, printBuf);
	EVE_cmd(DL_DISPLAY);
	EVE_cmd(CMD_SWAP);
	EVE_sendBurst();
}

void *peripheralThreadProc(void *arg0){
	Screen_updateDate("November 14, 2020");
	sec = 58;
	min = 59;
	hour = 11;
	while(!peripheralThreadStop){
		// Simple clock, replace with rtc
		sec++;
		delay(1000);
        if (sec == 60) {
            sec = 0;
            min++;
            if (min == 60) {
                min = 0;
                hour++;
                if(hour == 25){
                    hour = 1;
                }
            }
        }
        Screen_updateTime(hour, min);
	    // Update display
		Screen_update();
	}
	return (0);
}

void Speaker_init(void){
	EVE_setVolume(0x80);
	EVE_setSound(SQUAREWAVE, MIDI_C1);
}

void Speaker_on(void){
	EVE_startSound();
}

void Speaker_off(void){
	EVE_stopSound();
}
