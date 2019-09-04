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
#include "opentx.h"

#define  __BATTERY_DRIVER_C__

#define BATTERY_W 140
#define BATTERY_H 320
#define BATTERY_TOP ((LCD_H - BATTERY_H)/2)
#define BATTERY_CONNECTOR_W 32
#define BATTERY_CONNECTOR_H 10
#define BATTERY_BORDER 4
#define BATTERY_W_INNER (BATTERY_W - 2*BATTERY_BORDER)
#define BATTERY_H_INNER (BATTERY_H - 2*BATTERY_BORDER)
#define BATTERY_TOP_INNER (BATTERY_TOP + BATTERY_BORDER)

void battery_charge_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = PWR_CHARGE_FINISHED_GPIO_PIN | PWR_CHARGING_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(PWR_CHARGING_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(PWR_CHARGING_GPIO, PWR_CHARGE_FINISHED_GPIO_PIN | PWR_CHARGING_GPIO_PIN);
}

#define CHARGE_SAMPLES 10

uint16_t get_battery_charge_state()
{
  static uint16_t chargeSamples[CHARGE_SAMPLES] = {0};
  static uint16_t chargeSampleIndex = 0;
  bool lastChargeState = CHARGE_UNKNOWN;
  uint16_t chargeState = CHARGE_UNKNOWN;
#if !defined(SIMU)
  bool isFinished = !READ_CHARGE_FINISHED_STATE();
  bool isCharging = !READ_CHARGING_STATE();
  int maxSamples = boardState == BOARD_POWER_OFF ? CHARGE_SAMPLES/2 : CHARGE_SAMPLES;
  maxSamples = CHARGE_SAMPLES;
  if(chargeSampleIndex >= maxSamples) chargeSampleIndex = 0;
  uint16_t currentChargeState = isFinished ? CHARGE_FINISHED : isCharging ? CHARGE_STARTED : CHARGE_NONE;
  chargeSamples[chargeSampleIndex++] = currentChargeState;
  //TRACE("CHARGE sample %d value %d", chargeSampleIndex -1, currentChargeState);
#endif
  for(int index = 0; index < maxSamples; index++) {
    if(chargeState == CHARGE_UNKNOWN) {
      //prevent entering to charged status from non charging state
      if(chargeSamples[index] == CHARGE_FINISHED && lastChargeState != CHARGE_STARTED) continue;
      chargeState = chargeSamples[index];
    }
    else if(chargeState != chargeSamples[index]) {
      //the only way to detect none state is searching for inconsistent state
      //when battery is not charged finished pin is triggered periodically only
      chargeState = CHARGE_NONE;
    }
  }
  if(chargeState != CHARGE_UNKNOWN) lastChargeState = chargeState;
  return chargeState;
}

void drawChargingInfo(uint16_t chargeState){
	static int progress = 0;
	const char* text = chargeState == CHARGE_STARTED ? STR_BATTERYCHARGING : STR_BATTERYFULL;
    int h = 0;
    LcdFlags color = 0;
    if(CHARGE_STARTED == chargeState)
    {
        if(progress >= 100)
        {
            progress = 0;
        }
        else
        {
            progress+=25;
        }
        text = STR_BATTERYCHARGING;
        h = ((BATTERY_H_INNER * progress) / 100);
        color = ROUND|BATTERY_CHARGE_COLOR;
    }
    else if(CHARGE_FINISHED == chargeState)
    {
        text = STR_BATTERYFULL;
        h = BATTERY_H_INNER;
        color = ROUND|BATTERY_CHARGE_COLOR;
    }
    else
    {
        text = STR_BATTERYNONE;
        h = BATTERY_H_INNER;
        color = ROUND|TEXT_COLOR;
    }

    BACKLIGHT_ENABLE();
    lcd->drawSizedText(LCD_W/2, LCD_H-50, text, strlen(text), CENTERED|TEXT_BGCOLOR);

    lcd->drawFilledRect((LCD_W - BATTERY_W)/2, BATTERY_TOP, BATTERY_W, BATTERY_H, SOLID, ROUND|TEXT_BGCOLOR);
    lcd->drawFilledRect((LCD_W - BATTERY_W_INNER)/2, BATTERY_TOP_INNER, BATTERY_W_INNER, BATTERY_H_INNER, SOLID, ROUND|TEXT_COLOR);

    lcd->drawFilledRect((LCD_W - BATTERY_W_INNER)/2, BATTERY_TOP_INNER + BATTERY_H_INNER - h , BATTERY_W_INNER, h, SOLID, color);
    lcd->drawFilledRect((LCD_W - BATTERY_CONNECTOR_W)/2, BATTERY_TOP-BATTERY_CONNECTOR_H , BATTERY_CONNECTOR_W, BATTERY_CONNECTOR_H, SOLID, TEXT_BGCOLOR);
}
#define CHARGE_INFO_DURATION 500
//this method should be called by timer interrupt or by GPIO interrupt
void handle_battery_charge(uint32_t last_press_time)
{
#if !defined(SIMU)
  static uint16_t updateTime = 0;
  static uint16_t lastState = CHARGE_UNKNOWN;
  static uint32_t info_until = 0;
  static bool lcdInited = false;

  if(boardState != BOARD_POWER_OFF) return;
  uint16_t now = get_tmr10ms();
  uint16_t chargeState = get_battery_charge_state();
  if(chargeState != CHARGE_UNKNOWN) {

    if(lastState != chargeState) {
      //avoid positive check when none and unknown
      if(lastState + chargeState > 1) {
        //charge state changed - last state known
        info_until = now + (CHARGE_INFO_DURATION);
      }
    }
    //power buttons pressed
    else if(now - last_press_time < POWER_ON_DELAY) {
      info_until = now + CHARGE_INFO_DURATION;
    }
    lastState = chargeState;
  }

  if(now > info_until) {
    info_until = 0;
    lcd->clear();
    BACKLIGHT_DISABLE();
    if(lcdInited) {
      lcdOff();
    }
    return;
  }

  if(updateTime == 0 || ((get_tmr10ms() - updateTime) >= 50))
  {
      if(!lcdInited) {
        backlightInit();
        lcdInit();
        lcdInited = true;
      }
      else {
        lcdOn();
      }
      updateTime = get_tmr10ms();     
      lcdNextLayer();
      lcd->clear();
      drawChargingInfo(chargeState);
      lcdRefresh();
   }
#endif
}

uint16_t getBatteryVoltage()
{
  int32_t instant_vbat = anaIn(TX_VOLTAGE);  // using filtered ADC value on purpose
  return (uint16_t)((instant_vbat * (1000 + g_eeGeneral.txVoltageCalibration)) / 2942);
}
