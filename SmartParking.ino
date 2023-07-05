#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);
Servo myservo;

#define gate 10
#define carSensor 9
#define gateSensor1 12
#define gateSensor2 11

unsigned int gatePos = 90;
byte led[5] = {2, 3, 4, 5, 6};
byte sensor[5] = {8, A3, A2, A1, A0};
bool slot[5] = {1, 1, 1, 1, 1};
bool chk = false;
unsigned int aSlot = 0;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 5; i++) {
    pinMode(sensor[i], INPUT);
    pinMode(led[i], OUTPUT);
  }
  pinMode(gateSensor1, INPUT);
  pinMode(gateSensor2, INPUT);
  pinMode(carSensor, INPUT);

  lcd.begin();
  lcd.backlight();
  myservo.attach(gate);
  myservo.write(gatePos);
  lcd.setCursor(0, 0);
  lcd.print("Smart Car");
  lcd.setCursor(0, 1);
  lcd.print("Parking");
  delay(2000);

}

/*
  void loop() {
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == 'a') {
      myservo.write(90);
    } else if (ch == 'b') {
      myservo.write(0);
    }
  }
  Serial.print(digitalRead(sensor[0]));
  Serial.print(digitalRead(sensor[1]));
  Serial.print(digitalRead(sensor[2]));
  Serial.print(digitalRead(sensor[3]));
  Serial.print(digitalRead(sensor[4]));
  Serial.print(digitalRead(gateSensor1));
  Serial.print(digitalRead(gateSensor2));
  Serial.print(digitalRead(carSensor));
  Serial.println("");
  }

*/
void loop() {
  if (digitalRead(carSensor) == 0) {
    chk = true;
  }
  if (chk) {
    aSlot = checkSlot();
    lcd.clear();
    if (aSlot) {
      Serial.println(aSlot);
      lcd.setCursor(0, 0);
      lcd.print("Your slot is");
      lcd.setCursor(0, 1);
      lcd.print(aSlot);
      digitalWrite(led[aSlot - 1], 1);
      delay(200);
      Serial.println("Gate Opening");
      openDoor();
      delay(200);
      while (1) {
        Serial.println("Checking");
        bool chkk = chkEntry();
        if (chkk == true) {
          Serial.println("Entry Found");
          break;
        }
      }
      delay(200);
      closeDoor();
      delay(1000);
      digitalWrite(led[aSlot - 1], 0);
      slot[aSlot - 1] = 0;
      aSlot = 0;
      chk = false;
    } else {
      lcd.setCursor(0, 0);
      lcd.print("No Slot");
      lcd.setCursor(0, 1);
      lcd.print("Come Later");
      delay(1000);
    }
    lcd.clear();
    chk = false;
    lcd.setCursor(0, 0);
    lcd.print("Smart Car");
    lcd.setCursor(0, 1);
    lcd.print("Parking");
  }
}

unsigned int checkSlot() {
  for (int i = 0; i < 5; i++) {
    if (digitalRead(sensor[i])) {
      //            Serial.print(digitalRead(sensor[i]));
      return i + 1;
    }
    //    Serial.println("");
  }
  return 0;
}

void openDoor() {
  myservo.write(0);
}
void closeDoor() {
  myservo.write(90);
}
bool chkEntry() {
  if ((digitalRead(gateSensor1) == 0) && (digitalRead(gateSensor2) == 1)) {
    Serial.println("First");
    while ((digitalRead(gateSensor1) == 0) && (digitalRead(gateSensor2) == 1));
    delay(100);
    if ((digitalRead(gateSensor1) == 0) && (digitalRead(gateSensor2) == 0)) {
      Serial.println("Both");
      while ((digitalRead(gateSensor1) == 0) && (digitalRead(gateSensor2) == 0));
      delay(100);
      if ((digitalRead(gateSensor1) == 1) && (digitalRead(gateSensor2) == 0)) {
        Serial.println("Second");
        while ((digitalRead(gateSensor1) == 1) && (digitalRead(gateSensor2) == 0));
        delay(100);
        if ((digitalRead(gateSensor1) == 1) && (digitalRead(gateSensor2) == 1)) {
          Serial.println("Entered");
          delay(2000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Closing Door");
          Serial.println("Closing");
          return true;
        }
        Serial.println("Came Out 3");
        return false;
      }
      Serial.println("Came Out 2");
      return false;
    }
    Serial.println("Came Out 1");
    return false;
  }
  Serial.println("Came Out 0");
  return false;
}
