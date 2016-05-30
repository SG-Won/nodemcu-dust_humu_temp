// 128x64 LCD 셋팅
// Arduino UNO => PIN 4 = SCL, PIN 5 = SDA
// NodeMCU Dev Kit => D1 = SCL, D2 = SDA
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
  
#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
// 128x64 LCD 여기까지


// DHT22 설정 여기부터
#include "DHT.h"
#define DHTPIN 14     // NodeMCU Dev Kit => SD0 = 14
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
// DHT22 설정 여기까지


// 먼지센서 설정 여기부터
int measurePin = A0;
int ledPower = 12;
 
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
 
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;
// 먼지센서 설정 여기까지
 
void setup(){
//LCD관련 초기화
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();  // Clear the buffer.
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
//LCD관련 여기까지  

  Serial.begin(115200);
  pinMode(ledPower,OUTPUT); //먼지센서 초기화
  dht.begin(); //DHT22 초기
}

int getdust(){
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);
  
  voMeasured = analogRead(measurePin); // read the dust value
  
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  // 0 - 3.3V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (3.3 / 1024);
  
  dustDensity = 100 * calcVoltage - 0.1;
  return(dustDensity);
}
 
void loop(){
  display.clearDisplay();
  display.setCursor(30,0); //(왼쪽, 위)
  display.println("SuperGenius");
  display.setCursor(7,10);
  display.println("Environment Monitor");
  display.drawFastHLine(0, 22, 128, WHITE); //라인그리기 (Left, Top, Right, Color)
  display.setCursor(0,27);
  display.println("Temperature: ");
  display.println("Humidity: ");
  display.println("Micro Dust: ");
  display.println("Noise: ");
  display.display();

  //먼지센서 값 호출
  int dustA = map(getdust(),8,133,10,100);
  char dustB[4] = "";
  sprintf(dustB,"%03d",dustA);
  //먼지센서 여기까지

  //DHT22 값 호출
  float h = dht.readHumidity();  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  // DHT22 값 호출 여기까


  //먼지센서 시리얼
  Serial.print("Dust: ");
  Serial.print(dustB);
  Serial.print(" %\t\t");

  //DHT22 시리얼
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C\t\t");
  Serial.print(f);
  Serial.print(" *F\t\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C\t\t");
  Serial.print(hif);
  Serial.println(" *F");


  //LCD 표시
  display.setCursor(78,27);
  display.print(t);
  display.print("`C");
  display.display();

  display.setCursor(60,35);
  display.print(h);
  display.print("%");
  display.display();
  
  display.setCursor(70,43);
  display.print(dustB);
  display.print("mg/m3");
  display.display();
  
  delay(2000);
}


