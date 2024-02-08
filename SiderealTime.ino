//  Pi Pico
//  display local sidereal on an OLED display.  Uses a DS3231 clock which is good for one century 2000 - 2099

// ? each second add 1.0027379 to sidereal time, or ( total delta seconds / 365.2425) * 366.2425
// ? does doing the division first keep significant figures 

// ? timers - 500us for 1k stepper step( clk low,clk high ), 1/100 second for RA rate calc, 1 sec to keep time.
//   for 1/10 and 1 sec could set flags and service in loop

// from OAT  #define SIDEREAL_SECONDS_PER_DAY 86164.0905f

#include <Arduino.h>
#include <time.h>
#include <Wire.h>
#include <OLED1306_Basic.h>
#include <DS3231.h>
#include <RPi_Pico_TimerInterrupt.h>
#include <AccelStepper.h>

#define ROW0 0
#define ROW1 8
#define ROW2  16
#define ROW3  24
#define ROW4  32
#define ROW5  40
#define ROW6  48
#define ROW7  56

  OLED1306 LCD;
  DS3231 RTC;
  RPI_PICO_Timer ITimer0(0);
  AccelStepper RAstep(AccelStepper::DRIVER,6,7);
  AccelStepper DECstep(AccelStepper::DRIVER,8,9);

extern unsigned char SmallFont[];
extern unsigned char MediumNumbers[];
extern unsigned char BigNumbers[];


#define SFACTOR0 0.0027379           // sidereal  fast by this amount
#define SFACTOR1 1.0027379

DateTime GMT_eq;
DateTime today;

float my_longitude = -69.524125;
float my_latitude  = 44.44688;
uint8_t sid_hr, sid_mn, sid_sec;
//uint8_t GMTversion;
//float ssec;
//volatile long ra_steps;
volatile uint8_t finding;      // goto started
int new_target, old_target;


byte onetime = 1;

struct BSTAR {
  char con[4];
  uint8_t hr;
  uint8_t mn;
  int8_t  dd;
  uint8_t dm;
  char sname[20];
};

