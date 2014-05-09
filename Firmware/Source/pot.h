#ifndef POT_H
#define POT_H

#define POT_TH_MAX        410     // (4/5th of 512)
#define POT_TH_NEUTRAL    102     // (1/5th of 512)

#define POT_REPEAT_DELAY  16      // 16 timer periods

typedef enum {P_INIT, P_WAIT, 
              P_IN_INC, P_INC, P_OK,
              P_IN_DEC, P_DEC, P_CANCEL} PotState;

void pot_init(void);

PotState pot_updateState(void);

int pot_readSigned(void);

// Timer helpers
void pot_initTimer(void);
void pot_restartTimer(void);      // pot_timer = 0, Tenable = 1

#endif
