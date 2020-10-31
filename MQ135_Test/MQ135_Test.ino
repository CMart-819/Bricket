// MQ-135 AIR QUALITY :: http://fitrox.lnwshop.com
// MQ-135 CARBON DIOXIDE
#include <LiquidCrystal_I2C.h>

// ต่อเซ็นเซอร์กับขา A0
#define MQ135_PIN A0
LiquidCrystal_I2C lcd(0x3f, 16, 2);

// RL ดูจากหลังบอร์ด
int MQ_RESISTOR = 1000;

// RO ที่คำนวณได้
long RO = 24743;

//min และ max Rs/Ro
float MINRSRO = 0.358;
float MAXRSRO = 2.428;

// a และ b คำนวณได้จากกราฟ
float a = 116.602;
float b = -2.769;
 
void setup() {
  Serial.begin(9600);
  // Pre heat 20 วินาที
  Serial.println("Pre-heat sensor 20 seconds");
  delay(20000);
  Serial.println("Sensor ready start reading");
}
 
void loop() {
// อ่านค่า ADC และแปลงเป็นค่า Rs พร้อมแสดงค่าทาง Serial Monitor
  int ADCRAW = analogRead(MQ135_PIN);
  float RS = ((1024.0*MQ_RESISTOR)/ADCRAW) - MQ_RESISTOR;
  Serial.print("Rs: ");
  Serial.println(RS);
  
// คำนวณ Rs/Ro พร้อมแสดงค่าทาง Serial Monitor
  float RSRO = RS/RO;
  Serial.print("Rs/Ro: ");
  Serial.println(RSRO);
  
// ถ้า Rs/Ro อยู่ในช่วงที่วัดได้ ให้คำนวณและแสดงค่า ppm 
// ถ้าค่าที่ได้น้อยหือมากเกินไปให้แสดงข้อความว่าเกินช่วงที่เซ็นเซอร์จะวัดได้
  if(RSRO < MAXRSRO && RSRO > MINRSRO) {
   float ppm = a*pow(RS/RO, b);
   Serial.print("CO2 : ");
   Serial.print(ppm);
   Serial.println(" ppm");
  } 
  else {
   Serial.println("Out of range.");
  }
 delay(5000);
}