// bright stars
#define NUMSTAR 254
const struct BSTAR bstar[NUMSTAR] = {
{"And", 00, 39, + 30, 51,"delta And * "},
{"Psc", 00, 39, + 21, 26,"55 Psc ** "},
{"And", 00, 42, + 40, 52,"M32 Ell Gal "},
{"And", 00, 42, + 41, 16,"M31 Andromeda S"},
{"Scl", 00, 47, - 25, 17,"NGC 253 Spl Gal"},
{"Cas", 00, 49, + 57, 49,"eta Cas ** "},
{"And", 01, 9, + 35, 37,"beta And * "},
{"Cas", 01, 33, + 60, 42,"M 103 Open Cl "},
{"Tri", 01, 33, + 30, 39,"M 33 Pinwheel S"},
{"Psc", 01, 36, + 15, 47,"M 74 Spl Gal "},
{"Per", 01, 41, + 51, 34,"M 76 Barbell Pl"},
{"Ari", 01, 53, + 19, 17,"gamma Ari ** "},
{"Psc", 02, 02, + 02, 45,"alpha Psc ** "},
{"And", 02, 03, + 42, 19,"gamma And * ** "},
{"Tri", 02, 12, + 30, 18,"6 Tri ** "},
{"Cet", 02, 12, -2, 23,"66 Cet * * "},
{"Per", 02, 19, + 57, 8,"NGC 869 h Ope"},
{"Per", 02, 22, + 57, 06,"NGC 884 chi O"},
{"Cas", 02, 29, + 67, 24,"iota Cas *** "},
{"Per", 02, 42, + 42, 46,"M 34 Open Cl "},
{"Cet", 02, 42, 0, 01,"M 77 Spl Gal "},
{"Cet", 02, 43, + 03, 14,"gamma Cet ** "},
{"Per", 02, 50, + 55, 53,"eta Per * * "},
{"Eri", 02, 58, - 40, 18,"theta Eri ** "},
{"Cep", 03, 06, + 79, 25,"Struve 320 ** "},
{"Per", 03, 24, + 49, 51,"alpha Per * "},
{"Tau", 03, 47, + 24, 06,"M 45 Pleiades O"},
{"Eri", 03, 54, -2, 57,"32 Eri ** "},
{"Cam", 03, 57, + 61, 06,"Struve 67 ** "},
{"Eri", 04, 14, - 12, 44,"NGC 1535 Plt Ne"},
{"Tau", 04, 35, + 16, 30,"Aldebaran * "},
{"Aur", 04, 57, + 33, 10,"iota Aur * "},
{"Ori", 05, 14, -8, 12,"Rigel ** "},
{"Aur", 05, 16, + 49, 59,"Capella * "},
{"Lep", 05, 24, - 24, 31,"M 79 Glob Cl "},
{"Ori", 05, 24, -2, 23,"eta Ori ** "},
{"Aur", 05, 28, + 35, 50,"M 38 Open Cl "},
{"Tau", 05, 34, + 22, 01,"Crab Nebula Spr"},
{"Ori", 05, 35, + 9, 56,"lambda Ori ** "},
{"Ori", 05, 35, -5, 23,"Trapezium **** "},
{"Ori", 05, 35, -5, 23,"M 42 Orion Neb."},
{"Ori", 05, 35, -5, 54,"iota Ori ** "},
{"Ori", 05, 35, -5, 16,"M 43 NE Ori Neb"},
{"Aur", 05, 36, + 34, 8,"M 36 Open Cl "},
{"Ori", 05, 38, -2, 36,"sigma Ori *** *"},
{"Ori", 05, 40, -1, 56,"zeta Ori ** * "},
{"Ori", 05, 46, + 00, 03,"M 78 Dif Neb "},
{"Aur", 05, 52, + 32, 32,"M37 Open Cl "},
{"Ori", 05, 55, + 07, 24,"Betelgeuse * "},
{"Aur", 05, 59, + 44, 56,"beta Aur * "},
{"Aur", 05, 59, + 37, 12,"theta Aur ** "},
{"Gem", 06, 8, + 24, 20,"M 35 Open Cl "},
{"Mon", 06, 23, + 04, 35,"epsilon Mon * *"},
{"Mon", 06, 28, -7, 02,"beta Mon *** "},
{"Aur", 06, 36, + 38, 26,"UU Aur * (red) "},
{"CMa", 06, 45, - 16, 43,"Sirius ** "},
{"Lyn", 06, 46, + 59, 26,"12 Lyn *** "},
{"CMa", 06, 47, - 20, 45,"M41 Open Cl "},
{"Gem", 06, 52, + 33, 57,"theta Gem * "},
{"CMa", 06, 56, - 14, 02,"mu CMa ** "},
{"Mon", 07, 02, -8, 20,"M 50 Open Cl "},
{"Gem", 07, 20, + 21, 59,"delta Gem ** "},
{"Gem", 07, 29, + 20, 54,"NGC2392 Eskimo "},
{"Gem", 07, 34, + 31, 53,"Castor ** * "},
{"Pup", 07, 36, - 14, 28,"M 47 Open Cl "},
{"Pup", 07, 38, - 26, 48,"kappa Pup ** "},
{"CMi", 07, 39, + 05, 13,"Procyon * "},
{"Pup", 07, 41, - 14, 49,"M 46 Open Cl "},
{"Pup", 07, 44, - 23, 52,"M 93 Open Cl "},
{"Gem", 07, 45, + 28, 01,"Pollux * "},
{"Cnc", 8, 12, + 17, 38,"zeta Cnc *** "},
{"Hya", 8, 13, -5, 47,"M 48 Open Cl "},
{"Cnc", 8, 40, + 19, 59,"M 44 Praesepe O"},
{"Cnc", 8, 46, + 28, 45,"iota Cnc * * "},
{"Cnc", 8, 51, + 11, 48,"M 67 Open Cl "},
{"UMa", 8, 59, + 48, 02,"iota UMa ** "},
{"Lyn", 9, 21, + 34, 23,"alpha Lyn * "},
{"UMa", 9, 55, + 69, 03,"M 81 Spl Gal "},
{"UMa", 9, 55, + 69, 40,"M 82 Irr Gal "},
{"Leo", 10, 19, + 19, 50,"gamma Leo ** "},
{"UMa", 10, 22, + 41, 30,"mu UMa * "},
{"Hya", 10, 24, - 18, 38,"NGC 3242 Plt Ne"},
{"Leo", 10, 43, + 11, 42,"M 95 BSp Gal "},
{"Leo", 10, 46, + 11, 49,"M 96 Spl Gal "},
{"Leo", 10, 47, + 12, 35,"M 105 Ell Gal "},
{"UMa", 11, 9, + 44, 29,"psi UMa * "},
{"UMa", 11, 11, + 55, 40,"M 108 Spl Gal "},
{"UMa", 11, 14, + 55, 01,"M 97 Owl Neb Pl"},
{"UMa", 11, 18, + 31, 31,"xi UMa ** "},
{"Leo", 11, 18, + 13, 05,"M 65 Spl Gal "},
{"Leo", 11, 20, + 12, 59,"M 66 Spl Gal "},
{"UMa", 11, 57, + 53, 22,"M109 BSp Gal "},
{"Com", 12, 13, + 14, 54,"M 98 Spl Gal "},
{"Com", 12, 18, + 14, 25,"M 99 Spl Gal "},
{"CVn", 12, 18, + 47, 18,"M106 Spl Gal "},
{"Vir", 12, 21, + 04, 28,"M 61 Spl Gal "},
{"UMa", 12, 22, + 58, 05,"M 40 ** "},
{"Com", 12, 22, + 15, 49,"M 100 Spl Gal "},
{"Vir", 12, 25, + 12, 53,"M 84 Ell Gal "},
 {"Com", 12, 25, + 18, 11,"M 85 Ell Gal "},
{"Vir", 12, 26, + 12, 56,"M 86 Ell Gal "},
{"Vir", 12, 29, + 02, 03,"3C273 Quasar "},
{"Vir", 12, 29, + 8, 00,"M 49 Ell Gal "},
{"Crv", 12, 29, - 16, 30,"delta Crv * * "},
{"Vir", 12, 30, + 12, 23,"M 87 Ell Gal "},
{"Com", 12, 32, + 14, 25,"M 88 Spl Gal "},
{"Com", 12, 35, + 18, 22,"24 Com * * "},
{"Com", 12, 35, + 14, 29,"M 91 BSp Gal "},
{"Vir", 12, 35, + 12, 33,"M 89 Ell Gal "},
{"Vir", 12, 36, + 13, 9,"M 90 Spl Gal "},
{"Vir", 12, 37, + 11, 49,"M 58 Spl Gal "},
{"Hya", 12, 39, - 26, 45,"M 68 Glob Cl "},
{"Vir", 12, 39, - 11, 37,"M104 Sombrero S"},
{"Vir", 12, 41, -1, 27,"gamma Vir ** "},
{"Vir", 12, 42, + 11, 38,"M 59 Ell Gal "},
{"Vir", 12, 43, + 11, 33,"M 60 Ell Gal "},
{"CVn", 12, 45, + 45, 26,"Y Cvn * (red) "},
{"Cam", 12, 49, + 83, 24,"Struve 1694 * *"},
{"CVn", 12, 50, + 41, 07,"M 94 Spl Gal "},
{"CVn", 12, 56, + 38, 19,"Cor Caroli * * "},
{"Com", 12, 56, + 21, 40,"M 64 Black Eye "},
{"Com", 13, 12, + 18, 10,"M 53 Glob Cl "},
{"CVn", 13, 15, + 42, 02,"M 63 Sunflower "},
{"UMa", 13, 23, + 54, 55,"Mizar (w/Alcor)"},
{"Vir", 13, 25, - 11, 9,"Spica * "},
{"CVn", 13, 29, + 47, 11,"M 51 Whirlpool "},
{"Hya", 13, 37, - 29, 51,"M 83 Spl Gal "},
//{"CVn", 13, 42, + 28, 22,"M 3 Glob Cl "},
{"CVn", 13, 43, 28, 22, "M 3 Glob C1 "},
//{"UMa", 13, 47, + 49, 18,"eta UMa * "},
{"UMa", 13, 47, 49, 18,"eta Uma * "},
{"UMa", 14, 03, + 54, 21,"M 101 Spl Gal "},
{"UMa", 14, 03, + 54, 21,"M 102 = M 101 S"},
{"Boo", 14, 15, + 19, 11,"Arcturus * "},
{"Boo", 14, 32, + 38, 18,"gamma Boo ** "},
{"Boo", 14, 44, + 27, 04,"epsilon Boo ** "},
{"Boo", 14, 51, + 19, 06,"xi Boo ** "},
{"Boo", 15, 15, + 33, 18,"delta Boo * * "},
//{"Ser", 15, 18, + 02, 05,"M 5 Glob Cl "},
{"Ser", 15, 18, 2 , 5, "M 5 Glob C1 "},
{"Boo", 15, 24, + 37, 21,"mu Boo * ** "},
{"Ser", 15, 34, + 10, 32,"delta Ser ** "},
{"CrB", 15, 39, + 36, 38,"zeta CrB ** "},
{"Sco", 16, 04, - 11, 22,"xi Sco *** ** "},
{"Sco", 16, 05, - 19, 48,"beta Sco * * "},
{"Sco", 16, 11, - 19, 27,"nu Sco ** ** "},
{"Sco", 16, 17, - 22, 59,"M 80 Glob Cl "},
{"Her", 16, 19, + 46, 18,"tau Her * "},
{"Sco", 16, 23, - 26, 30,"M 4 Glob Cl "},
{"Sco", 16, 29, - 26, 25,"Antares ** "},
{"Oph", 16, 32, - 13, 02,"M 107 Glob Cl "},
{"Dra", 16, 36, + 52, 54,"16  17 Dra * **"},
{"Her", 16, 41, + 36, 27,"M 13 Hercules G"},
{"Her", 16, 44, + 23, 47,"NGC 6210 Plt Ne"},
{"Oph", 16, 47, -1, 57,"M 12 Glob Cl "},
{"Oph", 16, 57, -4, 06,"M 10 Glob Cl "},
{"Oph", 17, 01, - 30, 07,"M 62 Glob Cl "},
{"Oph", 17, 02, - 26, 15,"M 19 Glob Cl "},
{"Her", 17, 14, + 14, 23,"alpha Her ** "},
{"Her", 17, 15, + 24, 50,"delta Her ** "},
{"Her", 17, 15, + 36, 48,"pi Her * "},
{"Her", 17, 17, + 43, 8,"M 92 Glob Cl "},
{"Oph", 17, 18, - 24, 17,"omicron Oph ** "},
{"Oph", 17, 19, - 18, 31,"M 9 Glob Cl "},
{"Her", 17, 23, + 37, 8,"rho Her ** "},
{"Dra", 17, 32, + 55, 10,"nu Dra * * "},
{"Oph", 17, 37, -3, 16,"M 14 Glob Cl "},
{"Sco", 17, 40, - 32, 12,"M 6 Butterfly O"},
{"Dra", 17, 41, + 72, 9,"psi Dra * * "},
{"Her", 17, 53, + 40, 00,"90 Her ** "},
{"Sco", 17, 53, - 34, 48,"M 7 Open Cl "},
{"Sgr", 17, 56, - 19, 01,"M 23 Open Cl "},
{"Dra", 17, 58, + 66, 38,"NGC 6543 Plt Ne"},
{"Her", 18, 01, + 21, 35,"95 Her ** "},
{"Sgr", 18, 02, - 23, 02,"M 20 Triffid Di"},
{"Sgr", 18, 03, - 24, 23,"M 8 Lagoon Neb "},
{"Sgr", 18, 04, - 22, 29,"M 21 Open Cl "},
{"Oph", 18, 05, + 02, 30,"70 Oph ** "},
{"Oph", 18, 12, + 06, 50,"NGC 6572 Plt Ne"},
{"Sgr", 18, 18, - 18, 24,"M 24 Open Cl "},
{"Ser", 18, 18, - 13, 46,"M 16 Eagle Neb."},
{"Sgr", 18, 19, - 17, 07,"M 18 Open Cl "},
{"Sgr", 18, 20, - 16, 10,"M 17 Omega Dif "},
{"Sgr", 18, 24, - 24, 52,"M 28 Glob Cl "},
{"Sgr", 18, 31, - 32, 20,"M 69 Glob Cl "},
{"Sgr", 18, 31, - 19, 14,"M 25 Open Cl "},
{"Sgr", 18, 36, - 23, 55,"M 22 Glob Cl "},
{"Lyr", 18, 36, + 38, 47,"503Vega * * "},
{"Sgr", 18, 43, - 32, 18,"M 70 Glob Cl "},
{"Lyr", 18, 44, + 39, 38,"epsilon Lyr ** "},
{"Sct", 18, 45, -9, 23,"M 26 Open Cl "},
{"Sct", 18, 51, -6, 16,"M 11 Open Cl "},
{"Lyr", 18, 53, + 33, 01,"M 57 Ring Neb P"},
{"Sgr", 18, 55, - 30, 28,"M 54 Glob Cl "},
{"Ser", 18, 56, + 04, 12,"theta Ser * * "},
{"Lyr", 19, 16, + 30, 10,"M 56 Glob Cl "},
{"Cyg", 19, 30, + 27, 57,"Albireo * * "},
{"Sgr", 19, 40, - 30, 56,"M 55 Glob Cl "},
{"Sgr", 19, 43, - 14, 8,"NGC 6818 Plt Ne"},
{"Cyg", 19, 44, + 50, 31,"NGC 6826 Plt Ne"},
{"Cyg", 19, 44, + 45, 07,"delta Cyg ** "},
{"Aql", 19, 50, + 8, 52,"Altair * "},
{"Sge", 19, 53, + 18, 46,"M 71 Glob Cl "},
{"Vul", 19, 59, + 22, 43,"M 27 Plt Neb "},
{"Sgr", 20, 06, - 21, 55,"M 75 Glob Cl "},
{"Cyg", 20, 23, + 38, 31,"M 29 Open Cl "},
{"Cyg", 20, 41, + 32, 18,"49 Cyg ** "},
{"Cyg", 20, 41, + 45, 16,"Deneb * "},
{"Del", 20, 46, + 16, 07,"gamma Del ** "},
{"Aqr", 20, 53, - 12, 32,"M 72 Glob Cl "},
{"Aqr", 20, 58, - 12, 37,"M 73 **** "},
{"Aqr", 21, 04, - 11, 22,"NGC7009 Saturn "},
{"Cyg", 21, 06, + 38, 44,"61 Cyg * * "},
{"Ctg", 21, 14, + 38, 02,"tau Cyg ** "},
{"Cep", 21, 28, + 70, 33,"beta Cep * * "},
{"Peg", 21, 30, + 12, 10,"M 15 Glob Cl "},
{"Cyg", 21, 32, + 48, 26,"M 39 Open Cl "},
{"Aqr", 21, 33, 0, 49,"M2 Glob Cl "},
{"Cap", 21, 40, - 23, 11,"M30 Glob Cl "},
{"Cep", 21, 43, + 58, 46,"mu Cep * (red)"},
{"Aqr", 22, 28, 0, 01,"zeta Aqr ** "},
{"Cep", 22, 29, + 58, 24,"delta Cep * * "},
{"Peg", 22, 43, + 30, 13,"eta Peg * * "},
{"Aqr", 23, 19, - 13, 27,"94 Aqr * * "},
{"Cas", 23, 24, + 61, 35,"M 52 Open Cl "},
{"And", 23, 25, + 42, 32,"NGC 7662 Plt Ne"},
{"And", 23, 37, + 46, 27,"lambda And * "},
{"Psc", 23, 46, + 03, 29,"19 (TX) Psc * ("},
};



