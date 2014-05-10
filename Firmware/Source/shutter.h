#ifndef SHUTTER_H
#define SHUTTER_H

#define SHUTTER_FOCUS_DELAY 1

void shutter_init(void);
void shutter_open(void);
void shutter_close(void);

// Timer helpers
void shutter_initTimer(void);
void shutter_restartTimer(void);        // shutter_timer = 0

#endif
