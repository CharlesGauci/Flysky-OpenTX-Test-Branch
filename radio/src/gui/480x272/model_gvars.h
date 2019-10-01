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
#ifndef _MODEL_GVARS_H
#define _MODEL_GVARS_H

#include "tabsgroup.h"
#include "page.h"
#include "window.h"
#include "numberedit.h"

class GVarButton : public Button {
  public:
    GVarButton(Window * parent, const rect_t & rect, uint8_t gvar);
    void checkEvents() override;
    void paint(BitmapBuffer * dc) override;
  protected:
    void drawFlightMode(BitmapBuffer * dc, coord_t x, coord_t y, int fm, LcdFlags attr);
    uint8_t gvar;
    int lines;
    int32_t gvarSum; //used for invalidation
    uint8_t currentFlightMode; //used for invalidation
};


class ModelGVarsPage : public PageTab {
  public:
    ModelGVarsPage() : PageTab(STR_GLOBAL_VARS, ICON_MODEL_GVARS) {}
    void build(Window * window);
    void rebuild(Window * window);
};

class GVarRenderer : public Window {
  public:
  GVarRenderer(Window* window, rect_t rect, uint8_t gvarIndex) :
    Window(window, rect),
    index(gvarIndex)
  {

  }
  void paint(BitmapBuffer * dc) override;
  void checkEvents() override;
  protected:
    uint8_t index;
    gvar_t lastGVar;
    uint8_t lastFlightMode;
};


class GVarEditWindow : public Page {
  public:
    GVarEditWindow(uint8_t gvarIndex) :
      Page(),
      index(gvarIndex)
  {
    values = new NumberEdit*[MAX_FLIGHT_MODES];
    buildBody(&body);
    buildHeader(&header);
  }
  ~GVarEditWindow() {
    delete[] values;
  }
  protected:
    uint8_t index;
    void buildHeader(Window * window);
    void buildBody(Window * window);
    void setProperties(int onlyForFlightMode = -1);
    NumberEdit* min;
    NumberEdit* max;
    NumberEdit** values;
    GVarRenderer * gVarInHeader;

    //StaticText* varValueHeader;
};
#endif