void setup() {

  delay(1000);
  i2init();
  Serial.begin(9600);

  LCD.InitLCD();
  LCD.setFont(SmallFont);
  LCD.clrScr();
  LCD.print((char *)"Hello",LEFT,ROW0);
  delay(1000);
  LCD.clrRow(0);  LCD.clrRow(1);
    
  pinMode( 25, OUTPUT );

  RAstep.setAcceleration( 10.0 );
  RAstep.setMinPulseWidth( 3 );
  RAstep.setEnablePin( 3 );
  RAstep.setPinsInverted( 0,0,1);     // enable low
  DECstep.setAcceleration( 10.0 );
  DECstep.setMinPulseWidth( 3 );

  DECstep.enableOutputs();
  RAstep.enableOutputs();
  
  RAstep.setMaxSpeed( 999.0 );
  DECstep.setMaxSpeed( 999.0 );
  //RAstep.setSpeed(  3.42 );     // fake sidereal rate enabled  test


  // moveTo( position relative to zero starting position ) for goto's
  // move( relative to the current position );
  // setCurrentPosition( for fixups )
  RAstep.moveTo( -654 );
  DECstep.moveTo( 456 );
  finding = 1;                         // flag goto in progress, need to reset sidereal speed when done

  ITimer0.attachInterruptInterval(1000, TimerHandler0);   // time in us, 1000 == 1ms

}


