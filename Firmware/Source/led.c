/*
# Copyright (c) 2013, Thomas S. Price
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "stm8s.h"
#include "led.h"
#include "ihm.h"

#define DEFAULT_LED_DUTY 10


static char _display_buf[4] = {0};

static uint8_t _duty;

static led_blink_type _blink;

static void _led_write_segments(char ch)
{
  /* Character Map, segments active-low
*
*        0 1 2 3 4 5 6 7 8 9 A B C D E F L P T U
* A: PB5 0 1 0 0 1 0 0 0 0 0 0 1 0 1 0 0 1 0 1 1
* F: PA1 0 1 1 1 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 0
* B: PB4 0 0 0 0 0 1 1 0 0 0 0 1 1 0 1 1 1 0 1 0
* G: PC3 1 1 0 0 0 0 0 1 0 0 0 0 1 0 0 0 1 0 0 1
* C: PC4 0 0 1 0 0 0 0 0 0 0 0 0 1 0 1 1 1 1 1 0
* E: PC7 0 1 0 1 1 1 0 1 0 1 0 0 0 0 0 0 0 0 0 0
* D: PC6 0 1 0 0 1 0 0 1 0 0 1 0 0 0 0 1 0 1 0 0
*/
  switch (ch & 0x7f)
  {
  case 0: // 0
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3));
    break;
  case 1: // 1
    GPIO_WriteHigh(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7));
    break;
  case 2: // 2
    GPIO_WriteHigh(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    break;
  case 3: // 3
    GPIO_WriteHigh(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_7));
    break;
  case 4: // 4
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_6 | GPIO_PIN_7));
    break;
  case 5: // 5
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_5));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_7));
    break;
  case 6: // 6
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_5));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7));
    break;
  case 7: // 7
    GPIO_WriteHigh(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7));
    break;
  case 8: // 8
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7));
    break;
  case 9: // 9
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_7));
    break;
  case 10: // A
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_7));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_6));
    break;
  case 11: // B
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7));
    break;
  case 12: // C
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_5));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_6 | GPIO_PIN_7));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4));
    break;
  case 13: // D
    GPIO_WriteHigh(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7));
    break;
  case 14: // E
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_5));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    break;
  case 15: // F
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_5));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_7));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_6));
    break;
  case LED_L: // L
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_6 | GPIO_PIN_7));
    break;
  case LED_P: // P
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_7));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_6));
    break;
  case LED_T: // T
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7));
    break;
  case LED_U: // U
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_5));
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3));
    break;
  default: // Blank
    GPIO_WriteHigh(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1));
    GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5));
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7));
  }
  if (ch & 0x80) // DP
  {
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_5));
  }
  else
  {
    GPIO_WriteHigh(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_5));
  }
}


static void _led_write_com(char digit)
{
  // LED_COM_4: PD1
  // LED_COM_3: PD4
  // LED_COM_2: PD6
  // LED_COM_1: PA3
  switch (digit)
  {
  case 1:
    GPIO_WriteHigh(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_3);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_6);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_4);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_1);
    break;
  case 2:
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_3);
    GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_6);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_4);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_1);
    break;
  case 3:
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_3);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_6);
    GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_4);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_1);
    break;
  case 4:
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_3);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_6);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_4);
    GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_1);
    break;
  case 0:
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_3);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_6);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_4);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_1);
    break;
  }
}


// Display character at position 1-4.
// If digit = 0, turn off all digits
void _led_display_char(uint8_t digit)
{
  char ch;
  if (digit == 0 || digit > 4)
  {
    _led_write_segments(0x7f);
    _led_write_com(0);
    return;
  }
  ch = _display_buf[digit - 1];
  _led_write_com(0);
  _led_write_segments(ch);
  _led_write_com(digit);
}


void led_init(void)
{
  // GPIO Initialize
  // From left->right, Digit 1, 2, 3, 4 (_display_buf[0], _display_buf[1], _display_buf[2], _display_buf[3])
  // LED_COM_1: PA3
  // LED_COM_2: PD6
  // LED_COM_3: PD4
  // LED_COM_4: PD1
  // LED_SEG_A: PB5
  // LED_SEG_B: PB4
  // LED_SEG_C: PC4
  // LED_SEG_D: PC6
  // LED_SEG_E: PC7
  // LED_SEG_F: PA1
  // LED_SEG_G: PC3
  // LED_SEG_DP: PC5
  GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)(GPIO_PIN_1 | GPIO_PIN_3), GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5), GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_7), GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)(GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_6), GPIO_MODE_OUT_PP_HIGH_FAST);
  // Clear buffer
  _display_buf[0] = 0x0b;
  _display_buf[1] = 0x0e;
  _display_buf[2] = 0x0e;
  _display_buf[3] = 0x0f;
  // Default values
  _duty = DEFAULT_LED_DUTY;
  _blink = LED_BLINK_OFF;
  // Disable update
  led_off();
  /*
// Test
led_set_digit( 1, 0x0 );
led_set_digit( 2, 0x1 );
led_set_digit( 3, 0x2 );
led_set_digit( 4, 0x3 );
led_on();
*/
}


