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

#define MakeFlashString(name, value) \
  static const char __##name[] PROGMEM = value; \
  const __FlashStringHelper *name = reinterpret_cast<const __FlashStringHelper *>(__##name);

// labels
MakeFlashString(LBL_TIME, "Time");
MakeFlashString(LBL_DATE, "Date");
MakeFlashString(LBL_ID, "Id");
MakeFlashString(LBL_ACTIVE, "Active");
MakeFlashString(LBL_PROGRAM, "Program");
MakeFlashString(LBL_TIME_START, "Time start");
MakeFlashString(LBL_TIME_END, "Time end");
MakeFlashString(LBL_WEEKLY, "Weekly");
MakeFlashString(LBL_APPLY, "Apply");

// settings
PropertyTime::Time clockTime;
PropertyDate::Date clockDate;
PropertyTime clockProp(LBL_TIME, &clockTime);
PropertyDate dateProp(LBL_DATE, &clockDate);

Property *settingsProperties[] = {
	&clockProp,
	&dateProp,
	NULL
};

PropertyPage settingsPropPage(settingsProperties);

// recording
uint16_t id;
bool active;
uint16_t program;
PropertyDate::Date dateStart;
PropertyTime::Time timeStart;
PropertyTime::Time timeEnd;
bool weekly;

static void recordingApply() {
};

PropertyU16 idProp(LBL_ID, &id, 0, 9);
PropertyBool activeProp(LBL_ACTIVE, &active);
PropertyU16 programProp(LBL_PROGRAM, &program, 1, 999);
PropertyDate dateStartProp(LBL_DATE, &dateStart);
PropertyTime timeStartProp(LBL_TIME_START, &timeStart);
PropertyTime timeEndProp(LBL_TIME_END, &timeEnd);
PropertyBool weeklyProp(LBL_WEEKLY, &weekly);
PropertyAction recordingApplyProp(LBL_APPLY, recordingApply);

Property *recordingProperties[] = {
	&idProp,
	&activeProp,
	&programProp,
	&dateStartProp,
	&timeStartProp,
	&timeEndProp,
	&weeklyProp,
	NULL
};

PropertyPage recordingPropPage(recordingProperties);

// menu
PropertyPage *propertyPages[] = {
	&settingsPropPage,
	&recordingPropPage,
	NULL
};

//void *lcd = NULL;
//Menu menu(propertyPages, lcd);

namespace ema {
	///////////////////////////////////////////////////////////////////////////
	// Page
	///////////////////////////////////////////////////////////////////////////

	class Page
	{
	public:
		Page() {}
		virtual Page *pollEvent() { return NULL; }
		virtual void display() {}

	};

	///////////////////////////////////////////////////////////////////////////
	// Derived Page
	///////////////////////////////////////////////////////////////////////////

	Page *_mainPage;
	Page *_menuPage;
	Page *_settingsPage;
	Page *_dateTimePage;
	Page *_recordingPage;
	Page *_schedulerPage;

	class MainPage : public Page
	{
	public:
		Page *pollEvent();
		void display();
	};

	Page *MainPage::pollEvent()
	{
		if (!digitalRead(PIN_BUTTON_OK)) {
			return _menuPage;
		}
		return NULL;
	}

	void MainPage::display()
	{
		lcd.setCursor(0, 0);
		lcd.print("MainPage");
	}

	class MenuPage : public Page
	{
	public:
		Page *pollEvent();
		void display();
	};

	Page *MenuPage::pollEvent()
	{
		if (!digitalRead(PIN_BUTTON_ESC)) {
			return _mainPage;
		}
		return NULL;
	}

	void MenuPage::display()
	{
		lcd.setCursor(0, 0);
		lcd.print("MenuPage");
	}

	class SettingsPage : public Page
	{
	};

	class DateTimePage : public Page
	{
	};

	class RecordingPage : public Page
	{
	};

	class SchedulerPage : public Page
	{
	};

}

ema::MainPage mainPage;
ema::MenuPage menuPage;
ema::SettingsPage settingsPage;
ema::DateTimePage dateTimePage;
ema::RecordingPage recordingPage;
ema::SchedulerPage schedulerPage;

ema::Page *currentPage = &mainPage;

//The setup function is called once at startup of the sketch
void setup()
{
	ema::_mainPage = &mainPage;
	ema::_menuPage = &menuPage;
	ema::_settingsPage = &settingsPage;
	ema::_dateTimePage = &dateTimePage;
	ema::_recordingPage = &recordingPage;
	ema::_schedulerPage = &schedulerPage;

// Add your initialization code here
	Serial.begin(9600);
    Wire.begin();
    DS3231_init(0x06);
    lcd.begin(24,2);
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, HIGH);
	pinMode(PIN_DEC_POWER, INPUT);

	pinMode(PIN_BUTTON_OK, INPUT);
	digitalWrite(PIN_BUTTON_OK, HIGH);
	pinMode(PIN_BUTTON_ESC, INPUT);
	digitalWrite(PIN_BUTTON_ESC, HIGH);
	pinMode(PIN_BUTTON_DOWN, INPUT);
	digitalWrite(PIN_BUTTON_DOWN, HIGH);
	pinMode(PIN_BUTTON_UP, INPUT);
	digitalWrite(PIN_BUTTON_UP, HIGH);

	currentPage->display();
}

#define BUFF_MAX 128
// The loop function is called in an endless loop
bool done = false;

void testIrDisplay()
{
	  // technisat
    //irsend.sendRC5(0x1A84, 12);
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

void changePage(ema::Page *page)
{
	if (page != NULL) {
		currentPage = page;
		currentPage->display();
	}
}

void loop()
{
	//Add your repeated code here
	changePage(currentPage->pollEvent());
}