// keep the steppers running
bool TimerHandler0(struct repeating_timer *t){
  (void) t;
  static uint8_t toggle;

  DECstep.run();

  // a goto clears the constant speed setting, need to set it again after a goto
  // distanceToGo uses the old target, can't call it to see if need to call run or
  // runSpeed.  If run is called when not goto'ing it will try to move to the old target.
  // So need a flag ( finding ) to control what function to call.
  if( finding ){
     RAstep.run();
     //DECstep.run();
     if( RAstep.distanceToGo() == 0 /*&& DECstep.distanceToGo() == 0*/ ) RAstep.setSpeed(  0.71 ), finding = 0;
  } 
  else if( RAstep.runSpeed() ){
    //++ra_steps;
    digitalWrite( 25, toggle );
    toggle ^= 1;
  }

  return true;
}

void loop() {

   get_GMT_base();
   delay(5000);
   if( today.second() % 5 ) delay(100);

   if( Serial.available() > 1 ){      // fine clock adjustment
      char c = Serial.read();
      if( c == '+' || c == '-' ){
        
         char d = Serial.read();
         d = d - '0';

         char sec = RTC.getSecond();
         if( c == '+' ){
           RTC.setSecond( sec + d );
         }
         if( c == '-' ){
           RTC.setSecond( sec - d );
         }
      }
     // if( c == 'v' ) GMTversion ^= 1;    // use 2024 or 2030 as base of the sidereal calc
   }

   noInterrupts();
   long ra = RAstep.currentPosition();
   long dec = DECstep.currentPosition();
   interrupts();
   Serial.write('d'); Serial.print( dec );  Serial.write(' ');
   Serial.write('r'); Serial.write('a'); Serial.print( ra ); Serial.write(' ');

   // fake goto the current target
   if( new_target != old_target ){
      old_target = new_target;

      // abs macro bugs.....?
      long stp = (long)bstar[new_target].dd;
      if( stp < 0 ) stp = -stp;
      stp *= 60L;
      stp += (long)bstar[new_target].dm;
      // long stp = abs( (long)bstar[new_target].dd ) * 60 + (long)bstar[new_target].dm;
      if( bstar[new_target].dd < 0 ) stp = -stp;
      
      noInterrupts();
      RAstep.moveTo( 0 );        // just crossed the meridian
      DECstep.moveTo( stp );
      finding = 1;               // important to set the moving flag
      interrupts();
      Serial.println( stp );
   }
   
}


