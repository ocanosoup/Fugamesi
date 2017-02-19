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
unsigned long currentMillis = 1;
void setup()
{
  pinMode(rl, OUTPUT);
  pinMode(gl, OUTPUT);
  pinMode(bl, OUTPUT);
  pinMode(touch, INPUT);
  Serial.begin(115200);
  test.begin(16,2);
  test.setRGB(0,0,255);
  for(int i = 0; i<20; i++)
  {
    avg[i] = 0;
  }
}

void loop()
{
  if(digitalRead(touch))
  {
    Serial.println("E");
    Serial.end();
    exit(1);
  }
  long sum = 0;
    for(int i=0; i<32; i++)
    {
        sum += analogRead(pinAdc);
    }
  
//    long sum = analogRead(pinAdc);
    sum >>= 5;
//    if(cnt <= 20) {
//      average = average-avg[cnt %20]/20 + sum/20;
//      avg[cnt % 20] = sum;
//      cnt = cnt + 1;
//    }
//    else if(sum < 3*average)
//    {
//      average = average-avg[cnt %20]/20 + sum/20;
//      avg[cnt % 20] = sum;
//      cnt = cnt + 1;
//    }
    average = average-avg[cnt %20]/20 + sum/20;
    avg[cnt % 20] = sum;
    cnt = cnt + 1;
    if(cnt > 20)
    {
      
      if(sum > 3*average)
      {
        if(currentMillis != 0) {
            unsigned long out = millis() - currentMillis;
            out = out;
            test.setCursor(0,0);
            test.print(out);
            Serial.println(out);
            currentMillis = millis();
        }
        else {
          currentMillis = millis();
        }
        test.setRGB(255,175,0);
        digitalWrite(rl, HIGH);
      } 
//      else if(sum > 1.7*average)
//      {
//        test.setCursor(0,1);
//        test.print(sum);
//        test.setRGB(0,255,0);
//        digitalWrite(gl, HIGH);
//      }
//      else if(sum > 1.2*average)
//      {
//        test.setCursor(0,1);
//        test.print(sum);
//        test.setRGB(200,100,50);
//        digitalWrite(bl, HIGH);
//      }
    }
    delay(10);
    digitalWrite(rl, LOW);
    digitalWrite(gl, LOW);
    digitalWrite(bl, LOW);

}


