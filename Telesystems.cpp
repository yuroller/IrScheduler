/*
 * Telesystems.cpp
 *
 *  Created on: 04/giu/2013
 *      Author: YV
 */

#include "Telesystems.h"
#include "IRremote.h"

static IRsend irsend;

const unsigned long IR_PRE_DATA_MASK = 0x00FD0000;

// 400ms between keys
void sendIrKey(IrKey k)
{
	irsend.sendNEC(IR_PRE_DATA_MASK | k, 32);
}
