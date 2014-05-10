#include "stm8s.h"
#include "shutter.h"

volatile uint8_t shutter_timer = 0;

void shutter_init(void) {
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)(GPIO_PIN_3), GPIO_MODE_OUT_PP_HIGH_FAST);  // PD3 output
  shutter_close();
  shutter_initTimer();
}

void shutter_open(void) {
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3);  // PD3 low
}

void shutter_close(void) {
    GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3);  // PD3 high
}

// Timer helpers

void shutter_initTimer(void) {
  // Init timer ...
  
  shutter_timer = 0;
}

void shutter_restartTimer(void) {
  shutter_timer = 0;
}
