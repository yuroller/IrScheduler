/*
  PropertyMenu.cpp - Arduino lcd menu with property editing library
  Written by Yuri Valentini <yuroller [at] gmail.com>
  Copyright (c) 2013 Yuri Valentini, All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "PropertyMenu.h"

const char SEL_LEFT = '[';
const char SEL_RIGHT = ']';
const char CHECKED = 'v';
const char UNCHECKED = ' ';
const char CHK_LEFT = '(';
const char CHK_RIGHT = ')';
const char SPACE = ' ';
const char CURSOR = '>';
const char REPLY_YES = 'Y';
const char REPLY_NO = 'N';
const char PREV_MENU[] = "..";

static void pad00Print(LCD *lcd, uint8_t n)
{
	assert(lcd != NULL);
	assert(n < 100);
	if (n < 10) {
		lcd->print('0');
	}
	lcd->print(n);
}

static void padMulti0Print(LCD *lcd, uint16_t n, uint8_t width)
{
	assert(lcd != NULL);
	uint16_t m = 10;
	for (uint8_t i = 1; i < width; ++i) {
		if (n < m) {
			lcd->print('0');
		}
		m *= 10;
	}
	assert(n < m);
	lcd->print(n);
}

template<typename T>
static void clipValue(T *n, T low, T high)
{
	assert(n != NULL);
	assert(low < high);

	if (*n < low) {
		*n = low;
	} else if (*n > high) {
		*n = high;
	}
}

template<typename T>
static void wrapDecrease(T *n, T low, T high)
{
	assert(n != NULL);
	assert(low < high);

	if (*n == low) {
		*n = high;
	} else {
		(*n)--;
	}
}

template<typename T>
static void wrapIncrease(T *n, T low, T high)
{
	assert(n != NULL);
	assert(low < high);

	if (*n == high) {
		*n = low;
	} else {
		(*n)++;
	}
}

///////////////////////////////////////////////////////////////////////////
// Screen
///////////////////////////////////////////////////////////////////////////

Screen::Screen(LCD *lcd, uint8_t cols, uint8_t rows)
: _lcd(lcd),
	_cols(cols),
	_rows(rows)
{
	assert(_lcd != NULL);
	assert(_cols > 0);
	assert(_rows > 0);
	_lcd->begin(cols, rows);
}

///////////////////////////////////////////////////////////////////////////
// Property
///////////////////////////////////////////////////////////////////////////

Property::Property(const __FlashStringHelper *name, uint8_t maxFocusParts)
: _name(name),
	_focusPart(0),
	_maxFocusParts(maxFocusParts)
{
	assert(name != NULL);
	assert(maxFocusParts > 0);
}

Property::~Property()
{
}

void Property::nextFocusPart()
{
	_focusPart++;
	if (_focusPart > _maxFocusParts) {
		_focusPart = 0;
		onExitEdit();
	}
}

void Property::paintLabel(LCD *lcd) const
{
	assert(lcd != NULL);
	lcd->print(_name);
}

void Property::enterEdit()
{
	onEnterEdit();
	_focusPart = 1;
}

void Property::onEnterEdit()
{
}

void Property::onExitEdit()
{
}

///////////////////////////////////////////////////////////////////////////
// PropertyTime
///////////////////////////////////////////////////////////////////////////

PropertyTime::PropertyTime(const __FlashStringHelper *name, PropertyTime::Time *var)
: Property(name, 2),
	_var(var)
{
	assert(var != NULL);
	if (_var->hour > 23) {
		_var->hour = 23;
	}
	if (_var->mins > 59) {
		_var->hour = 59;
	}
}

void PropertyTime::paintEdit(LCD *lcd) const
{
	assert(lcd != NULL);
	assert(getFocusPart() <= 2);
	uint8_t focusPart = getFocusPart();
	lcd->print(focusPart == 1 ? SEL_LEFT : SPACE );
	pad00Print(lcd, _var->hour);
	switch (focusPart) {
		case 1:
			lcd->print(SEL_RIGHT);
			break;
		case 2:
			lcd->print(SEL_LEFT);
			break;
		default:
			lcd->print(':');
			break;
	}
	pad00Print(lcd, _var->mins);
	lcd->print(focusPart == 2 ? SEL_RIGHT : SPACE);
}

bool PropertyTime::processEditInput(ButtonPress button)
{
	uint8_t focusPart = getFocusPart();
	assert(0 < focusPart && focusPart <= 2);
	if (button == BUTTON_PRESS_DOWN) {
		if (focusPart == 1) {
			wrapDecrease<uint8_t>(&_var->hour, 0, 23);
		} else {
			wrapDecrease<uint8_t>(&_var->mins, 0, 59);
		}
		return true;
	} else if (button == BUTTON_PRESS_UP) {
		if (focusPart == 1) {
			wrapIncrease<uint8_t>(&_var->hour, 0, 23);
		} else {
			wrapIncrease<uint8_t>(&_var->mins, 0, 59);
		}
		return true;
	} else if (button == BUTTON_PRESS_ENTER) {
		nextFocusPart();
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////
// PropertyDate
///////////////////////////////////////////////////////////////////////////

PropertyDate::PropertyDate(const __FlashStringHelper *name, PropertyDate::Date *var)
: Property(name, 3),
	_var(var)
{
	assert(var != NULL);
	clipValue<uint8_t>(&_var->day, 1, 31);
	clipValue<uint8_t>(&_var->month, 1, 12);
	if (_var->year2000 > 99) {
		_var->year2000 = 99;
	}
}

void PropertyDate::onExitEdit()
{
	// TODO: adjust date to a correct value
}

void PropertyDate::paintEdit(LCD *lcd) const
{
	assert(lcd != NULL);
	assert(getFocusPart() <= 3);
	uint8_t focusPart = getFocusPart();
	lcd->print(focusPart == 1 ? SEL_LEFT : SPACE );
	pad00Print(lcd, _var->day);
	switch (focusPart) {
		case 1:
			lcd->print(SEL_RIGHT);
			break;
		case 2:
			lcd->print(SEL_LEFT);
			break;
		default:
			lcd->print('/');
			break;
	}
	pad00Print(lcd, _var->month);
	switch (focusPart) {
		case 2:
			lcd->print(SEL_RIGHT);
			break;
		case 3:
			lcd->print(SEL_LEFT);
			break;
		default:
			lcd->print('/');
			break;
	}
	lcd->print("20");
	pad00Print(lcd, _var->year2000);
	lcd->print(focusPart == 3 ? SEL_RIGHT : SPACE);
}

bool PropertyDate::processEditInput(ButtonPress button)
{
	uint8_t focusPart = getFocusPart();
	assert(0 < focusPart && focusPart <= 3);
	if (button == BUTTON_PRESS_DOWN) {
		if (focusPart == 1) {
			wrapDecrease<uint8_t>(&_var->day, 1, 31);
		} else if (focusPart == 2) {
			wrapDecrease<uint8_t>(&_var->month, 1, 12);
		} else {
			wrapDecrease<uint8_t>(&_var->year2000, 0, 99);
		}
		return true;
	} else if (button == BUTTON_PRESS_UP) {
		if (focusPart == 1) {
			wrapIncrease<uint8_t>(&_var->day, 1, 31);
		} else if (focusPart == 2) {
			wrapIncrease<uint8_t>(&_var->month, 1, 12);
		} else {
			wrapIncrease<uint8_t>(&_var->year2000, 0, 99);
		}
		return true;
	} else if (button == BUTTON_PRESS_ENTER) {
		nextFocusPart();
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////
// PropertyU16
///////////////////////////////////////////////////////////////////////////

struct LimitWidth {
	uint16_t moreThan;
	uint8_t width;
};

const LimitWidth limitWidth[] = {
	{ 9999, 5 },
	{ 999, 4 }, 
	{ 99, 3 },
	{ 9, 2 },
	{ 0, 1 }
};

PropertyU16::PropertyU16(const __FlashStringHelper *name, uint16_t *var, uint16_t limitMin, uint16_t limitMax)
: Property(name, 1),
	_var(var),
	_limitMin(limitMin),
	_limitMax(limitMax)
{
	assert(var != NULL);
	assert(limitMax > limitMin);

	for (const LimitWidth *lw = &limitWidth[0]; ; ++lw) {
		if (lw->moreThan < limitMax) {
			_displayWidth = lw->width;
			break;
		}
	}
	clipValue(_var, _limitMin, _limitMax);
}

void PropertyU16::paintEdit(LCD *lcd) const
{
	assert(lcd != NULL);
	assert(getFocusPart() <= 1);

	uint8_t focusPart = getFocusPart();
	lcd->print(focusPart == 1 ? SEL_LEFT : SPACE);
	padMulti0Print(lcd, *_var, _displayWidth);
	lcd->print(focusPart == 1 ? SEL_RIGHT : SPACE);
}

bool PropertyU16::processEditInput(ButtonPress button)
{
	assert(getFocusPart() == 1);
	if (button == BUTTON_PRESS_DOWN) {
		wrapDecrease(_var, _limitMin, _limitMax);
		return true;
	} else if (button == BUTTON_PRESS_UP) {
		wrapIncrease(_var, _limitMin, _limitMax);
		return true;
	} else if (button == BUTTON_PRESS_ENTER) {
		nextFocusPart();
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////
// PropertyBool
///////////////////////////////////////////////////////////////////////////

PropertyBool::PropertyBool(const __FlashStringHelper *name, bool *var)
: Property(name, 1),
	_var(var)
{
	assert(var != NULL);
}

void PropertyBool::paintEdit(LCD *lcd) const
{
	assert(lcd != NULL);
	assert(getFocusPart() <= 1);

	uint8_t focusPart = getFocusPart();
	lcd->print(focusPart == 1 ? SEL_LEFT : SPACE);
	lcd->print(CHK_LEFT);
	lcd->print(*_var ? CHECKED : UNCHECKED);
	lcd->print(CHK_RIGHT);
	lcd->print(focusPart == 1 ? SEL_RIGHT : SPACE);
}

bool PropertyBool::processEditInput(ButtonPress button)
{
	assert(getFocusPart() == 1);
	if (button == BUTTON_PRESS_DOWN || button == BUTTON_PRESS_UP) {
		*_var = !*_var;
		return true;
	} else if (button == BUTTON_PRESS_ENTER) {
		nextFocusPart();
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////
// PropertyAction
///////////////////////////////////////////////////////////////////////////

PropertyAction::PropertyAction(const __FlashStringHelper *name, Callback callback)
: Property(name, 1),
	_callback(callback),
	_confirm(false)
{
	assert(callback != NULL);
}

void PropertyAction::paintEdit(LCD *lcd) const
{
	assert(lcd != NULL);
	assert(getFocusPart() <= 1);

	uint8_t focusPart = getFocusPart();
	if (focusPart == 1) {
		lcd->print(SEL_LEFT);
		lcd->print(_confirm ? REPLY_YES : REPLY_NO);
		lcd->print(SEL_RIGHT);
	} else {
		lcd->print(SPACE);
		lcd->print(SPACE);
		lcd->print(SPACE);
	}
}

bool PropertyAction::processEditInput(ButtonPress button)
{
	assert(getFocusPart() == 1);
	if (button == BUTTON_PRESS_DOWN || button == BUTTON_PRESS_UP) {
		_confirm = !_confirm;
		return true;
	} else if (button == BUTTON_PRESS_ENTER) {
		if (_confirm) {
			_callback();
		}
		nextFocusPart();
		return true;
	}
	return false;
}

void PropertyAction::onEnterEdit()
{
	_confirm = false;
}

///////////////////////////////////////////////////////////////////////////
// Page
///////////////////////////////////////////////////////////////////////////

Page::Page()
{
}

Page::~Page()
{
}

void Page::reset()
{
}

void Page::paint(Screen *screen) const
{
	assert(screen != NULL);
	screen->getLcd()->clear();
}

uint8_t Page::buttonInput(ButtonPress /*button*/, Screen * /*screen*/)
{
	return INVALID_LINE;
}