// start from date 2035/01/01 and known gmt sidereal time, find difference to todays date,
// scale diff to sidereal seconds length, add/sub offset for longitude,
// convert to a data/time and save the hours, minutes, seconds
void get_GMT_base(){
int64_t t1,t2,diff;
uint8_t h,m,s;

   GMT_eq = DateTime( 2035, 1, 1, 0, 0, 0 );
       
   // get the today info from the RTC which is using UTC time
   today = RTClib::now();
   h = today.hour();   m = today.minute();  s = today.second();
   if( h < 10 ) Serial.write('0');
   Serial.print( h ); Serial.write(':');
   if( m < 10 ) Serial.write('0');
   Serial.print( m ); Serial.write(':');
   if( s < 10 ) Serial.write('0');
   Serial.print( s ); Serial.print("  ");


   t1 = today.unixtime();   t2 = GMT_eq.unixtime();     // use 64 bit signed 
   diff =  t1 - t2;   
   
   diff += (float)diff * SFACTOR0;
   Serial.print("Base 2035/01/01 ");
   Serial.print( diff );   Serial.write(' ');   

   GMT_eq = DateTime( 2035, 1, 1, 6, 41, 56 );  // gmt sidereal time 2035/1/1
   t2 = GMT_eq.unixtime();
    
   t2 += diff;        // add sidereal adjustment

  // longitude adjustment
   t2 += my_longitude * 240.0;     // 86400/360;
   
   DateTime new_time = DateTime( (uint32_t)t2 );
   sid_hr = new_time.hour();   sid_mn = new_time.minute();  sid_sec = new_time.second();

   Serial.print( new_time.year() );   Serial.write('/');
   Serial.print( new_time.month() );  Serial.write('/');
   Serial.print( new_time.day() );    Serial.write(' ');

   LCD.setFont(MediumNumbers);
   if( sid_hr < 10 ) Serial.write('0');
   Serial.print( sid_hr );  Serial.write(':');
   LCD.printNumI(sid_hr,20,0,2,'0');
   if( sid_mn < 10 ) Serial.write('0');
   Serial.print( sid_mn );  Serial.write(':');
   LCD.printNumI(sid_mn,50,0,2,'0');
   if( sid_sec < 10 ) Serial.write('0');
   Serial.println( sid_sec );
   LCD.printNumI(sid_sec,80,0,2,'0');
   LCD.setFont(SmallFont); 

   display_stars();
}



