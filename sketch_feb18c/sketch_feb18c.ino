#include "rgb_lcd.h"
#include <Wire.h>
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
int leng = 0;
void setup()
{
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
          times[curr] = millis()-currentMillis;
          currentMillis = millis();
          curr = curr + 1;
        }
        else {
          currentMillis = millis();
        }
      } 
    }
    else if(cnt >20) {
    //Start decoding this nonsense
      int maxtimes = curr;
      curr = 0;
      delay(10);
      for(curr = 0; curr < maxtimes; curr++) {
        while(times[curr] > 1.5*step) {
          strcat(beatmap,"m");     
          leng = leng+1;
          times[curr] = times[curr] - step;
        }
        if(.5*step <= times[curr] && times[curr]<= 1.5*step) {
          leng = leng+2;
          strcat(beatmap,"on");
        }
        else if(0 <= times[curr] && times[curr] < .5*step) {
          leng = leng+1;
          strcat(beatmap,"n");
        }
      }
      test.setCursor(0,0);
      for(curr = 1; curr < leng-1; curr++){
        if(beatmap[curr] == 'n' && beatmap[curr+1] == 'm') {
          test.setRGB(255,0,0);
          beatmap[curr+1] = 'o';
        }
        if(beatmap[curr] == 'o' && beatmap[curr-1] == 'n' && beatmap[curr+1] == 'n') {
          beatmap[curr] = 'd';
        }
      }
      test.print(beatmap);
      free(beatmap);
      free(times);
      //Decode Game Logic states here based on beatmap
      
    }
}


