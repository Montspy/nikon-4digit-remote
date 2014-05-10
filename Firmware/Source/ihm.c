#include "stm8s.h"
#include "ihm.h"

#include "pot.h"
#include "led.h"

IHMState menuState = IHM_INIT;

// Default timings in seconds
uint8_t IHM_selfTimerDelay = 10;
uint8_t IHM_timeLapseInterval = 10;
uint8_t IHM_bulbExposureTime = 30;

void IHM_ok(void) {
  IHM_updateState(IHM_OK);
}

void IHM_inc(void) {
  IHM_updateState(IHM_INC);
}

void IHM_dec(void) {
  IHM_updateState(IHM_DEC);
}

void IHM_cancel(void) {
  IHM_updateState(IHM_CANCEL);
}

void IHM_init(void) {
  menuState = IHM_INIT;
  
  IHM_updateState(IHM_NONE);
}

extern PotState potState;
IHMState IHM_updateState(IHMInput input) {
  switch(menuState) {
  case IHM_INIT:
    //if(potState == P_WAIT)
      menuState = IHM_SELFTIMER;
    break;
    
  case IHM_SELFTIMER:
    if(input == IHM_OK) // Select
      menuState = IHM_SELFTIMER_S;
    else if(input == IHM_INC)   // Next item
      menuState = IHM_TIMELAPSE;
    else if(input == IHM_DEC)   // Previous item
      menuState = IHM_PRGMBULB;
    
    led_set_digit(1, LED_S);
    led_set_digit(2, LED_E);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_F);
    led_set_blink(LED_BLINK_OFF);
    break;
    
  case IHM_SELFTIMER_S:
    if(input == IHM_OK) // Activate
      menuState = IHM_SELFTIMER_A;
    else if(input == IHM_INC)   // delay++
      if(IHM_selfTimerDelay < (uint8_t)-2) IHM_selfTimerDelay++;
    else if(input == IHM_DEC)   // delay--
      if(IHM_selfTimerDelay > 1) IHM_selfTimerDelay--;
    else if(input == IHM_CANCEL)// Back
      menuState = IHM_SELFTIMER;
    
    led_set_digit(1, LED_S);
    led_set_number(IHM_selfTimerDelay);
    led_set_blink(LED_BLINK_1HZ);
    break;
    
  case IHM_SELFTIMER_A:
    // Start timer
    // End of timer => open shutter, wait 1s, close shutter
    // CANCEL => stop timer, close shutter
    
    led_set_digit(1, LED_S);
    led_set_number(IHM_selfTimerDelay);
    led_set_blink(LED_BLINK_2HZ);
    break;
    
  case IHM_TIMELAPSE:
    if(input == IHM_OK) // Select
      menuState = IHM_TIMELAPSE_S;
    else if(input == IHM_INC)   // Next item
      menuState = IHM_BULB;
    else if(input == IHM_DEC)   // Previous item
      menuState = IHM_SELFTIMER;
    
    led_set_digit(1, LED_L);
    led_set_digit(2, LED_A);
    led_set_digit(3, LED_P);
    led_set_digit(4, LED_S);
    led_set_blink(LED_BLINK_OFF);
    break;
    
  case IHM_TIMELAPSE_S:
    if(input == IHM_OK) // Activate
      menuState = IHM_TIMELAPSE_A;
    else if(input == IHM_INC)   // delay++
      if(IHM_timeLapseInterval < (uint8_t)-2) IHM_timeLapseInterval++;
    else if(input == IHM_DEC)   // delay--
      if(IHM_timeLapseInterval > 1) IHM_timeLapseInterval--;
    else if(input == IHM_CANCEL)// Back
      menuState = IHM_TIMELAPSE;
    
    led_set_digit(1, LED_L);
    led_set_number(IHM_timeLapseInterval);
    led_set_blink(LED_BLINK_1HZ);
    break;
    
  case IHM_TIMELAPSE_A:
    // Start timer
    // End of timer => open shutter, wait 1s, close shutter, restart timer
    // CANCEL => stop timer, close shutter
    
    led_set_digit(1, LED_L);
    led_set_number(IHM_timeLapseInterval);
    led_set_blink(LED_BLINK_2HZ);
    break;
    
  case IHM_BULB:
    if(input == IHM_OK) // Select
      menuState = IHM_BULB_S;
    else if(input == IHM_INC)   // Next item
      menuState = IHM_DELDBULB;
    else if(input == IHM_DEC)   // Previous item
      menuState = IHM_TIMELAPSE;
    
    led_set_digit(1, LED_B);
    led_set_digit(2, LED_U);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_B);
    led_set_blink(LED_BLINK_OFF);
    break;
    
  case IHM_BULB_S:
    if(input == IHM_OK) // Activate
      menuState = IHM_BULB_A;
    else if(input == IHM_CANCEL)// Back
      menuState = IHM_BULB;
    
    led_set_digit(1, LED_B);
    led_set_digit(2, LED_U);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_B);
    led_set_blink(LED_BLINK_1HZ);
    break;
    
  case IHM_BULB_A:
    // Open shutter
    // CANCEL => Close shutter
    
    led_set_digit(1, LED_B);
    led_set_digit(2, LED_U);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_B);
    led_set_blink(LED_BLINK_2HZ);
    break;
    
  case IHM_DELDBULB:
    if(input == IHM_OK) // Select
      menuState = IHM_DELDBULB_S;
    else if(input == IHM_INC)   // Next item
      menuState = IHM_PRGMBULB;
    else if(input == IHM_DEC)   // Previous item
      menuState = IHM_BULB;
    
    led_set_digit(1, LED_D);
    led_set_digit(2, LED_B);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_B);
    led_set_blink(LED_BLINK_OFF);
    break;
    
  case IHM_DELDBULB_S:
    if(input == IHM_OK) // Activate
      menuState = IHM_DELDBULB_A;
    else if(input == IHM_CANCEL)// Back
      menuState = IHM_DELDBULB;
    
    led_set_digit(1, LED_D);
    led_set_digit(2, LED_B);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_B);
    led_set_blink(LED_BLINK_1HZ);
    break;
    
  case IHM_DELDBULB_A:
    // Wait 1s, Open shutter
    // CANCEL => Close shutter
    
    led_set_digit(1, LED_D);
    led_set_digit(2, LED_B);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_B);
    led_set_blink(LED_BLINK_2HZ);
    break;
    
  case IHM_PRGMBULB:
    if(input == IHM_OK) // Select
      menuState = IHM_PRGMBULB_S;
    else if(input == IHM_INC)   // Next item
      menuState = IHM_SELFTIMER;
    else if(input == IHM_DEC)   // Previous item
      menuState = IHM_DELDBULB;
    
    led_set_digit(1, LED_P);
    led_set_digit(2, LED_B);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_B);
    led_set_blink(LED_BLINK_OFF);
    break;
    
  case IHM_PRGMBULB_S:
    if(input == IHM_OK) // Activate
      menuState = IHM_PRGMBULB_A;
    else if(input == IHM_INC)   // delay++
      if(IHM_bulbExposureTime < (uint8_t)-2) IHM_bulbExposureTime++;
    else if(input == IHM_DEC)   // delay--
      if(IHM_bulbExposureTime > 1) IHM_bulbExposureTime--;
    else if(input == IHM_CANCEL)// Back
      menuState = IHM_PRGMBULB;
    
    led_set_digit(1, LED_P);
    led_set_number(IHM_bulbExposureTime);
    led_set_blink(LED_BLINK_1HZ);
    break;
    
  case IHM_PRGMBULB_A:
    // Start timer, open shutter
    // End of timer => close shutter
    // CANCEL => stop timer, close shutter
    
    led_set_digit(1, LED_P);
    led_set_number(IHM_bulbExposureTime);
    led_set_blink(LED_BLINK_2HZ);
    break;
    
  default:
    menuState = IHM_INIT;
    break;
  }
  
  return menuState;
}
