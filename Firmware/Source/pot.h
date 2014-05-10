#ifndef POT_H
#define POT_H

#define POT_TH_MAX              410     // (4/5th of 512)
#define POT_TH_NEUTRAL          102     // (1/5th of 512)

#define TIM2_TICKS_PER_SEC      4       // 4 overflows per second
#define POT_REPEAT_DELAY        2       // 1/2s (in TIM2 tick/sec)

typedef enum {P_INIT = (uint8_t)0, P_WAIT, 
              P_IN_INC, P_INC, P_OK,
              P_IN_DEC, P_DEC, P_CANCEL} PotState;

void pot_init(void);

PotState pot_updateState(void);

int16_t pot_readSigned(void);

// Timer helpers
void pot_initTimer(void);
void pot_restartTimer(void);      // pot_timer = 0
void TIM2_CC1_IRQ(void);

// ADC helpers
void pot_initADC(void);
void ADC1_IRQ(void);

#endif
