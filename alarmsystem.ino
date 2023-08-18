#include <LiquidCrystal.h>
#include <dht.h>
#include "IRremote.h"
#include "IR.h"
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

dht DHT;
// Temp/humidity sensor pin
#define DHT11_PIN 2


IRrecv irrecv(13);
// The user input from IRremote
decode_results results;

// hours, minutes, seconds
int h = 12;
int m = 59;
int s = 45;

// Used to print the temp and humidity on off-ticks
float lastTemp;
float lastHumid;

bool alarm = false;

// An accurate increment of time (ms)
static uint32_t last_time, now = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  now = millis();
  irrecv.enableIRIn();
}


void tempHumidAlarm() {
  int chk = DHT.read11(DHT11_PIN);
    // The temp/humidity will print -999 at every other tick, so we will only
    // print when the temperature is not -999
    if (DHT.temperature != -999)
    {
     lastTemp = DHT.temperature;
     lastHumid = DHT.humidity;
     Serial.print("Temperature = ");
     Serial.println(DHT.temperature);
     Serial.print("Humidity = ");
     Serial.println(DHT.humidity);
     lcd.setCursor(0, 1);
     lcd.print("T=");
     lcd.print(DHT.temperature);
     lcd.print(" H=");
     lcd.print(DHT.humidity);
    }
    // Continues to print the temp/humidity when not yet updated
    else {
      lcd.setCursor(0, 1);
      lcd.print("T=");
      lcd.print(lastTemp);
      lcd.print(" H=");
      lcd.print(lastHumid);
    }
}

void time() {
  lcd.begin(16, 2);
  //Prints the time in hh:mm:ss 
  lcd.setCursor(0, 0);
  if(h < 10)
    lcd.print("0");
  lcd.print(h);
  lcd.print(":");
   if(m < 10)
     lcd.print("0");
   lcd.print(m);
   lcd.print(":");
   if(s < 10)
     lcd.print("0");
   lcd.print(s);

   // Ensures time is valid, (e.g. min <= 60)
   if(s==60){
    s=0;
    m=m+1;
   }
   if(m==60)
   {
    m=0;
    h=h+1;
   }
   if(h==13)
   {
    h=1;
   }
}

void loop() {
  for (int i = 0; i < 5; i++)
  {
    // I found this alternative to delay(1000),
    // it allows me to input from the IRremote without as much delay.
    while ((now - last_time) < 270)
    {
      now = millis();
    }
    last_time = now;
    if (!alarm)
      time();
    tempHumidAlarm();
    int tmpValue;
    if (irrecv.decode(&results)) // have we received an IR signal?
    {
      for (int i = 0; i < 22; i++)
      {
        // keyValue[] and keyBuf[] contain all the keys on the IRremote
        if ((keyValue[i] == results.value) && (i<KEY_NUM))
        {
          Serial.println(keyBuf[i]);
          tmpValue = results.value;
          Serial.println(results.value);
          // Increments hour when "1" is pressed
          if (results.value == 16724175) {
            h = h + 1;
          }
          // Increments minutes when "2" is pressed
          if (results.value == 16718055) {
            m = m + 1;
          }
          // Increments seconds when "3" is pressed
          if (results.value == 16743045) {
            s = 0;
          }
        }
        else if(REPEAT==i)
        {
          results.value = tmpValue;
        }
      }
      irrecv.resume(); // receive the next value
    }
    if (lastHumid > 85)
    {
      alarm = true;
      tone(6, 523);
    }
  }
  // increments seconds
  s = s + 1;
  // Ensures time is valid even outside of the "fast" loop
   if(s==60){
    s=0;
    m=m+1;
   }
   if(m==60)
   {
    m=0;
    h=h+1;
   }
   if(h==13)
   {
    h=1;
   }
}
