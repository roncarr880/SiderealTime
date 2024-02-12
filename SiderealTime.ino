//  Pi Pico
//  display local sidereal on an OLED display.  Uses a DS3231 clock which is good for one century 2000 - 2099
//  display items of interest crossing the meridian, fake goto with steppers(test steppers on cncv3)

//  each second add 1.0027379 to sidereal time, or ( total delta seconds / 365.2425) * 366.2425

// from OAT  #define SIDEREAL_SECONDS_PER_DAY 86164.0905f

#include <Arduino.h>
#include "PinMap.h"
#include "BrightStars.h"
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
  AccelStepper RAstep(AccelStepper::DRIVER, RA_STEP, RA_DIR);    // pins step, direction
  AccelStepper DECstep(AccelStepper::DRIVER, DEC_STEP, DEC_DIR);

extern unsigned char SmallFont[];
extern unsigned char MediumNumbers[];
extern unsigned char BigNumbers[];


#define SFACTOR0 0.0027379           // sidereal  fast by this amount
#define SFACTOR1 1.0027379

DateTime GMT_eq;
DateTime today;

float my_longitude = -69.524125;
float my_latitude  = 44.44689;
uint8_t sid_hr, sid_mn, sid_sec;
volatile uint8_t finding;      // goto started
int new_target, old_target;


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
  RAstep.setEnablePin( DRV_ENABLE );
  RAstep.setPinsInverted( 0,0,1);     // direction, step , enable low
  DECstep.setAcceleration( 10.0 );
  DECstep.setMinPulseWidth( 3 );

  DECstep.enableOutputs();
  RAstep.enableOutputs();

  // timer running at 1ms. Max speed will be timer limited at 1000.
  // step mode 16, 200 step motor  18.75 rpm max or 3.2 seconds for one rotation
  RAstep.setMaxSpeed( 1001.0 );    // need to be last in order to work at all?
  DECstep.setMaxSpeed( 1001.0 );
  //RAstep.setSpeed(  3.42 );     // fake sidereal rate enabled  test


  // moveTo( position relative to zero starting position ) for goto's
  // move( relative to the current position );
  // setCurrentPosition( for fixups )
  RAstep.moveTo( -1 );                 // checking if alive
  DECstep.moveTo( 1 );
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
     if( RAstep.distanceToGo() == 0 ) RAstep.setSpeed(  0.71 ), finding = 0;
  } 
  else if( RAstep.runSpeed() ){
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
   long ha = RAstep.currentPosition();
   long dec = DECstep.currentPosition();
   interrupts();
   Serial.write('d'); Serial.print( dec );  Serial.write(' ');
   Serial.write('h'); Serial.write('a'); Serial.print( ha ); Serial.write(' ');

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
