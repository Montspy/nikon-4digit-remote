#include "stm8s.h"
#include "shutter.h"

void shutter_init(void) {
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)(GPIO_PIN_3), GPIO_MODE_OUT_PP_HIGH_FAST);  // PD3 output
  shutter_close();
}

void shutter_open(void) {
    GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3);  // PD3 low
}

void shutter_close(void) {
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3);  // PD3 high
}