void display_stars( ){
static int indx = 2555;
int i;
int hr, mn;

   hr = sid_hr;  mn = sid_mn;
   if( hr > 23 ) hr -= 24;
   // first time
   if( indx == 2555 ){
      for( i = 0; i < NUMSTAR; ++i ){
         if( bstar[i].hr > hr ) break;
         if( bstar[i].hr == hr && bstar[i].mn > mn ) break;
         display_stars2( i );
         delay(10);
      }
      indx = i;
      // if( indx < 0 ) indx = NUMSTAR - 1;
      if( indx >= NUMSTAR ) indx = 0;
   }

   // check if reached the next star
   if( hr == bstar[indx].hr && mn == bstar[indx].mn ){
      display_stars2( indx );
      new_target = indx;
      ++indx;
      if( indx >= NUMSTAR ) indx = 0;
   }
 
}

void display_stars2old( uint8_t p ){
static char snames[20];
static char cnames[20];
static char temp[45];

 // Serial.print( bstar[p].sname );  Serial.write(' ');

  strncpy( temp,bstar[p].sname,6 );  temp[6] = 0;
  strcat( temp, " " );
  strcat( temp, snames );
  strncpy( snames, temp, 18 );  snames[19] = 0;
  LCD.print( snames, 0, ROW2 );

  strncpy( temp, bstar[p].con,3 ); temp[3] = 0;
  strcat( temp, " ");
  strcat( temp, cnames );
  strncpy( cnames, temp, 18);  cnames[19] = 0;
  LCD.print( cnames, 0, ROW3 );
  
}

void display_stars2( int p ){
static char line2[25];
static char line3[25];
static char line4[25];
static char line5[25];
static char line6[25];
static char line7[25];

   strcpy( line7, line6 );
   strcpy( line6, line5 );
   strcpy( line5, line4 );
   strcpy( line4, line3 );
   strcpy( line3, line2 );

   strcpy( line2,bstar[p].con );
   strcat( line2, " " );
   strncat( line2, bstar[p].sname,15 );
   //strcat( line2, bstar[p].sname );
   line2[24] = 0;
   
   LCD.print( line2, 0, ROW2 );  LCD.clrRow(2,strlen(line2)*6 );
   LCD.print( line3, 0, ROW3 );  LCD.clrRow(3,strlen(line3)*6 );
   LCD.print( line4, 0, ROW4 );  LCD.clrRow(4,strlen(line4)*6 );
   LCD.print( line5, 0, ROW5 );  LCD.clrRow(5,strlen(line5)*6 );
   LCD.print( line6, 0, ROW6 );  LCD.clrRow(6,strlen(line6)*6 );
   LCD.print( line7, 0, ROW7 );  LCD.clrRow(7,strlen(line7)*6 );
  
}

/*****  extern I2C  functions needed by the OLED library  ******/
uint8_t i2byte_count;       // avoid overfilling i2c buffer ( 32 size for pi pico ?, it is 256 it seems )
uint8_t i2adr_saved;

void i2init(){
  Wire.begin();
  Wire.setClock(400000);
}

void i2start( uint8_t adr ){
  Wire.beginTransmission(adr);
  i2byte_count = 1;
  i2adr_saved = adr;
}