///////////////////////////////////////////////////////////////////////////
// ScrollablePage
///////////////////////////////////////////////////////////////////////////

ScrollablePage::ScrollablePage()
: _maxLines(0)
{
	reset();
}

void ScrollablePage::reset()
{
	_topIndex = 0;
	_cursorRow = 0;
}

void ScrollablePage::setMaxLines(uint8_t maxLines)
{
	assert(_maxLines == 0);
	_maxLines = maxLines;
}

void ScrollablePage::paint(Screen *screen) const
{
	assert(screen != NULL);
	LCD *lcd = screen->getLcd();
	lcd->clear();
	for (uint8_t i = 0; i < screen->getRows(); ++i) {
		uint8_t idx = _topIndex + i;
		if (idx == 0) {
			lcd->setCursor(COL_CONTENTS, i);
			lcd->print(PREV_MENU);
		} else {
			uint8_t line = idx - 1;
			if (line < _maxLines) {
				paintLine(line, i, screen);
			}
		}
	}
	paintCursor(screen);
}

uint8_t ScrollablePage::buttonInput(ButtonPress button, Screen *screen)
{
	assert(screen != NULL);
	uint8_t idx = getCurIdx();
	switch (button) {
		case BUTTON_PRESS_ENTER:
			if (idx == 0) {
				return 0;
			} else {
				focusLine(idx - 1);
				paintLine(idx - 1, _cursorRow, screen);
			}
			break;
		case BUTTON_PRESS_DOWN:
			if (idx < _maxLines) {
				if (_cursorRow < screen->getRows() - 1) {
					_cursorRow++;
					paintCursor(screen);
				} else {
					_topIndex++;
					paint(screen);
				}
			}
			break;
		case BUTTON_PRESS_UP:
			if (getCurIdx() > 0) {
				if (_cursorRow == 0) {
					_topIndex--;
					paint(screen);
				} else {
					_cursorRow--;
					paintCursor(screen);
				}
			}
			break;
		default:
			break;
	}
	return INVALID_LINE;
}

