
// wiring to CNCv3 board
// GP 4 and 5 are I2C pins, in use.
// GP 16,17,18,19 are SPI pins, saved.


// On proto board have 12v from CNCv3 on Vin to 5 volt regulator to PiPico Vsys.
// PiPico 3.3 out to 3.3 power and to the I2C RTC and OLED screen.  SDA and SCL are wired as appropriate.
// connect 3.3v to 5v on proto board to use 3.3 VDD on CNCv3
//  !!! revisit wiring if use TMC2130, power up issue will need isolation of 3.3 power
//  !!! in this case 3.3 to CNCv3 5v pin should come from 5 volt regulator only via diode drops
//  !!! and Vsys should be fed with a diode from 5 volt regulator to isolate from Vbus power.

//   Pi Pico GP Pin           Wire to arduino Proto Board Pin for CNCv3 connection

#define RA_STEP 6            // D2
#define RA_DIR  7            // D5
#define DEC_STEP 8           // D3
#define DEC_DIR  9           // D6
#define FOCUS_STEP 2         // D4  wire if used
#define FOCUS_DIR  3         // D7  wire if used

#define DRV_ENABLE 10        // D8
