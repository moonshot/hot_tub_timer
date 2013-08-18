// The purpose of this circuit is to prevent the hot tub
// from turning on its heater during peak hours
// when the electric rate is doubled. $$$.
// During the summer (June 1 through September 30), 
// the peak period is on non-holiday weekdays from noon to 5 p.m. 
// During the winter (October 1 through May 31), 
// the peak period is on non-holiday weekdays from 4 p.m. to 9 p.m.

// This sketch is based on examples from http://www.ladyada.net/learn/breakoutplus/ds1307rtc.html
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;
const int RTCgnd =  16; // analog 2
const int RTC5v = 17; // analog 3
const int RELAYpin = 3; // digital 3, for activating the relay
const boolean DEBUG = false;

void setup () {
  
  pinMode(RTCgnd, OUTPUT); 
  pinMode(RTC5v, OUTPUT); 
  pinMode(RELAYpin, OUTPUT); 
  digitalWrite(RTCgnd, LOW);
  digitalWrite(RTC5v, HIGH);
  digitalWrite(RELAYpin, LOW); // default is the hot tub temp sensor
  
  if (DEBUG) {
    Serial.begin(57600);
  }
  
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    if (DEBUG) {
      Serial.println("RTC is NOT running!");
    }
  }

  // following line sets the RTC to the date & time this sketch was compiled
  RTC.adjust(DateTime(__DATE__, __TIME__));
}

void loop () {
  DateTime t = RTC.now();
 
  if (DEBUG) {
    Serial.print("Day of week: ");
    Serial.println(t.dayOfWeek());
  }

  // peak hours are only on weekdays (day=1-5)
  if (t.dayOfWeek() > 0 && t.dayOfWeek() < 6) {

    if (DEBUG) {
      Serial.print("Hour: ");
      Serial.println(t.hour());
    }

    // during the winter peak hours are 5-9pm
    // during the summer peak hours are noon-5pm
    // Switch the relay to the Normally Open leg
    // (fixed resistor value for 104 degrees F)
    if (t.hour() >= 12 && t.hour() < 17 ) {
      if (DEBUG) {
        Serial.println("Peak time.");
      }
      digitalWrite(RELAYpin, HIGH);
    } else {
      if (DEBUG) {
        Serial.println("Off Peak.");
      }
      digitalWrite(RELAYpin, LOW);
    }

  } else {
   // weekends are always off peak
    if (DEBUG) {
      Serial.println("It's the freakin' weekend. Off Peak.");
    }
    digitalWrite(RELAYpin, LOW);
  }
 
  // no need to be super precise
  delay(60000);
}