void led_on(void)
{
   TIM1_DeInit();
  
  /* TIM1 configuration:
* TIM1 in PWM mode without output disabled. We only interest in the interrupts.
* Period of PWM is 1024us; Duty cycle adjustable from 1/16 - 15/16.
* With 16Mhz clock, we use:
* TIM1_Period = 16 - 1
* TIM1_Prescaler = 1024 - 1
* TIM1_Pulse = [0..15]
*/
  TIM1_TimeBaseInit(1023, TIM1_COUNTERMODE_UP, 15, 0);
  /*
TIM1_OCMode = TIM1_OCMODE_PWM1
TIM1_OutputState = TIM1_OUTPUTSTATE_DISABLED
TIM1_OutputNState = TIM1_OUTPUTNSTATE_DISABLED
TIM1_Pulse = [1..16], change using TIM1_SetCompare1
TIM1_OCPolarity = TIM1_OCPOLARITY_LOW; Doesn't matter as output is disabled
TIM1_OCNPolarity = TIM1_OCNPOLARITY_HIGH; Doesn't matter as output is disabled
TIM1_OCIdleState = TIM1_OCIDLESTATE_SET; Doesn't matter as output is disabled
TIM1_OCNIdleState = TIM1_OCIDLESTATE_RESET; Doesn't matter as output is disabled
*/
  TIM1_OC1Init
  (
    TIM1_OCMODE_PWM1,
    TIM1_OUTPUTSTATE_DISABLE, TIM1_OUTPUTNSTATE_DISABLE,
    _duty,
    TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH,
    TIM1_OCIDLESTATE_SET, TIM1_OCNIDLESTATE_RESET
  );
  TIM1_ITConfig((TIM1_IT_TypeDef)(TIM1_IT_UPDATE | TIM1_IT_CC1), ENABLE);
  /* TIM1 Main Output disabled */
  TIM1_CtrlPWMOutputs(DISABLE);
  /* TIM1 counter enable */
  TIM1_Cmd(ENABLE);
}


void led_off(void)
{
  TIM1_ITConfig((TIM1_IT_TypeDef)(TIM1_IT_UPDATE | TIM1_IT_CC1), DISABLE);
  TIM1_ClearITPendingBit((TIM1_IT_TypeDef)(TIM1_IT_UPDATE | TIM1_IT_CC1));
  TIM1_Cmd(DISABLE);
  // Display Off
  _led_write_segments(0x7f);
  _led_write_com(0);
}


void led_set_duty(uint8_t duty)
{
  if (duty > 15) duty = 15;
  TIM1_SetCompare1((uint16_t)duty);
}

void led_set_blink(led_blink_type blink)
{
  _blink = blink;
}


void led_set_digit(uint8_t digit, uint8_t value)
{
  if (digit == 0 || digit > 4)
  {
    return;
  }
  _display_buf[digit - 1] = value;
}


// Called from TIM1 UEV ISR
void LED_TIM1_UPDATE_ISR(void)
{
  // turn on next digit
  static uint8_t digit = 1;
  static uint8_t turn_on = 1;
  static uint16_t blink_count = 0;
  
  // This function will be called every 1024us, for the fastest blink rate of 2Hz,
  // LED should toggle every 244 cycles
  ++blink_count;
  switch (_blink)
  {
  case LED_BLINK_OFF:
    turn_on = 1;
    blink_count = 0;
    break;
  case LED_BLINK_2HZ:
    if (blink_count > 244)
    {
      turn_on = !turn_on;
      blink_count = 0;
    }
    break;
  case LED_BLINK_1HZ:
    if (blink_count > 488)
    {
      turn_on = !turn_on;
      blink_count = 0;
    }
    break;
  case LED_BLINK_HALFHZ:
    if (blink_count > 977)
    {
      turn_on = !turn_on;
      blink_count = 0;
    }
    break;
  }
  if (turn_on)
    _led_display_char(digit);
  digit++;
  if (digit > 4)
    digit = 1;
}

void led_set_number(uint16_t value) {
  if(value != (uint16_t)-1) {
    led_set_digit(2, (value/100)%10);
    led_set_digit(3, (value/10)%10);
    led_set_digit(4, (value)%10);
  }
  else {        // Blank
    led_set_digit(2, -1);
    led_set_digit(3, -1);
    led_set_digit(4, -1);
  }
}


// Called from ISR
void LED_TIM1_CC1_ISR(void)
{
  // turn off
  _led_display_char(0);
}