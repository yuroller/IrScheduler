/*
 * Telesystems.h
 *
 *  Created on: 04/giu/2013
 *      Author: YV
 */

#ifndef TELESYSTEMS_H_
#define TELESYSTEMS_H_

enum IrKey {
	IR_STANDBY 		= 0x9A65,
	IR_AV 			= 0x1AE5,
	IR_TV_RADIO 	= 0x18E7,
	IR_MUTE 		= 0x9867,
	IR_VOL_PLUS 	= 0xB24D,
	IR_EPG 			= 0x708F,
	IR_CH_PLUS 		= 0xB04F,
	IR_VOL_MINUS 	= 0x8A75,
	IR_MENU 		= 0x48B7,
	IR_CH_MINUS 	= 0x8877,
	IR_AUDIO 		= 0xA25D,
	IR_SUB 			= 0xA05F,
	IR_UP 			= 0x609F,
	IR_LEFT 		= 0x5AA5,
	IR_OK 			= 0x58A7,
	IR_RIGHT 		= 0xD827,
	IR_DOWN 		= 0x6897,
	IR_EXIT 		= 0xAA55,
	IR_BACK 		= 0xA857,
	IR_1 			= 0x4AB5,
	IR_2 			= 0x0AF5,
	IR_3 			= 0x08F7,
	IR_TEXT 		= 0xC837,
	IR_4 			= 0x6A95,
	IR_5 			= 0x2AD5,
	IR_6 			= 0x28D7,
	IR_INFO 		= 0xE817,
	IR_7 			= 0x728D,
	IR_8 			= 0x32CD,
	IR_9 			= 0x30CF,
	IR_0 			= 0xF00F,
	IR_RED 			= 0x52AD,
	IR_GREEN 		= 0x12ED,
	IR_YELLOW 		= 0x10EF,
	IR_BLUE 		= 0xD02F,
	IR_PREV 		= 0x629D,
	IR_REW 			= 0x22DD,
	IR_FWD 			= 0x20DF,
	IR_NEXT 		= 0xE01F,
	IR_REC 			= 0x42BD,
	IR_FILE 		= 0x02FD,
	IR_PLAY_PAUSE 	= 0x00FF,
	IR_STOP 		= 0xC03F,

	IR_NONE			= 0
};

extern void sendIrKey(IrKey k);

#endif /* TELESYSTEMS_H_ */