void i2send( unsigned int data ){
  if( ++i2byte_count == 253 ){
     i2stop();
     i2start( i2adr_saved ); 
  }
  Wire.write(data);
}
  

void i2stop( ){
  Wire.endTransmission();
  i2byte_count = 0;
}



//*************************************************** old code **********************************************


#ifdef NOWAY


// Tropical Year = 365.2425 (days).
// Date and time GMT of vernal equinox. 
// data from https://data.giss.nasa.gov/modelE/ar5plots/srvernal.html
struct  VERNAL_EQUINOX {
   uint16_t yr;
   uint8_t mo;
   uint8_t day;
   uint8_t hr;
   uint8_t mn;
};

#define VEQ_SIZE 38
struct VERNAL_EQUINOX vernal_eq[VEQ_SIZE] = {      // ? delta = +6hr, -10min each year. from 365.25 solar year / leap years
  { 2023, 3, 20, 21, 22 },                         // 33275 fudge factor for 1st year in table
  { 2024, 3, 20, 3, 11 },
  { 2025, 3, 20, 9, 0 },
  { 2026, 3, 20, 14, 49 },
  { 2027, 3, 20, 20, 38 },
  { 2028, 3, 20, 2, 28 },
  { 2029, 3, 20, 8, 17 },
  
  { 2030, 3, 20, 14, 6 },                         // 7115 offset, could remove table and use just one year
  { 2031, 3, 20, 19, 55 },
  { 2032, 3, 20, 1, 44 },
  { 2033, 3, 20, 7, 34 },
  { 2034, 3, 20, 13, 23 },
  { 2035, 3, 20, 19, 12 },
  { 2036, 3, 20, 1, 1 },
  { 2037, 3, 20, 6, 50 },
  { 2038, 3, 20, 12, 40 },
  { 2039, 3, 20, 18, 29 },
  
  { 2040, 3, 20, 0, 18 },
  { 2041, 3, 20, 6, 7 },
  { 2042, 3, 20, 11, 56 },
  { 2043, 3, 20, 17, 46 },
  { 2044, 3, 19, 23, 35 },
  { 2045, 3, 20, 5, 24 },
  { 2046, 3, 20, 11, 13 },
  { 2047, 3, 20, 17, 02 },
  { 2048, 3, 19, 22, 52 },
  { 2049, 3, 20, 4, 41 },
  
  { 2050, 3, 20, 10, 30 },
  { 2051, 3, 20, 16, 19 },
  { 2052, 3, 19, 22, 8 },
  { 2053, 3, 20, 3, 58 },
  { 2054, 3, 20, 9, 47 },
  { 2055, 3, 20, 15, 36 },
  { 2056, 3, 19, 21, 25 },
  { 2057, 3, 20, 3, 14 },
  { 2058, 3, 20, 9, 4 },
  { 2059, 3, 20, 14, 53 },
  
  { 2060, 3, 19, 20, 42 },
//  { 2070, 3, 20, 6, 54 },       // these 10 year intervals won't work with the additive fudge factor method
//  { 2080, 3, 19, 17, 06 },
//  { 2090, 3, 20, 3, 18 },
//  { 2099, 3, 20, 7, 41 }
  
};



// find the vernal equinox for the current year and current local sidereal time
// start from data time of vernal equinox, find difference to todays date,
// scale diff to sidereal seconds length, add/sub offset for longitude,
// convert to a data/time and save the hours, minutes, seconds

// table driven model
void get_GMT_base(){
int i,j;
int64_t t1,t2,diff;
int32_t fudge = 33275;
 
   // get the today info from the RTC which is using UTC time
   today = RTClib::now();

   for( i = 0,j = 0; i < VEQ_SIZE; ++i ){
   //if( vernal_eq[i].yr >= today.year() ) break;
   if( i == 7 ) break;   // !!! testing
      fudge += 21600;      // +6 hours
      fudge -=  660;       // -11 minutes  ? 648 on average, table is 11 11 11 11 10 11 11 11 11 10
      if( ++j == 5 ) fudge += 60, j = 0;
      fudge = fudge % 86400;  // mod 24 hours
   }

Serial.print(fudge); Serial.write(' ');
   if( i == VEQ_SIZE ) --i;    // last entry in table
Serial.print( i );  Serial.write(' ');

   GMT_eq = DateTime( vernal_eq[i].yr, vernal_eq[i].mo, vernal_eq[i].day, vernal_eq[i].hr, vernal_eq[i].mn, 0 );

   t1 = today.unixtime();   t2 = GMT_eq.unixtime();     // use 64 bit signed
   diff =  t1 - t2;
Serial.print( diff );   Serial.write(' ');   
   
   diff = (float)diff * SFACTOR;
Serial.print( diff );   Serial.write(' ');   

   t1 += diff;
   t1 -= 3600 * vernal_eq[i].hr;        //  add or sub time of eqinox 
   t1 -= 60 * vernal_eq[i].mn;

  // !!! fudge factors, why needed?  fractional part of year 365.2425
   // t1 += 9*3600;  t1 += 14*60;  t1 += 36;
   t1 += fudge;

  // longitude adjustment
   t1 += my_longitude * 240.0;     // 86400/360;
   
   DateTime new_time = DateTime( (uint32_t)t1 );
   sid_hr = new_time.hour();   sid_mn = new_time.minute();  sid_sec = new_time.second();

   Serial.print( new_time.year() );   Serial.write('/');
   Serial.print( new_time.month() );  Serial.write('/');
   Serial.print( new_time.day() );    Serial.write(' ');
   
   if( sid_hr < 10 ) Serial.write('0');
   Serial.print( sid_hr );  Serial.write(':');
   if( sid_mn < 10 ) Serial.write('0');
   Serial.print( sid_mn );  Serial.write(':');
   if( sid_sec < 10 ) Serial.write('0');
   Serial.println( sid_sec );   
}


