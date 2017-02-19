#include "rgb_lcd.h"
#include <Wire.h>
const int rl = 2;
const int gl = 3;
const int bl = 4;
const int touch = 5;
const int pinAdc = A0;
long average = 0;
int cnt = 0;
rgb_lcd test;
int avg[20];
boolean listening = 1;
unsigned long currentMillis = 1;
unsigned long* times = (unsigned long*)(malloc(sizeof(unsigned long)*500));
char* beatmap = (char*)(malloc(sizeof(char)*500));
int curr = 0;
int step;
void setup()
{
  pinMode(rl, OUTPUT);
  pinMode(gl, OUTPUT);
  pinMode(bl, OUTPUT);
  test.begin(16,2);
  test.setRGB(0,0,255);
  listening = 1;
  for(int i = 0; i<20; i++)
  {
    avg[i] = 0;
  }
  step = 350;
}

void loop()
{
  if(digitalRead(touch) && listening)
  {
    test.setCursor(0,0);
    Serial.println("E");
    delay(2000);
    listening = 0;
  }
  long sum = 0;
    for(int i=0; i<32; i++)
    {
        sum += analogRead(pinAdc);
    }
  
//    long sum = analogRead(pinAdc);
    sum >>= 5;

    average = average-avg[cnt %20]/20 + sum/20;
    avg[cnt % 20] = sum;
    cnt = cnt + 1;
    if(cnt > 20 & listening)
    {
      
      if(sum > 750)
      {
        if(currentMillis != 0) {
          test.setCursor(0,0);
          times[curr] = millis()-currentMillis;
          test.print(sum);
          test.setCursor(0,1);
          test.print(5*average);
          currentMillis = millis();
          curr = curr + 1;
        }
        else {
          currentMillis = millis();
        }
        test.setRGB(255,175,0);
        digitalWrite(rl, HIGH);
      } 
    }
    else if(cnt >20) {
    //Start decoding this nonsense
      int maxtimes = curr;
      curr = 0;
      delay(10);
      for(curr = 0; curr < maxtimes; curr++) {
        test.setCursor(0,0);
        test.print(times[curr]);
        delay(100);
        while(times[curr] > 1.5*step) {
          strcat(beatmap,"m");     
          times[curr] = times[curr] - step;
        }
        if(.5*step <= times[curr] && times[curr]<= 1.5*step) {
          strcat(beatmap,"o");
        }
        else if(0 <= times[curr] && times[curr] < .5*step) {
          strcat(beatmap,"n");
        }
      }
      test.setCursor(0,0);
      test.print(beatmap);
      //Decode Game Logic states here based on beatmap
    }
    digitalWrite(rl,LOW);
}


