#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
  Serial.begin(9600);
  lcd.begin();
  dht.begin();
  lcd.print("T&H Tracker");
  lcd.setCursor(0, 1);
  lcd.print("Bricket.Co");
  delay(3000);
  lcd.clear();
}

void loop() {

  delay(3000);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Failed to read!");
    Serial.print("Failed to read!");
    return;
  }

  float hi = dht.computeHeatIndex(f, h);

  lcd.setCursor(0, 0);
  lcd.print("H:");
  lcd.print(h, 1);
  lcd.print("% ");
  lcd.print("T:");
  lcd.print(t, 1);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Heat In: ");
  lcd.print((hi-32)*5/9, 1);
  lcd.println("C  ");

  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" C ");
  Serial.print(f);
  Serial.print(" F\t");
  Serial.print("Heat index: ");
  Serial.print((hi-32)*5/9);
  Serial.println(" C");
}