void get_GMT_base(){
//int i,j;
int64_t t1,t2,diff;
int32_t fudge;
int32_t hr,mn;

   // { 2030, 3, 20, 14, 6 },                         // 7115 
   // { 2023, 3, 20, 21, 22 },                        // 33275
   // {2024, 3, 20, 3, 11 },                          // 54215

   if( GMTversion == 0 ){          // 2024 version
      fudge = 54215;
      GMT_eq = DateTime( 2024, 3, 20, 3, 11, 0 );
      hr = 3, mn = 11; 
   }
   else{                           // 2030 version
      fudge = 7115;
      GMT_eq = DateTime( 2030, 3, 20, 14, 6, 0 );
      hr = 14, mn = 6; 
   }
       
   // get the today info from the RTC which is using UTC time
   today = RTClib::now();

   // GMT_eq = DateTime( 2030, 3, 20, 14, 6, 0 );
   if( GMTversion ) Serial.print("Base2030 ");
   else Serial.print("Base2024 ");

   t1 = today.unixtime();   t2 = GMT_eq.unixtime();     // use 64 bit signed
   diff =  t1 - t2;
Serial.print( diff );   Serial.write(' ');   
   
   diff = (float)diff * SFACTOR;
Serial.print( diff );   Serial.write(' ');   

   t1 += diff;
   
   t1 -= 3600 * hr;        //  add or sub time of eqinox 
   t1 -= 60 * mn;

   t1 += fudge;            // end up at -12:07:25 or just before noon previous day. Should this be noon? We scaled seconds
                           // to be sidereal size seconds.  1.0027379 * 12 * 3600 = 43318.  Other calc is 43645.

  // longitude adjustment
   t1 += my_longitude * 240.0;     // 86400/360;
   
   DateTime new_time = DateTime( (uint32_t)t1 );
   sid_hr = new_time.hour();   sid_mn = new_time.minute();  sid_sec = new_time.second();

   Serial.print( new_time.year() );   Serial.write('/');
   Serial.print( new_time.month() );  Serial.write('/');
   Serial.print( new_time.day() );    Serial.write(' ');
   
   if( sid_hr < 10 ) Serial.write('0');
   Serial.print( sid_hr );  Serial.write(':');
   if( sid_mn < 10 ) Serial.write('0');
   Serial.print( sid_mn );  Serial.write(':');
   if( sid_sec < 10 ) Serial.write('0');
   Serial.println( sid_sec );   
}

struct BSTAR {
  char sname[20];
  char con[4];
  uint8_t hr;
  uint8_t mn;
};

// bright stars
#define NUMSTAR 23
struct BSTAR bstar[NUMSTAR] =   
{
 { "B Cas           " , "Cas",  0, 10 },
 { "B And        M31" , "And",  1, 10 },
 { "A Aries      M33" , "Ari",  2,  8 },
 { "Mirphak   x&hPer" , "Per",  3, 24 },
 { "Aldebaran    M45" , "Tau",  4, 35 },
 { "Rigel        B33" , "Ori",  5, 14 },
 { "Capella   M1 M37" , "Aur",  5, 16 },
 { "Betelgeuse   M42" , "Ori",  5, 55 },
 { "Sirius          " , "Cma",  6, 45 },
 { "CastorPollux M35" , "Gem",  7, 34 },
 { "Procyon         " , "Cmi",  7, 39 },
 { "B Cancer     M44" , "Cnc",  8, 15 },
 { "Regulus         " , "Leo", 10,  8 },
 { "Denebola        " , "Leo", 11, 50 },
 { "Spica           " , "Vir", 13, 25 }, 
 { "Alkaid       M51" , "UMa", 13, 47 },
 { "Arcturus   M5 M3" , "Boo", 14, 15 },
 { "Antares         " , "Sco", 16, 29 },
 { "B Her        M13" , "Her", 16, 30 },
 { "Vega         M57" , "Lyr", 18, 36 },
 { "Altair       M27" , "Aql", 19, 50 },
 { "Deneb           " , "Cyg", 20, 41 },
 { "Markab          " , "Peg", 23, 01 }
};




#endif
