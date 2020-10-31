// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// Replace with your network credentials
const char* ssid = "NeeTz"; //เปลี่ยนเป็นชื่อเน็ต
const char* password = "xxxneetz"; //เปลี่ยนเป็นพาสเน็ต

#define DHTPIN 27     // Digital pin connected to the DHT sensor
#define MQ135_PIN 25      // Analog pin connected to MqQ135 sensor
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.getTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.getHumidity();
  float hh = h/1000;
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(hh);
    return String(hh);
  }
}

String readaq(){
  aq = analogRead(MQ135_PIN);
  Serial.print("Air quality value : ");
  Serial.println(aq);
  delay(5000);

  if (isnan(aq)) {
    Serial.println("Failed to read from AQ135 sensor");
    return "--";
  }
  if aq < 450{
    Serial.print("Great");
    return String("Great");
  }  
  if 450 < aq < 600{
    Serial.print("OK");
    return String("OK");
  }
  if 600 < aq < 1000{
    Serial.print("Concerning");
    return String("Concerning");
  }
  if aq > 1000{
    Serial.print("Critical");
    return String("Critical");
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
    .aq-lebel{
      front-size:1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>"Bricket Smart Box Server"</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
  <p>
      <i class="fas fa-wind" style="color:#255f63";></i>
      <span class="aq-lebel">Air Quality</span>
      <span id="aq">%aq</span>
  </p>
</body>

<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;

setInterVal(function( ){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("aq").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET","/aq".true);
  xhttp.send();
},10000);
</script>

</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  else if (var == "aq"){
   return readaq(); 
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.setup(27);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
  server.on("/aq", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readaq().c_str());
  });
  
  // Start server
  server.begin();

  lcd.begin();
  lcd.setCursor(0, 0);
  lcd.print("T&H Tracker");
  lcd.setCursor(0, 1);
  lcd.print("Bricket.co");
  delay(3000);
  lcd.clear();
}
 
void loop(){
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

  int aq = analogRead(MQ135_PIN);

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
  lcd.setCursor(0, 2);
  lcd.print("Air Quality: ");

    if aq < 450{
    lcd.print("Great");
  }
  if 450 < aq < 600{
    lcd.print("OK");
  }
  if 600 < aq < 1000{
    lcd.print("Concerning");
  }
  if aq > 1000{
    lcd.print("Critical");
  }

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
  Serial.print("Air quality: ");

  if aq < 450{
    Serial.print("Great");
  }
  if 450 < aq < 600{
    Serial.print("OK");
  }
  if 600 < aq < 1000{
    Serial.print("Concerning");
  }
  if aq > 1000{
    Serial.print("Critical");
  }
}
