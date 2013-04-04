// Do not remove the include below
#include "IrScheduler.h"

const int buttonPin = 2;
const int ledPin = 4;
IRsend irsend;

//The setup function is called once at startup of the sketch
void setup()
{
// Add your initialization code here
	Serial.begin(9600);
	pinMode(ledPin, OUTPUT);
	pinMode(buttonPin, INPUT);
	digitalWrite(ledPin, LOW);
}

enum Key {
	k_standby =    0x9A65,
	k_av =         0x1AE5,
	k_tv_radio =   0x18E7,
	k_mute =       0x9867,
	k_vol_plus =   0xB24D,
	k_epg =        0x708F,
	k_ch_plus =    0xB04F,
	k_vol_minus =  0x8A75,
	k_menu =       0x48B7,
	k_ch_minus =                0x8877,
	k_audio    =                0xA25D,
	k_sub      =                0xA05F,
	k_up       =                0x609F,
	k_left     =                0x5AA5,
	k_ok       =                0x58A7,
	k_right    =                0xD827,
	k_down     =                0x6897,
	k_exit     =                0xAA55,
	k_back     =                0xA857,
	k_1        =                0x4AB5,
	k_2        =                0x0AF5,
	k_3        =                0x08F7,
	k_text     =                0xC837,
	k_4        =                0x6A95,
	k_5        =                0x2AD5,
	k_6        =                0x28D7,
	k_info      =               0xE817,
	k_7        =                0x728D,
	k_8        =                0x32CD,
	k_9        =                0x30CF,
	k_0        =                0xF00F,
	k_red       =               0x52AD,
	k_green     =               0x12ED,
	k_yellow    =               0x10EF,
	k_blue      =               0xD02F,
	k_prev      =               0x629D,
	k_rew       =               0x22DD,
	k_fwd       =               0x20DF,
	k_next      =               0xE01F,
	k_rec       =               0x42BD,
	k_file      =               0x02FD,
	k_play_pause =              0x00FF,
	k_stop       =              0xC03F,
	k_end_seq = 0
};

const Key CHANNELS[] = {
		k_0,
		k_1,
		k_2,
		k_3,
		k_4,
		k_5,
		k_6,
		k_7,
		k_8,
		k_9
};

const Key SEQUENCE1[] = { k_1, k_7, k_9, k_end_seq };
const Key SEQUENCE2[] = { k_2, k_2, k_end_seq };
const Key SEQUENCE_STOP[] = { k_stop, k_ok, k_end_seq };
const unsigned long PRE_DATA = 0x00FD0000;

const int RECORDINGS[] = {5, -1, 7, -1, 22, -1};

// The loop function is called in an endless loop
bool pressed = false;

void sendKey(Key k, unsigned long wait = 400)
{
	digitalWrite(ledPin, HIGH);
	irsend.sendNEC(PRE_DATA | k, 32);
	digitalWrite(ledPin, LOW);
	delay(wait);
}

void sendSequence(const Key *sequence, unsigned long wait = 400)
{
	while (*sequence != k_end_seq) {
		sendKey(*sequence, wait);
		sequence++;
	}
}

#define ARRAY_LEN(a) (sizeof(a)/sizeof(a[0]))

void startRec(int ch) {
	Key seq[5];
	seq[4] = k_end_seq;
	for (byte i = 0; i < ARRAY_LEN(seq)-1; ++i) {
		seq[ARRAY_LEN(seq)-i-2] = CHANNELS[ch % 10];
		ch /= 10;
	}
	sendKey(k_standby, 20000);
	byte skip = 0;
	for (byte i = 0; i < ARRAY_LEN(seq)-2; ++i) {
		if (seq[i] == k_0) {
			skip++;
		}
	}
	sendSequence(seq + skip);
	delay(5000);
	sendKey(k_rec);
}

void stopRec() {
	sendSequence(SEQUENCE_STOP, 1000);
	delay(2000);
	sendKey(k_standby);
}

size_t next_event = 0;

void loop()
{
//Add your repeated code here
	  // technisat
      //irsend.sendRC5(0x1A84, 12);
	int buttonState = digitalRead(buttonPin);

	  // check if the pushbutton is pressed.
	  // if it is, the buttonState is HIGH:
	  if (buttonState == HIGH) {
		if (!pressed && next_event < ARRAY_LEN(RECORDINGS)) {
			pressed = true;
			if (RECORDINGS[next_event]  < 0) {
				stopRec();
			} else {
				startRec(RECORDINGS[next_event]);
			}
			next_event++;
		}
	  } else {
	    pressed = false;
	  }
}
