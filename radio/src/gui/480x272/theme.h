/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _THEME_H_
#define _THEME_H_

#include <list>
#include "zone.h"

class BitmapBuffer;

#define MAX_THEME_OPTIONS              5

class Theme;
void registerTheme(Theme * theme);

class Theme
{
  public:
    struct PersistentData {
      ZoneOptionValue options[MAX_THEME_OPTIONS];
    };

    Theme(const char * name, const ZoneOption * options=NULL):
      name(name),
      options(options),
      thumb(NULL)
    {
      registerTheme(this);
    }

    inline const char * getName() const
    {
      return name;
    }

    const char * getFilePath(const char * filename) const;

    void drawThumb(uint16_t x, uint16_t y, uint32_t flags);

    inline const ZoneOption * getOptions() const
    {
      return options;
    }

    void init() const;

    virtual void update(bool withBitmaps = true) const
    {
    }

    ZoneOptionValue * getOptionValue(unsigned int index) const;

    virtual void load() const;

    virtual void drawBackground() const;

    virtual const BitmapBuffer * getIconBitmap(uint8_t index, bool selected) const { return nullptr; }

    virtual const BitmapBuffer * getIconMask(uint8_t index) const { return nullptr; }

    virtual bool drawTopbarBackground(uint32_t options=0) const = 0;

    virtual void drawIcon(coord_t x, coord_t y, uint8_t index, LcdFlags flags) const { }

    virtual bool drawMenuIcon(uint8_t index, uint8_t position, bool selected) const { return false; }

    virtual void drawMessageBox(const char * title, const char * text, const char * action, uint32_t flags) const;

  public:
    const char * name;
    const ZoneOption * options;
    BitmapBuffer * thumb;
    static const BitmapBuffer * asterisk;
    static const BitmapBuffer * question;
    static const BitmapBuffer * busy;
};

extern Theme * theme;

inline const char * getThemePath(const char * filename)
{
  return theme->getFilePath(filename);
}

Theme * getTheme(const char * name);
void loadTheme(Theme * theme);
void loadTheme();

std::list<Theme *> & getRegisteredThemes();

#endif // _THEME_H_
