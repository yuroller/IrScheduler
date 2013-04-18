// Do not remove the include below
#include "IrScheduler.h"

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

enum Pin {
	PIN_IR_TX = 3,
	PIN_BUTTON_UP = 7,
	PIN_BUTTON_DOWN = 6,
	PIN_BUTTON_ESC = 8,
	PIN_BUTTON_OK = 9,
	PIN_LED = 13,
	PIN_LCD_RS = 12,
	PIN_LCD_E = 11,
	PIN_LCD_D4 = 5,
	PIN_LCD_D5 = 4,
	PIN_LCD_D6 = 10,
	PIN_LCD_D7 = 2,
	PIN_LCD_BACKLIGHT = 0,
	PIN_DEC_POWER = A3
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

const int RECORDINGS[] = {26, -1, 7, -1, 22, -1};

// global objects
IRsend irsend;
menwiz tree;
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_E,
		PIN_LCD_D4, PIN_LCD_D5,	PIN_LCD_D6, PIN_LCD_D7);

bool pressed = false;

void sendKey(Key k, unsigned long wait = 400)
{
	digitalWrite(PIN_LED, LOW);
	irsend.sendNEC(PRE_DATA | k, 32);
	digitalWrite(PIN_LED, HIGH);
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

bool IsDecoderOn()
{
	int foto = digitalRead(PIN_DEC_POWER);
	return foto == LOW;
}

void startRec(int ch) {
	Key seq[5];
	seq[4] = k_end_seq;
	for (byte i = 0; i < ARRAY_LEN(seq)-1; ++i) {
		seq[ARRAY_LEN(seq)-i-2] = CHANNELS[ch % 10];
		ch /= 10;
	}
	if (!IsDecoderOn())
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

size_t next_event;

void applyClock() {
  Serial.println("Apply clock");
}

void applySchedule() {
  Serial.println("Apply schedule");
}

void applySettings() {
  Serial.println("Apply settings");
}

int clock_year = 2000;
byte clock_month = 1, clock_day= 1,
  clock_hour = 0, clock_mins = 0;
byte id = 0;
boolean active = 0;
int program = 1;
byte start_month = 1, start_day = 1,
	start_hour = 0, start_mins = 0,
	end_hour = 0, end_mins = 0;
byte pre_recording = 2, post_recording = 10;

//The setup function is called once at startup of the sketch
void setup()
{
// Add your initialization code here
	Serial.begin(9600);
    Wire.begin();
    DS3231_init(0x06);
    lcd.begin(24,2);
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, HIGH);
	pinMode(PIN_DEC_POWER, INPUT);

#if 0
	tree.begin(&lcd, 24, 2);
	_menu *menu_root;
	_menu *menu_set_clock,
		*menu_set_year, *menu_set_month, *menu_set_day,
		*menu_set_hour, *menu_set_mins,
		*menu_apply_clock;
	_menu *menu_schedule_recording,
		*menu_set_id,
		*menu_set_active,
		*menu_set_program,
		*menu_set_start_month, *menu_set_start_day,
		*menu_set_start_hour, *menu_set_start_mins,
		*menu_set_end_hour, *menu_set_end_mins,
		*menu_apply_schedule;
	_menu *menu_settings,
		*menu_set_pre_recording,
		*menu_set_post_recording,
		*menu_apply_settings;
	_menu *menu_transmit_ir;
	const __FlashStringHelper *APPLY = F("Apply");

	menu_root = tree.addMenu(MW_ROOT, NULL, F("Root"));
	  menu_set_clock = tree.addMenu(MW_SUBMENU, menu_root, F("Set Clock"));
	    menu_set_year = tree.addMenu(MW_VAR, menu_set_year, F("Year"));
	      menu_set_year->addVar(MW_AUTO_INT, &clock_year, 2000, 2099, 1);
	    menu_set_month = tree.addMenu(MW_VAR, menu_set_month, F("Month"));
		  menu_set_month->addVar(MW_AUTO_BYTE, &clock_month, 1, 12, 1);
		menu_set_day = tree.addMenu(MW_VAR, menu_set_day, F("Day"));
		  menu_set_day->addVar(MW_AUTO_BYTE, &clock_day, 1, 31, 1);
	    menu_set_hour = tree.addMenu(MW_VAR, menu_set_clock, F("Hour"));
	      menu_set_hour->addVar(MW_AUTO_BYTE, &clock_hour, 0, 23, 1);
	    menu_set_mins = tree.addMenu(MW_VAR, menu_set_clock, F("Mins"));
	      menu_set_mins->addVar(MW_AUTO_BYTE, &clock_mins, 0, 55, 5);
	    menu_apply_clock = tree.addMenu(MW_VAR, menu_set_clock, APPLY);
	      menu_apply_clock->addVar(MW_ACTION, applyClock);
	  menu_schedule_recording = tree.addMenu(MW_SUBMENU, menu_root, F("Schedule Recording"));
	  	menu_set_id = tree.addMenu(MW_VAR, menu_schedule_recording, F("Id"));
	  	  menu_set_id->addVar(MW_AUTO_BYTE, &id, 0, 9, 1);
	    menu_set_active = tree.addMenu(MW_VAR, menu_schedule_recording, F("Active"));
	      menu_set_active->addVar(MW_BOOLEAN, &active);
	    menu_set_program = tree.addMenu(MW_VAR, menu_schedule_recording, F("Program"));
	      menu_set_program->addVar(MW_AUTO_INT, &program, 1, 999, 1);
	    menu_set_start_month = tree.addMenu(MW_VAR, menu_schedule_recording, F("Start Month"));
		  menu_set_start_month->addVar(MW_AUTO_BYTE, &start_month, 1, 12, 1);
	    menu_set_start_day = tree.addMenu(MW_VAR, menu_schedule_recording, F("Start Day"));
	      menu_set_start_day->addVar(MW_AUTO_BYTE, &start_day, 1, 31, 1);
	    menu_set_start_hour = tree.addMenu(MW_VAR, menu_schedule_recording, F("Start Hour"));
	      menu_set_start_hour->addVar(MW_AUTO_BYTE, &start_hour, 0, 23, 1);
	    menu_set_start_mins = tree.addMenu(MW_VAR, menu_schedule_recording, F("Start Mins"));
	      menu_set_start_mins->addVar(MW_AUTO_BYTE, &start_mins, 0, 55, 5);
	    menu_set_end_hour = tree.addMenu(MW_VAR, menu_schedule_recording, F("End Hour"));
	      menu_set_end_hour->addVar(MW_AUTO_BYTE, &end_hour, 0, 23, 1);
	    menu_set_end_mins = tree.addMenu(MW_VAR, menu_schedule_recording, F("End Mins"));
	      menu_set_end_mins->addVar(MW_AUTO_BYTE, &end_mins, 0, 55, 5);
	    menu_apply_schedule = tree.addMenu(MW_VAR, menu_schedule_recording, APPLY);
	      menu_apply_schedule->addVar(MW_ACTION, applySchedule);
	  menu_settings = tree.addMenu(MW_SUBMENU, menu_root, F("Settings"));
	    menu_set_pre_recording = tree.addMenu(MW_VAR, menu_settings, F("Pre-recording"));
	      menu_set_pre_recording->addVar(MW_AUTO_BYTE, &pre_recording, 0, 60, 1);
	    menu_set_post_recording = tree.addMenu(MW_VAR, menu_settings, F("Post-recording"));
	      menu_set_post_recording->addVar(MW_AUTO_BYTE, &post_recording, 0, 60, 1);
	    menu_apply_settings = tree.addMenu(MW_VAR, menu_settings, APPLY);
	      menu_apply_settings->addVar(MW_ACTION, applySettings);
	  menu_transmit_ir = tree.addMenu(MW_VAR, menu_root, F("Transmit IR"));
	    menu_transmit_ir->addVar(MW_ACTION, stopRec);

	  tree.navButtons(PIN_BUTTON_UP, PIN_BUTTON_DOWN, PIN_BUTTON_ESC, PIN_BUTTON_OK);
#endif

}

#define BUFF_MAX 128
// The loop function is called in an endless loop
bool done = false;
void loop()
{
//Add your repeated code here
	  // technisat
      //irsend.sendRC5(0x1A84, 12);
//	tree.draw();
#if 0
	int buttonState = digitalRead(PIN_BUTTON_OK);

	  // check if the pushbutton is pressed.
	  // if it is, the buttonState is HIGH:
	  if (buttonState == LOW) {
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
#endif
	  digitalWrite(PIN_LED, IsDecoderOn());
	  char buff[BUFF_MAX];
	  struct ts t;
	  DS3231_get(&t);
      snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year,
           t.mon, t.mday, t.hour, t.min, t.sec);
      lcd.home();
      lcd.print(buff);
      Serial.println(buff);
      delay(1000);
      if (t.hour == 23 && t.min == 10 && !done) {
    	  startRec(RECORDINGS[0]);
    	  done = true;
      }
}
