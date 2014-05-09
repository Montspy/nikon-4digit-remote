#include "stm8s.h"
#include "pot.h"

#include "ihm.h"

PotState potState = P_INIT;
volatile unsigned int pot_timer;

void pot_init(void) {
  // Init ADC
  
  // Init Pot timer
  pot_initTimer();
  
  potState = P_INIT;
}

PotState pot_updateState(void) {
  int position = pot_readSigned();
  
  switch(potState) {
  case P_INIT:
    if((position > -POT_TH_NEUTRAL) && (position < +POT_TH_NEUTRAL))
      potState = P_WAIT;
    break;
    
  case P_WAIT:
    if(position >= +POT_TH_NEUTRAL) {
      potState = P_IN_INC;
      
      pot_restartTimer();
    }
    else if(position <= -POT_TH_NEUTRAL) {
      potState = P_IN_DEC;
      
      pot_restartTimer();
    }
    break;
    
  case P_IN_INC:
    if(position < +POT_TH_NEUTRAL) {    // Back to neutral
      potState =  P_WAIT;
      
      IHM_inc();
    }
    else if(pot_timer >= POT_REPEAT_DELAY) {     // Repeat delay elapsed
      potState = P_INC;
      
      pot_restartTimer();
      
      IHM_inc();
    }
    else if(position >= +POT_TH_MAX) {  // Not an INC, but an OK
      potState = P_OK;
      
      IHM_ok();
    }
    break;
    
  case P_INC:
    if(position < +POT_TH_NEUTRAL) {    // Back to neutral
      potState =  P_WAIT;
    }
    else if(pot_timer >= POT_REPEAT_DELAY) {    // Repeat delay elapsed
      potState = P_INC;
      pot_restartTimer();
      
      IHM_inc();
    }
    break;
    
  case P_OK:
    if(position < +POT_TH_NEUTRAL)
      potState = P_WAIT;
    break;
    
  case P_IN_DEC:
    if(position > -POT_TH_NEUTRAL) {    // Back to neutral
      potState =  P_WAIT;
      
      IHM_dec();
    }
    else if(pot_timer >= POT_REPEAT_DELAY) {     // Repeat delay elapsed
      potState = P_INC;
      
      pot_restartTimer();
      
      IHM_dec();
    }
    else if(position <= -POT_TH_MAX) {  // Not a DEC, but a CANCEL
      potState = P_CANCEL;
      
      IHM_cancel();
    }
    break;
    
  case P_DEC:
    if(position > -POT_TH_NEUTRAL) {    // Back to neutral
      potState =  P_WAIT;
    }
    else if(pot_timer >= POT_REPEAT_DELAY) {    // Repeat delay elapsed
      potState = P_DEC;
      pot_restartTimer();
      
      IHM_dec();
    }
    break;
    
  case P_CANCEL:
    if(position > -POT_TH_NEUTRAL)
      potState = P_WAIT;
    break;
    
  default:
    potState = P_INIT;
    break;
  }
  
  return potState;
}

int pot_readSigned(void) {
  return 0;
}

void pot_initTimer(void) {
  
}

void pot_restartTimer(void) {
  pot_timer = 0;
  
}
