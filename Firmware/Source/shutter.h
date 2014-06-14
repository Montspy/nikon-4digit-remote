#ifndef SHUTTER_H
#define SHUTTER_H

#define SHUTTER_FOCUS_DELAY 1

/* Wiring the remote cable
 * 2 small NPN transitors or 2 small NMOS
 * In my cable: White is GND, Red is Shutter, Yellow is Focus
 * 
 * If you are using NPNs: Solder the two emitters to the GND (white)
 *                        Solder one collector to the shutter (red) and the second collector to the focus (yellow)
 *                        Solder two 10k-ish resistors, on for the base of each NPN
 *                        Solder the second lead of the resistors together to the spare pin of the voltmeter (PD3).
 * 
 * If you are using NMOS: Solder the two sources to the GND (white)
 *                        Solder one drain to the shutter (red) and the second drain to the focus (yellow)
 *                        Solder the two gates together to the spare pin of the voltmeter (PD3).
 * 
 */

void shutter_init(void);
void shutter_open(void);
void shutter_close(void);

#endif
