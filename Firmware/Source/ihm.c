#include "stm8s.h"
#include "stdbool.h"
#include "ihm.h"

#include "pot.h"
#include "led.h"
#include "shutter.h"

IHMState menuState = IHM_INIT;
volatile uint16_t IHM_timer = 0;

// Default timings in seconds
uint8_t IHM_selfTimerDelay = 10;
uint8_t IHM_timeLapseInterval = 10;
uint8_t IHM_bulbExposureTime = 30;
bool actionDone = false;

IHMInput IHM_input = IHM_NONE;

void IHM_ok(void) {
  IHM_input = IHM_OK;
  led_set_digit(2, 0);
}

void IHM_inc(void) {
  IHM_input = IHM_INC;
  led_set_digit(2, 1);
}

void IHM_dec(void) {
  IHM_input = IHM_DEC;
  led_set_digit(2, 0xd);
}

void IHM_cancel(void) {
  IHM_input = IHM_CANCEL;
  led_set_digit(2, 0xc);
}

void IHM_init(void) {
  menuState = IHM_INIT;
  
  IHM_updateState(IHM_NONE);
  IHM_initTimer();
}

extern PotState potState;
IHMState IHM_updateState() {
  switch(menuState) {
  case IHM_INIT:
    if(potState == P_WAIT)
      menuState = IHM_SELFTIMER;
    break;
    
  case IHM_SELFTIMER:
    if(IHM_input == IHM_OK) // Select
      menuState = IHM_SELFTIMER_S;
    else if(IHM_input == IHM_INC)   // Next item
      menuState = IHM_TIMELAPSE;
    else if(IHM_input == IHM_DEC)   // Previous item
      menuState = IHM_PRGMBULB;
    
    led_set_digit(1, LED_S);
    led_set_digit(2, LED_E);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_F);
    led_set_blink(LED_BLINK_OFF);
    break;
    
  case IHM_SELFTIMER_S:
    if(IHM_input == IHM_OK) { // Activate
      menuState = IHM_SELFTIMER_A;
      IHM_restartTimer();
    }
    else if(IHM_input == IHM_INC) {   // delay++
      if(IHM_selfTimerDelay < (uint8_t)-2)
        IHM_selfTimerDelay++;
    }
    else if(IHM_input == IHM_DEC) {   // delay--
      if(IHM_selfTimerDelay > 1)
        IHM_selfTimerDelay--;
    }
    else if(IHM_input == IHM_CANCEL)// Back
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
    
    if(!actionDone) {
      while((IHM_timer < IHM_selfTimerDelay*TIM2_TICKS_PER_SEC) && (IHM_input != IHM_CANCEL)) pot_updateState();
      
      if(IHM_input != IHM_CANCEL) {
        shutter_open();
        IHM_restartTimer();
        while(IHM_timer < 1) nop();
        shutter_close();
        
        actionDone = true;
      }
    }
    
    if(IHM_input == IHM_CANCEL) {// Back
      menuState = IHM_SELFTIMER_S;
      
      shutter_close();
      
      actionDone = false;
    }
    break;
    
  case IHM_TIMELAPSE:
    if(IHM_input == IHM_OK) // Select
      menuState = IHM_TIMELAPSE_S;
    else if(IHM_input == IHM_INC)   // Next item
      menuState = IHM_BULB;
    else if(IHM_input == IHM_DEC)   // Previous item
      menuState = IHM_SELFTIMER;
    
    led_set_digit(1, LED_L);
    led_set_digit(2, LED_A);
    led_set_digit(3, LED_P);
    led_set_digit(4, LED_S);
    led_set_blink(LED_BLINK_OFF);
    break;
    
  case IHM_TIMELAPSE_S:
    if(IHM_input == IHM_OK) // Activate
      menuState = IHM_TIMELAPSE_A;
    else if(IHM_input == IHM_INC) {   // delay++
      if(IHM_timeLapseInterval < (uint8_t)-2)
        IHM_timeLapseInterval++;
    }
    else if(IHM_input == IHM_DEC) {   // delay--
      if(IHM_timeLapseInterval > 1)
        IHM_timeLapseInterval--;
    }
    else if(IHM_input == IHM_CANCEL)// Back
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
    
    IHM_restartTimer();
    while((IHM_timer < IHM_timeLapseInterval*TIM2_TICKS_PER_SEC) && (IHM_input != IHM_CANCEL)) pot_updateState();
    
    if(IHM_input != IHM_CANCEL) {
      shutter_open();
      IHM_restartTimer();
      while(IHM_timer < 1) nop();
      shutter_close();
    }
    
    if(IHM_input == IHM_CANCEL) {// Back
      menuState = IHM_TIMELAPSE_S;
      
      shutter_close();
    }
    break;
    
  case IHM_BULB:
    if(IHM_input == IHM_OK) // Select
      menuState = IHM_BULB_S;
    else if(IHM_input == IHM_INC)   // Next item
      menuState = IHM_DELDBULB;
    else if(IHM_input == IHM_DEC)   // Previous item
      menuState = IHM_TIMELAPSE;
    
    led_set_digit(1, LED_B);
    led_set_digit(2, LED_U);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_B);
    led_set_blink(LED_BLINK_OFF);
    break;
    
  case IHM_BULB_S:
    if(IHM_input == IHM_OK) // Activate
      menuState = IHM_BULB_A;
    else if(IHM_input == IHM_CANCEL)// Back
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
    
    shutter_open();
    
    while(IHM_input != IHM_CANCEL) pot_updateState();
    
    shutter_close();
    
    menuState = IHM_BULB_S;
    break;
    
  case IHM_DELDBULB:
    if(IHM_input == IHM_OK) // Select
      menuState = IHM_DELDBULB_S;
    else if(IHM_input == IHM_INC)   // Next item
      menuState = IHM_PRGMBULB;
    else if(IHM_input == IHM_DEC)   // Previous item
      menuState = IHM_BULB;
    
    led_set_digit(1, LED_D);
    led_set_digit(2, LED_B);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_B);
    led_set_blink(LED_BLINK_OFF);
    break;
    
  case IHM_DELDBULB_S:
    if(IHM_input == IHM_OK) // Activate
      menuState = IHM_DELDBULB_A;
    else if(IHM_input == IHM_CANCEL)// Back
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
    
    IHM_restartTimer();
    while((IHM_timer < TIM2_TICKS_PER_SEC) && (IHM_input != IHM_CANCEL)) pot_updateState();
    
    if(IHM_input != IHM_CANCEL) {
      shutter_open();
      
      while(IHM_input != IHM_CANCEL) pot_updateState();
    }
    
    shutter_close();
    
    menuState = IHM_DELDBULB_S;
    break;
    
  case IHM_PRGMBULB:
    if(IHM_input == IHM_OK) // Select
      menuState = IHM_PRGMBULB_S;
    else if(IHM_input == IHM_INC)   // Next item
      menuState = IHM_SELFTIMER;
    else if(IHM_input == IHM_DEC)   // Previous item
      menuState = IHM_DELDBULB;
    
    led_set_digit(1, LED_P);
    led_set_digit(2, LED_B);
    led_set_digit(3, LED_L);
    led_set_digit(4, LED_B);
    led_set_blink(LED_BLINK_OFF);
    break;
    
  case IHM_PRGMBULB_S:
    if(IHM_input == IHM_OK) // Activate
      menuState = IHM_PRGMBULB_A;
    else if(IHM_input == IHM_INC) {   // delay++
      if(IHM_bulbExposureTime < (uint8_t)-2)
        IHM_bulbExposureTime++;
    }
    else if(IHM_input == IHM_DEC) {   // delay--
      if(IHM_bulbExposureTime > 1)
        IHM_bulbExposureTime--;
    }
    else if(IHM_input == IHM_CANCEL)// Back
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
    
    shutter_open();
    
    IHM_restartTimer();
    while((IHM_timer < IHM_bulbExposureTime*TIM2_TICKS_PER_SEC) && (IHM_input != IHM_CANCEL)) pot_updateState();
    
    shutter_close();
    
    menuState = IHM_PRGMBULB_S;
    break;
    
  default:
    menuState = IHM_INIT;
    break;
  }
  
  IHM_input = IHM_NONE;
  
  return menuState;
}

// Timer helpers
void IHM_initTimer(void) {
  // Init timer ...
  
  IHM_timer = 0;
}

void IHM_restartTimer(void) {
  IHM_timer = 0;
}
