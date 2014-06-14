#ifndef IHM_H
#define IHM_H

#define LED_A   0xa
#define LED_B   0xb
#define LED_D   0xd
#define LED_E   0xe
#define LED_F   0xf
#define LED_L   16
#define LED_P   17
#define LED_S   0x5
#define LED_T   18
#define LED_U   19

typedef enum {IHM_INIT, IHM_SELFTIMER, IHM_TIMELAPSE, IHM_BULB, IHM_DELDBULB, IHM_PRGMBULB,     // Choices (INC and DEC is browsing the modes, OK selects the mode)
              IHM_SELFTIMER_S, IHM_TIMELAPSE_S, IHM_BULB_S, IHM_DELDBULB_S, IHM_PRGMBULB_S,     // Selected modes, blinking digits (INC and DEC modifies the delay if available, 
                                                                                                // OK activates the mode, CANCEL goes back to the choices)
              IHM_SELFTIMER_A, IHM_TIMELAPSE_A, IHM_BULB_A, IHM_DELDBULB_A, IHM_PRGMBULB_A,     // Active modes, blinking letter (CANCEL deactivates the mode, goes back to the selected mode)
} IHMState;

typedef enum {IHM_OK, IHM_INC, IHM_DEC, IHM_CANCEL, IHM_NONE} IHMInput;

void IHM_ok(void);
void IHM_inc(void);
void IHM_dec(void);
void IHM_cancel(void);

void IHM_init(void);

IHMState IHM_updateState();

// Timer helpers
void IHM_initTimer(void);
void IHM_restartTimer(void);        // IHM_timer = 0

#endif