void ScrollablePage::paintCursor(Screen *screen) const
{
	assert(screen != NULL);
	LCD *lcd = screen->getLcd();
	for (uint8_t i = 0; i < screen->getRows(); ++i) {
		lcd->setCursor(COL_CURSOR, i);
		lcd->print(_cursorRow == i ? CURSOR : SPACE);
	}
}

void ScrollablePage::focusLine(uint8_t /*line*/)
{
}

///////////////////////////////////////////////////////////////////////////
// PropertyPage
///////////////////////////////////////////////////////////////////////////

PropertyPage::PropertyPage(Property **propertiesAry)
: _propertiesAry(propertiesAry),
	_maxPropNameLen(0)
{
	assert(propertiesAry != NULL);
	assert(propertiesAry[0] != NULL);
	reset();
	size_t maxLen = strlen(PREV_MENU);
	uint8_t i = 0;
	const Property *p = _propertiesAry[0];
	while (p != NULL) {
		size_t l = strlen(reinterpret_cast<const char *>(p->getName()));
		if (l > maxLen) {
			maxLen = l;
		}
		i++;
		p = _propertiesAry[i];
	}
	_maxPropNameLen = static_cast<uint8_t>(maxLen);
	setMaxLines(i);
}

void PropertyPage::reset()
{
	_focusLine = INVALID_LINE;
}

