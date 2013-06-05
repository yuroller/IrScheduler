// Do not remove the include below
#include "IrScheduler.h"
#include "Telesystems.h"

enum Pin {
	PIN_IR_TX = 3,
	PIN_BUTTON_UP = 7,
	PIN_BUTTON_DOWN = 6,
	PIN_BUTTON_ENTER = 9,
	PIN_LED = 13,
	PIN_LCD_RS = 12,
	PIN_LCD_E = 11,
	PIN_LCD_D4 = 5,
	PIN_LCD_D5 = 4,
	PIN_LCD_D6 = 10,
	PIN_LCD_D7 = 2,
	//PIN_LCD_BACKLIGHT = 8,
	PIN_DEC_POWER = A3
};

enum {
	LCD_WIDTH = 24,
	LCD_HEIGHT = 2
};


const IrKey CHANNELS[] = {
		IR_0,
		IR_1,
		IR_2,
		IR_3,
		IR_4,
		IR_5,
		IR_6,
		IR_7,
		IR_8,
		IR_9
};

const IrKey SEQUENCE1[] = { IR_1, IR_7, IR_9, IR_NONE };
const IrKey SEQUENCE2[] = { IR_2, IR_2, IR_NONE };
const IrKey SEQUENCE_STOP[] = { IR_STOP, IR_OK, IR_NONE };

const int RECORDINGS[] = {26, -1, 7, -1, 22, -1};

// global objects
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_E,
		PIN_LCD_D4, PIN_LCD_D5,	PIN_LCD_D6, PIN_LCD_D7);
Button button_up = Button();
Button button_down = Button();
Button button_enter = Button();

void sendKey(IrKey k, unsigned long wait = 400)
{
	digitalWrite(PIN_LED, LOW);
	sendIrKey(k);
	digitalWrite(PIN_LED, HIGH);
	delay(wait);
}

void sendSequence(const IrKey *sequence, unsigned long wait = 400)
{
	while (*sequence != 0) {
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
	IrKey seq[5];
	seq[4] = IR_NONE;
	for (byte i = 0; i < ARRAY_LEN(seq)-1; ++i) {
		seq[ARRAY_LEN(seq)-i-2] = CHANNELS[ch % 10];
		ch /= 10;
	}
	if (!IsDecoderOn())
		sendKey(IR_STANDBY, 20000);
	byte skip = 0;
	for (byte i = 0; i < ARRAY_LEN(seq)-2; ++i) {
		if (seq[i] == IR_0) {
			skip++;
		}
	}
	sendSequence(seq + skip);
	delay(5000);
	sendKey(IR_REC);
}

void stopRec() {
	sendSequence(SEQUENCE_STOP, 1000);
	delay(2000);
	sendKey(IR_STANDBY);
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
MakeFlashString(LBL_CLOCK, "Clock");
MakeFlashString(LBL_SETTINGS, "Settings");
MakeFlashString(LBL_RECORDINGS, "Recordings");

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

static void recordingApply()
{
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
	&recordingApplyProp,
	NULL
};

PropertyPage recordingPropPage(recordingProperties);

//RecListPage recListPage(&recordingPropPage);

MenuItem menuSettingsItem(LBL_SETTINGS, &settingsPropPage);
MenuItem menuRecordingItem(LBL_RECORDINGS, &recordingPropPage);

// menu
MenuItem *mainMenuItems[] = {
	&menuSettingsItem,
	&menuRecordingItem,
	NULL
};

MenuItemPage mainMenuPage(mainMenuItems);

#define BUFF_MAX 128
// The loop function is called in an endless loop
bool done = false;
bool pressed = false;

void testIrDisplay()
{
	  // technisat
    //irsend.sendRC5(0x1A84, 12);
#if 0
	int buttonState = digitalRead(PIN_BUTTON_ENTER);

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

static ButtonPress getButtonPress()
{
	ButtonPress b = BUTTON_PRESS_NONE;
	if (button_up.check() == ON) {
		b = BUTTON_PRESS_UP;
	} else if (button_down.check() == ON) {
		b = BUTTON_PRESS_DOWN;
	} else if (button_enter.check() == ON) {
		b = BUTTON_PRESS_ENTER;
	}
	return b;
}

//The setup function is called once at startup of the sketch
void setup()
{
	Serial.begin(9600);
    Wire.begin();
    DS3231_init(0x06);
    lcd.begin(24,2);
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, HIGH);
	pinMode(PIN_DEC_POWER, INPUT);

	button_up.assign(PIN_BUTTON_UP);
	button_up.turnOnPullUp();
	button_down.assign(PIN_BUTTON_DOWN);
	button_down.turnOffPullUp();
	button_enter.assign(PIN_BUTTON_ENTER);
	button_enter.turnOnPullUp();
}

void loop()
{
	Screen screen(&lcd, LCD_WIDTH, LCD_HEIGHT);
	Page *page = &mainMenuPage;
	page->paint(&screen);
	ButtonPress b = getButtonPress();
	uint8_t line = page->buttonInput(b, &screen);
	if (page == &mainMenuPage && line != Page::INVALID_LINE && line != 0) {
		page = mainMenuItems[line-1]->getPage();
		page->reset();
		page->paint(&screen);
	} else if ((page == &settingsPropPage || page == &recordingPropPage) && line == 0) {
		page = &mainMenuPage;
		//page->reset(); // no reset for keeping parent position
		page->paint(&screen);
	}

}
