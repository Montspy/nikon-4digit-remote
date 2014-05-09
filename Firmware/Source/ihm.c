#include "stm8s.h"
#include "ihm.h"

#include "pot.h"

IHMState menuState = IHM_INIT;

// Default timings in seconds
unsigned int IHM_selfTimerDelay = 10;
unsigned int IHM_timeLapseInterval = 10;        /* !!! A value of 0 is actually 1/2s !!! */
unsigned int IHM_bulbExposureTime = 30;

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
    if(potState == P_WAIT)
      menuState = IHM_SELFTIMER;
    break;
    
  case IHM_SELFTIMER:
    if(input == IHM_OK) // Select
      menuState = IHM_SELFTIMER_S;
    else if(input == IHM_INC)   // Next item
      menuState = IHM_TIMELAPSE;
    else if(input == IHM_DEC)   // Previous item
      menuState = IHM_PRGMBULB;
    break;
    
  case IHM_SELFTIMER_S:
    if(input == IHM_OK) // Activate
      menuState = IHM_SELFTIMER_A;
    else if(input == IHM_INC)   // delay++
      if(IHM_selfTimerDelay < (unsigned int)-1) IHM_selfTimerDelay++;
    else if(input == IHM_DEC)   // delay--
      if(IHM_selfTimerDelay > 1) IHM_selfTimerDelay--;
    else if(input == IHM_CANCEL)// Back
      menuState = IHM_SELFTIMER;
    break;
    
  case IHM_SELFTIMER_A:
    // Start timer
    // CANCEL => stop timer, close shutter
    break;
    
  case IHM_TIMELAPSE:
    if(input == IHM_OK) // Select
      menuState = IHM_TIMELAPSE_S;
    else if(input == IHM_INC)   // Next item
      menuState = IHM_BULB;
    else if(input == IHM_DEC)   // Previous item
      menuState = IHM_SELFTIMER;
    break;
    
  case IHM_TIMELAPSE_S:
    if(input == IHM_OK) // Activate
      menuState = IHM_TIMELAPSE_A;
    else if(input == IHM_INC)   // delay++
      if(IHM_timeLapseInterval < (unsigned int)-1) IHM_timeLapseInterval++;
    else if(input == IHM_DEC)   // delay--
      if(IHM_timeLapseInterval > 0) IHM_timeLapseInterval--;
    else if(input == IHM_CANCEL)// Back
      menuState = IHM_TIMELAPSE;
    break;
    
  case IHM_TIMELAPSE_A:
    break;
    
  case IHM_BULB:
    if(input == IHM_OK) // Select
      menuState = IHM_BULB_S;
    else if(input == IHM_INC)   // Next item
      menuState = IHM_DELDBULB;
    else if(input == IHM_DEC)   // Previous item
      menuState = IHM_TIMELAPSE;
    break;
    
  case IHM_BULB_S:
    if(input == IHM_OK) // Activate
      menuState = IHM_BULB_A;
    else if(input == IHM_CANCEL)// Back
      menuState = IHM_BULB;
    break;
    
  case IHM_BULB_A:
    break;
    
  case IHM_DELDBULB:
    if(input == IHM_OK) // Select
      menuState = IHM_DELDBULB_S;
    else if(input == IHM_INC)   // Next item
      menuState = IHM_PRGMBULB;
    else if(input == IHM_DEC)   // Previous item
      menuState = IHM_BULB;
    break;
    
  case IHM_DELDBULB_S:
    if(input == IHM_OK) // Activate
      menuState = IHM_DELDBULB_A;
    else if(input == IHM_CANCEL)// Back
      menuState = IHM_DELDBULB;
    break;
    
  case IHM_DELDBULB_A:
    break;
    
  case IHM_PRGMBULB:
    if(input == IHM_OK) // Select
      menuState = IHM_PRGMBULB_S;
    else if(input == IHM_INC)   // Next item
      menuState = IHM_SELFTIMER;
    else if(input == IHM_DEC)   // Previous item
      menuState = IHM_DELDBULB;
    break;
    
  case IHM_PRGMBULB_S:
    if(input == IHM_OK) // Activate
      menuState = IHM_PRGMBULB_A;
    else if(input == IHM_INC)   // delay++
      if(IHM_bulbExposureTime < (unsigned int)-1) IHM_bulbExposureTime++;
    else if(input == IHM_DEC)   // delay--
      if(IHM_bulbExposureTime > 1) IHM_bulbExposureTime--;
    else if(input == IHM_CANCEL)// Back
      menuState = IHM_PRGMBULB;
    break;
    
  case IHM_PRGMBULB_A:
    break;
    
  default:
    menuState = IHM_INIT;
    break;
    
  }
  
  return menuState;
}
