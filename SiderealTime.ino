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
float ssec;

void setup() {

  i2init();
  Serial.begin(9600);

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
   
   if( sid_hr < 10 ) Serial.write('0');
   Serial.print( sid_hr );  Serial.write(':');
   if( sid_mn < 10 ) Serial.write('0');
   Serial.print( sid_mn );  Serial.write(':');
   if( sid_sec < 10 ) Serial.write('0');
   Serial.println( sid_sec );   
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
  if( ++i2byte_count == 255 ){
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




#endif
