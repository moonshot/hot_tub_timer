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
const int NUM_TEST_CASES = 6;


// Data for testing
const int TEST_DATA[NUM_TEST_CASES][8]={
  // year, month, day, hour, min, sec, expected_peak_start, expected_peak_end
  {13, 1, 1, 12, 0, 0, 15, 20}, // ST Winter
  {13, 3, 1, 12, 0, 0, 15, 20}, // ST Winter
  {13, 4, 1, 12, 0, 0, 16, 21}, // DT Winter
  {13, 6, 1, 12, 0, 0, 12, 17}, // DT Summer
  {13, 11, 8, 12, 0, 0, 16, 21}, // DT Winter
  {13, 12, 1, 12, 0, 0, 15, 20} // ST Winter
};


// Calculate the peak start and end times
// this changes between winter and summer
// During the winter peak hours are 5-9pm
// During the summer peak hours are noon-5pm
// However the RTC is not daylight savings aware
// so we need to handle this here also
void get_peak_times (DateTime t, int pt[]) {
  pt[0] = 12;
  pt[1] = 17;
}


// Print out the test case time data and result
void printResult(const DateTime& dt, const char* res) {
    Serial.print(res);
    Serial.print('\t');
    Serial.print(dt.day(), DEC);
    Serial.print('/');
    Serial.print(dt.month(), DEC);
    Serial.print('/');
    Serial.print(dt.year(), DEC);
    Serial.println();
}


// Test the calculations for peak times
// Note that the global variable DEBUG needs to be set to true
// in order for the test to be triggered and to output their results
void test_peak_times () {
  int i;
  int test_pt[2];

  for (i = 0; i < NUM_TEST_CASES; i += 1) {
    DateTime test_t (TEST_DATA[i][0], TEST_DATA[i][1], TEST_DATA[i][2], TEST_DATA[i][3], TEST_DATA[i][4], TEST_DATA[i][5]);
    get_peak_times(test_t, test_pt);
    if (test_pt[0] == TEST_DATA[i][6] && test_pt[1] == TEST_DATA[i][7]) {
      printResult(test_t, "PASS");
    } 
    else {
      printResult(test_t, "FAIL");
    }
  }
}

// Initialize variables and run tests
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

  // Set the RTC to the date & time this sketch was compiled
  RTC.adjust(DateTime(__DATE__, __TIME__));

  if (DEBUG) {
    test_peak_times();
  }
}

// Check the current time and if it is within peak times then
// make sure the heater will not come on
void loop () {
  DateTime t = RTC.now();

  if (DEBUG) {
    Serial.print("Day of week: ");
    Serial.println(t.dayOfWeek());
  }

  // Peak hours are only on weekdays (day=1-5)
  if (t.dayOfWeek() > 0 && t.dayOfWeek() < 6) {

    if (DEBUG) {
      Serial.print("Hour: ");
      Serial.println(t.hour());
    }

    // Initialize an array that will contain the
    // peak start and end times
    int peak_times[2];

    // Populate the array 'peak_times' with the 
    // peak start and end times for right now
    get_peak_times(t, peak_times);

    // Turn the relay on or off depending on
    // whether or not it is peak time
    if (t.hour() >= peak_times[0] && t.hour() < peak_times[1] ) {
      if (DEBUG) {
        Serial.println("Peak time.");
      }
      // Switch the relay to the Normally Open leg
      // (fixed resistor value for 104 degrees F)
      digitalWrite(RELAYpin, HIGH);
    } 
    else {
      if (DEBUG) {
        Serial.println("Off Peak.");
      }
      // Switch the relay to use the leg
      // with the actual thermometer reading
      digitalWrite(RELAYpin, LOW);
    }

  } 
  else {
    // Weekends are always off peak
    if (DEBUG) {
      Serial.println("It's the freakin' weekend. Off Peak.");
    }
    // Switch the relay to use the leg
    // with the actual thermometer reading
    digitalWrite(RELAYpin, LOW);
  }

  // No need to be super precise
  delay(60000);
}

