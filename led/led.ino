#include <DS3232RTC.h>
#include <Wire.h>
#include <TimeLib.h>

#define PIR_INPUT 2
#define LED_COLD 3
#define LED_WARM 5
#define PHASE_MINUTES 15
 
void setup() {
  Serial.begin(9600);
  setupLight();
  setupClock();
  setupPIR();
}

void setupLight() {
  pinMode(LED_COLD, OUTPUT);
  pinMode(LED_WARM, OUTPUT);
  turnOff();
}

void setupClock() {
  // the function to get the time from the RTC
  setSyncProvider(RTC.get);
  if(timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  
  // Set alarm clock (everyday at 6:40am)
  RTC.setAlarm(ALM1_MATCH_HOURS, 50, 6, 1);
}

void setupPIR() {
  pinMode(PIR_INPUT, INPUT);
  turnOff();
}

void loop() {
  // setClockTime(); // Only to set the initial time
  // sunrise(); //  Only for testing purpose
  printTimeToSerial();
  if ( RTC.alarm(ALARM_1) ) {
    // FIXME: Checken if it is a weekday
  
    Serial.print("Alarm1 has triggered!");
    sunrise();
  }

  for (int i = 0; i < 600; i++) {
    unsigned int pirInput = digitalRead(PIR_INPUT);
    if (pirInput == HIGH) {
      printTimeToSerial();
      motionLight();
    } else {
      delay(100);
    }
  }
}

// Wake-Up Light 
void sunrise() {
  Serial.println("Starting sunrise");
  int i = 0;
  float delayms = 10 * PHASE_MINUTES * 60;
  
  while (++i < 100) { 
    delay(delayms);
    analogWrite(LED_WARM, f(i));
    if (i >= 80) {
      analogWrite(LED_COLD, f(i - 80));
    }
  }

  i = 100 - 80;
  while (++i < 100) {
    delay(delayms); 
    analogWrite(LED_COLD, f(i));
  }

  while (i-- > 80) {
    delay(delayms);
  }
  Serial.println("Sunrise over");
  turnOff();
}

// Motion Light (if you have to get up during the night [crying babies anyone?])
void motionLight() {
  Serial.println("Starting motion light");
  int i = 0;
  float delayms = 100;

  // 1,2 Sekunde hochfahren (aber nur bis 12% Helligkeit)
  while (++i < 12) { 
    analogWrite(LED_WARM, f(i));
    delay(delayms);
  }

  delay(8000);

  // 3 second shutdown
  while (i-- > 0) {
    analogWrite(LED_WARM, f(i));
    delay(delayms * 3);
  }
  Serial.println("Motion light over");
  turnOff();
}

// returns values between 0, 255 for inputs between (0, 99)
int f(double i) {
  int f = (int) ((0.0258505 * i * i) - (0.00646116 * i) + 1);
  Serial.println(f);
  return f;
}

void turnOff() {
  analogWrite(LED_WARM, 0);
  analogWrite(LED_COLD, 0);
}


void printTimeToSerial() {
    printDigits(hour(), ' ');
    printDigits(minute(), ':');
    printDigits(second(), ':');
    printDigits(day(), ' ');
    printDigits(month(), '.');
    Serial.print('.');
    Serial.print(year()); 
    Serial.println(); 
}

void printDigits(int digits, char delim) {
    Serial.print(delim);
    if(digits < 10) {
        Serial.print('0');
    }
    Serial.print(digits);
}

// Set initial clock time and date. Only to be used once to init the clock!
void setClockTime() {
  tmElements_t tm;
  tm.Hour = 11;
  tm.Minute = 59;
  tm.Second = 00;
  tm.Day = 6;
  tm.Month = 11;
  tm.Year = 2016 - 1970;
  int success = RTC.write(tm); //set the RTC from the tm structure (0 = success)
  Serial.print(success);
}



