#include <LiquidCrystal.h>
#include <TinyGPS.h>

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);

const int relay_Pin = 2;
const int buzzer_Pin = 3;
const int ir_Sensor = 10;
const int alcohol_Sensor = 11;
const int vibration_Sensor = 12;
const int tilt_Sensor = 13; 
TinyGPS gps;
long lat, lon;
bool ir_status = LOW;
bool alcohol_Status = LOW;
bool vibration_Status = LOW;
bool tilt_Status = LOW;

void setup() {
  pinMode(relay_Pin, OUTPUT);
  pinMode(buzzer_Pin, OUTPUT);
  pinMode(ir_Sensor, INPUT);
  pinMode(alcohol_Sensor, INPUT);
  pinMode(vibration_Sensor, INPUT);
  pinMode(tilt_Sensor, INPUT_PULLUP); 
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("ACCIDENT DETECTION");
  lcd.setCursor(3, 2);
  lcd.print("SYSTEM");
}


void loop() {
  ir_status = digitalRead(ir_Sensor);
  delay(100);
  if (ir_status == HIGH) {
    digitalWrite(buzzer_Pin, LOW);
    delay(200);
    lcd.clear();
    lcd.print("Seat Belt");
    lcd.setCursor(3, 2);
    lcd.print("Detected");
    delay(500);

    while (1) {
      alcohol_Status = digitalRead(alcohol_Sensor);
      delay(100);
      if (alcohol_Status == LOW) {
        digitalWrite(buzzer_Pin, LOW);
        delay(200);
        lcd.clear();
        lcd.print("Alcohol not");
        lcd.setCursor(3, 2);
        lcd.print("Detected");
        delay(500);
        digitalWrite(relay_Pin, HIGH);
        delay(200);

        while (1) {
          lcd.clear();
          lcd.print("Vehicle Started");
          delay(500);

          while (1) {
            vibration_Status = digitalRead(vibration_Sensor);
            tilt_Status = digitalRead(tilt_Sensor); 
            delay(100);

            if (vibration_Status == HIGH) {
              if (tilt_Status == LOW) {
                lcd.clear();
                lcd.print("Critical Accident");
                lcd.setCursor(3, 2);
                lcd.print("Sending Msg");
                delay(500);
                digitalWrite(relay_Pin, LOW); // Turn off the relay
                Serial.println("AT+CMGF=1");    // Sets the GSM Module in Text Mode
                delay(100);  // Delay of 100 milliseconds
                Serial.println("AT+CMGS=\"+919922512017\"\r"); // Replace with mobile number
                delay(100);
                Serial.println("Critical Accident Detected");// The SMS text you want to send
                Serial.println("please check location");// The SMS text you want to send
                while (1) {
                  gps_read();
                }
              } else {
                lcd.clear();
                lcd.print("Non-Critical Accident");
                lcd.setCursor(3, 2);
                lcd.print("No Msg Sent");
                delay(500);
              }
            }
          }
        }
      } else {
        lcd.clear();
        lcd.print("Alcohol ");
        lcd.setCursor(3, 2);
        lcd.print("Detected");
        delay(500);
        digitalWrite(relay_Pin, LOW); // Turn off the relay
        delay(200);
        digitalWrite(buzzer_Pin, HIGH);
        delay(200);
      }
    }
  } else {
    lcd.clear();
    lcd.print("Seat Belt");
    lcd.setCursor(3, 2);
    lcd.print("not Detected");
    digitalWrite(relay_Pin, LOW); // Turn off the relay
    delay(200);
    digitalWrite(buzzer_Pin, HIGH);
    delay(200);
  }
}

void gps_read() {
  byte a;
  if (Serial.available()) {
    a = Serial.read();
    while (gps.encode(a)) { // encode gps data
      gps.get_position(&lat, &lon); // get latitude and longitude
      Serial.println("Position: ");
      Serial.print("lat:");
      Serial.println((lat * 0.000001), 8);
      Serial.print("log:");
      Serial.println((lon * 0.000001), 8);
    }
  }
}