uint8_t PropertyPage::buttonInput(ButtonPress button, Screen *screen)
{
	assert(screen != NULL);
	if (_focusLine == INVALID_LINE) {
		return ScrollablePage::buttonInput(button, screen);
	}
	Property *p = _propertiesAry[_focusLine];
	LCD *lcd = screen->getLcd();
	assert(p->getFocusPart() != 0);
	if (p->processEditInput(button)) {
		screen->getLcd()->setCursor(_maxPropNameLen + 2, getCursorRow());
		p->paintEdit(lcd);
	}
	bool hasFocus = p->getFocusPart() != 0;
	if (!hasFocus) {
		_focusLine = INVALID_LINE;
	}
	return INVALID_LINE;
}


void PropertyPage::paintLine(uint8_t line, uint8_t row, Screen *screen) const
{
	assert(screen != NULL);
	Property *p = _propertiesAry[line];
	if (p != NULL) {
		LCD *lcd = screen->getLcd();
		lcd->setCursor(COL_CONTENTS, row);
		p->paintLabel(lcd);
		lcd->setCursor(_maxPropNameLen + 2, row);
		p->paintEdit(lcd);
	}
}

void PropertyPage::focusLine(uint8_t line)
{
	Property *p = _propertiesAry[line];
	p->enterEdit();
	_focusLine = line;
}


///////////////////////////////////////////////////////////////////////////
// MenuItem
///////////////////////////////////////////////////////////////////////////

MenuItem::MenuItem(const __FlashStringHelper *name, Page *page)
: _name(name),
	_page(page)
{
	assert(name != NULL);
	assert(page != NULL);
}


///////////////////////////////////////////////////////////////////////////
// MenuItemPage
///////////////////////////////////////////////////////////////////////////

MenuItemPage::MenuItemPage(MenuItem *menuItemAry[])
: _menuItemAry(menuItemAry)
{
	assert(menuItemAry != NULL);
	uint8_t i = 0;
	const MenuItem *p = _menuItemAry[0];
	while (p != NULL) {
		i++;
		p = _menuItemAry[i];
	}
	setMaxLines(i);
}

uint8_t MenuItemPage::buttonInput(ButtonPress button, Screen *screen)
{
	if (button == BUTTON_PRESS_ENTER) {
		return getCurIdx();
	}
	return ScrollablePage::buttonInput(button, screen);
}

void MenuItemPage::paintLine(uint8_t line, uint8_t row, Screen *screen) const
{
	assert(screen != NULL);
	MenuItem *p = _menuItemAry[line];
	if (p != NULL) {
		LCD *lcd = screen->getLcd();
		lcd->setCursor(COL_CONTENTS, row);
		lcd->print(p->getName());
	}
}
