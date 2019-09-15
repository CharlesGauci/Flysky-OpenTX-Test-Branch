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

#ifndef _MODEL_LOGICAL_SWITCHES_H
#define _MODEL_LOGICAL_SWITCHES_H

#include "tabsgroup.h"

class LogicalSwitchButton : public Button {
  public:
    LogicalSwitchButton(Window * parent, const rect_t & rect, int lsIndex, std::function<uint8_t(void)> onPress);
    bool isActive();
    void checkEvents() override;
    virtual void paint(BitmapBuffer * dc) override;
    void updateHeight();
    void setLsIndex(uint8_t index);
    void setDefined(bool defined);
  protected:
    uint8_t lsIndex;
    bool active;
    bool defined;
};

class ModelLogicalSwitchesPage: public PageTab {
public:
    ModelLogicalSwitchesPage();

    virtual void build(Window * window) override {
      build(window, -1);
    }

protected:
    void build(Window * window, int8_t focusIndex);
    void rebuild(Window * window, int8_t focusIndex);
    void editLogicalSwitch(Window * window, uint8_t lsIndex);
};

#endif //_MODEL_LOGICAL_SWITCHES_H
