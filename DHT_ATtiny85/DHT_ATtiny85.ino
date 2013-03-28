// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"
int button1 = 4;
// pin 3 is open
int fan1 = 2;
int fan2 = 1;
int toggle = 0;
int val = 0;

#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
  pinMode(fan1, OUTPUT);
  pinMode(fan2, OUTPUT);
  pinMode(button1, INPUT);
  dht.begin();
}

void loop() {
//       digitalWrite(7,HIGH);
//    digitalWrite(6,HIGH);
//digitalWrite(6,HIGH);
//digitalWrite(7,HIGH);
//analogWrite(6,255);
  val = digitalRead(button1);
  Serial.print("val: ");
  Serial.print(val);
  //IF button is pressed, Manually turn on the fans
  if (val == LOW) {
    if (toggle == 1) {
    digitalWrite(fan1,LOW);
    digitalWrite(fan2,LOW);
    toggle = 0;
    } else if (toggle == 0) {
    digitalWrite(fan1,HIGH);
    digitalWrite(fan2,HIGH);
    toggle = 1;
    }
  } 
      Serial.print("toggle: ");
  Serial.println(toggle);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();
   float lambda = (log10(h)-2.0)/0.4343+(17.62*t)/(243.12+t);
  float tf = t*(9/5) + 32 + 13;
  float td = 243.12*lambda/(17.62-lambda);
  float tdf = td*(9/5) + 32 + 13;
 

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
   Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(tf);
    Serial.print(" *F");
    Serial.print(" \t");
    Serial.print("lambda: ");
    Serial.print(lambda);
    Serial.print(" \t");
    Serial.print("DewPoint: ");
    Serial.print(tdf);
    Serial.println(" *F");
  }
  
  if ((tf-5) <= tdf) {
    digitalWrite(fan1,HIGH);
    digitalWrite(fan2,HIGH);
    delay(30000); //pause for 60 seconds to let the fans run to clear humidity
  } else if (toggle == 0) {
    digitalWrite(fan1,LOW);
    digitalWrite(fan2,LOW);
  }
}
