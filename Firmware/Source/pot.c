#include "stm8s.h"
#include "pot.h"

#include "ihm.h"
#include "led.h"

#include "stdbool.h"

PotState potState = P_INIT;
volatile uint8_t pot_timer = 0;
volatile uint16_t pot_rawADC = 0;
volatile bool pot_convDone = false;

void pot_init(void) {
  // Init ADC
  pot_initADC();
  
  // Init Pot timer
  pot_initTimer();
  
  potState = P_INIT;
  pot_timer = 0;
}

PotState pot_updateState(void) {
  int16_t position = pot_readSigned();
  
  switch(potState) {
  case P_INIT:
    if((position > -POT_TH_NEUTRAL) && (position < +POT_TH_NEUTRAL))
      potState = P_WAIT;
    break;
    
  case P_WAIT:
    if(position >= +(POT_TH_NEUTRAL+50)) {
      potState = P_IN_INC;
      
      pot_restartTimer();
    }
    else if(position <= -(POT_TH_NEUTRAL+50)) {
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
    else if(position >= +(POT_TH_MAX+50)) {  // Not an INC, but an OK
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
    if(position < +POT_TH_NEUTRAL)      // Back to neutral
      potState = P_WAIT;
    break;
    
  case P_IN_DEC:
    if(position > -POT_TH_NEUTRAL) {    // Back to neutral
      potState =  P_WAIT;
      
      IHM_dec();
    }
    else if(pot_timer >= POT_REPEAT_DELAY) {     // Repeat delay elapsed
      potState = P_DEC;
      
      pot_restartTimer();
      
      IHM_dec();
    }
    else if(position <= -(POT_TH_MAX+50)) {  // Not a DEC, but a CANCEL
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
    if(position > -POT_TH_NEUTRAL)      // Back to neutral
      potState = P_WAIT;
    break;
    
  default:
    potState = P_INIT;
    break;
  }
  
  return potState;
}

int16_t pot_readSigned(void) {
  ADC1_StartConversion();
  
  while(!pot_convDone) nop();
  pot_convDone = false;
  
  return pot_rawADC - 512;
}

// Timer helpers
void pot_initTimer(void) {
  TIM2_DeInit();
  
  TIM2_TimeBaseInit(TIM2_PRESCALER_1024, 3906);
  TIM2_PrescalerConfig(TIM2_PRESCALER_1024, TIM2_PSCRELOADMODE_IMMEDIATE);
  
  /* Output Compare Active Mode configuration: Channel1 */
  /*
    TIM2_OCMode = TIM2_OCMODE_INACTIVE
    TIM2_Pulse = 3906   ==> 1 tick every 0.25s
    TIM2_OCPolarity = TIM2_OCPOLARITY_HIGH
  */
  TIM2_OC1Init(TIM2_OCMODE_INACTIVE, TIM2_OUTPUTSTATE_DISABLE, 3906, TIM2_OCPOLARITY_HIGH);
  TIM2_OC1PreloadConfig(DISABLE);
  
  TIM2_ARRPreloadConfig(ENABLE);
  
  /* TIM2 IT enable */
  TIM2_ITConfig(TIM2_IT_CC1, ENABLE);
  TIM2_ITConfig(TIM2_IT_CC2, DISABLE);
  TIM2_ITConfig(TIM2_IT_CC3, DISABLE);
  
  TIM2_Cmd(ENABLE);
  
  pot_timer = 0;
}

void pot_restartTimer(void) {
  pot_timer = 0;
}

// ADC helpers
void pot_initADC(void) {
  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);
  
  /* De-Init ADC peripheral*/
  ADC1_DeInit();

  /* Init ADC2 peripheral */
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_3, ADC1_PRESSEL_FCPU_D2, \
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL3,\
            DISABLE);

  /* Enable EOC interrupt */
  ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);
  
  /*Start Conversion */
  ADC1_StartConversion();
}

void ADC1_IRQ(void) {
  pot_rawADC = ADC1_GetConversionValue();
  
  pot_convDone = true;
}
