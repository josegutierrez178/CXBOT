// Pins for QTI connections on board
#include <Servo.h>
#include <SoftwareSerial.h>

#define lineSensor1 49
#define lineSensor2 51
#define lineSensor3 53
#define redpin 45
#define greenpin 46
#define bluepin 44
#define TxPin 14

SoftwareSerial mySerial = SoftwareSerial(255, TxPin);

const int pingPin = 50;
int redRGB = A0;
int greenRGB = A1;
int blueRGB = A2;

char position;

bool leftsen = true;
bool midsen = true;
bool rightsen = true;

Servo servoLeft;
Servo servoRight;

int count;
int numbers[5] = {-1, -1, -1, -1, -1};

// 4-minute timer
unsigned long botStartTime = 0;
bool fourMinuteActionDone = false;
const unsigned long FOUR_MINUTES = 240000;   // 4 minutes


void setup() {
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  pinMode(redRGB, OUTPUT);
  pinMode(greenRGB, OUTPUT);
  pinMode(blueRGB, OUTPUT);

  analogWrite(redpin, 255);
  analogWrite(greenpin, 255);
  analogWrite(bluepin, 255);

  setColor(255, 255, 255);

  servoLeft.attach(12);
  servoRight.attach(11);

  Serial.begin(9600);
  Serial2.begin(9600);
  mySerial.begin(9600);

  count = 0;

  delay(100);
  mySerial.write(12);   // clear
  delay(10);
  mySerial.write(22);   // no cursor no blink
  delay(10);
  mySerial.write(17);   // backlight
  delay(10);

  delay(14000);

  botStartTime = millis();
}

void loop() {
  // 4-minute timer check
 
 if (!fourMinuteActionDone && millis() - botStartTime >= FOUR_MINUTES) { // 4-minute timer check
fourMinuteActionDone = true;

  // Replace all -1 values with 2
  for (int i = 0; i < 5; i++) {
    if (numbers[i] == -1) {
      numbers[i] = 2;
    }
  }

  // Recalculate score after replacement
  int score = sum_array(numbers, 5);

  // Redisplay updated array
  mySerial.write(12);
  delay(5);
  for (int i = 0; i < 5; i++) {
    mySerial.print(numbers[i]);
    mySerial.print(" ");
  }

  mySerial.write(148);
  mySerial.print(score);

  final_task(score); //runs the final task

  while (true) {
    // stop forever
  }
 }

  int qti1 = rcTime(lineSensor1);
  int qti2 = rcTime(lineSensor2);
  int qti3 = rcTime(lineSensor3);

  leftsen  = (qti1 < 275);
  midsen   = (qti2 < 80);
  rightsen = (qti3 < 150);

  int state = 4 * leftsen + 2 * midsen + rightsen; //calculates the state for the QTIs

  switch (state) {
    case 0://when all three QTIs read black
      count++; //counts what hash we are at

      if (count == 10) {
        servoLeft.writeMicroseconds(1700); //rolls forward to align with object
        servoRight.writeMicroseconds(1300);
        delay(250);

        servoLeft.detach(); //detatch the wheels
        servoRight.detach();

        float inches = getDistanceInches(pingPin);// Run Ping sensor
        Serial.println(inches);

        if (inches < 5.0) {
          setColor(255, 0, 255); //flash green
          delay(250);
          setColor(255, 255, 255);

          numbers[3] = position % 10; //calculate the value of the spot of our object

          // for (int i = 0; i < 5; i++) {
          //   Serial.print(numbers[i]); 
          // }

          if (Serial.available() > 0) {
            mySerial.write(12);
            delay(5);
            for (int i = 0; i < 5; i++) {
              mySerial.print(numbers[i]); //displays this on the first line of the LCD
            }
          }

          bool hasNegativeOne = true;


          while (hasNegativeOne && millis() < FOUR_MINUTES) { //loops if it's under 4 minuts and the array has a negative one
            Serial2.print(position);

            setColor(255, 255, 0); 
            delay(250);
            setColor(255, 255, 255);
            delay(250);

            if (Serial2.available()) {
              char incoming = Serial2.read();

              setColor(0, 0, 0);
              delay(250);
              setColor(255, 255, 255);

              numbers[(incoming - 50) / 10] = incoming % 10;

              for (int i = 0; i < 5; i++) {
                Serial.print(numbers[i]);
              }

              mySerial.write(12);
              delay(5);
              for (int i = 0; i < 5; i++) {
                mySerial.print(numbers[i]);
                mySerial.print(" ");
              }

              mySerial.write(148);
              mySerial.print(sum_array(numbers, 5));

              hasNegativeOne = false;
              for (int i = 0; i < 5; i++) {
                if (numbers[i] == -1) {
                  hasNegativeOne = true;
                }
              }
            }
          }

          final_task(sum_array(numbers, 5)); //calculates the final score of all the points added together and performs either a song or a light show
          delay(100000000); //Keeps the LCD board on
        }
        else {
          position = 'P'; //Detects no object at the end hash

          setColor(0, 255, 255); //flashes red, no object detected
          delay(250);
          Serial2.print('P'); 
          setColor(255, 255, 255);

          numbers[3] = position % 10; //calculates our own value for the array

          // for (int i = 0; i < 5; i++) {
          //   Serial.print(numbers[i]);
          // }

          if (Serial.available() > 0) {
            mySerial.write(12);
            delay(5);
            for (int i = 0; i < 5; i++) {
              mySerial.print(numbers[i]); //prints out the values in the array on the LDC
            }
          }

          bool hasNegativeOne = true;

          while (hasNegativeOne && millis() < FOUR_MINUTES) { //loops if there's a negative 1 in the array and if it's under 4 minutes
            Serial2.println(position);

            setColor(255, 255, 0); //flash blue indicating signal transmitted
            delay(250);
            setColor(255, 255, 255);
            delay(250);

            if (Serial2.available()) {
              char incoming = Serial2.read();

              setColor(0, 0, 0); //flash white indicating signal being read
              delay(250);
              setColor(255, 255, 255);

              numbers[(incoming - 50) / 10] = incoming % 10; //calculates the position and the value of the incoming value

              mySerial.write(12);
              delay(5);
              for (int i = 0; i < 5; i++) {
                mySerial.print(numbers[i]);//prints out the updated array on the LDC
                mySerial.print(" "); 
              }

              mySerial.write(148);
              mySerial.print(sum_array(numbers, 5)); //Prints out the sum on the second line of the LDC

              hasNegativeOne = false; //clears boolean for having negative one
              for (int i = 0; i < 5; i++) {
                if (numbers[i] == -1) { //checks if there's a -1 in the array
                  hasNegativeOne = true; //sets it back to false to keep the while loop running
                }
              }
            }
          }

          final_task(sum_array(numbers, 5)); //calculates and runs the celebration when all 4 digits are filled in. 
          delay(100000000);
        }
        
  

      }

      if (count == 6) {
        Serial.print("sixth hash detected");
        servoLeft.writeMicroseconds(1600); //turns to the right
        servoRight.writeMicroseconds(1600);
        delay(550);
      }

      if (count == 1) {
        analogWrite(redpin, 0); //flashes purple
        analogWrite(bluepin, 0);

        servoLeft.writeMicroseconds(1600);//rolls forward
        servoRight.writeMicroseconds(1300);
        delay(250);

        servoLeft.writeMicroseconds(1500); //stops
        servoRight.writeMicroseconds(1500);
        delay(1000);

        float inches = getDistanceInches(pingPin); //reads ping sensor
        if (inches < 3.0) {
          Serial.println(inches);
          setColor(255, 0, 255); //flash green
          delay(250);
          position = 'Q'; //saves the position value
          setColor(255, 255, 255);
        } else {
          setColor(0, 255, 255);
          delay(250); //flash red
          setColor(255, 255, 255);
        }

        analogWrite(redpin, 255); //turns off
        analogWrite(bluepin, 255);

        Serial.println(count);
      }

      if (count == 2) {
        analogWrite(greenpin, 0);//turns on green

        servoLeft.writeMicroseconds(1690);//goes straight
        servoRight.writeMicroseconds(1300);
        delay(250);

        analogWrite(greenpin, 255); //turns off

        servoLeft.writeMicroseconds(1500); //stops
        servoRight.writeMicroseconds(1500);
        delay(1000);

        float inches = getDistanceInches(pingPin); //gets data from ping sensor
        if (inches < 3.0) {
          Serial.println(inches);
          setColor(255, 0, 255); //flash green
          delay(250);
          position = 'R'; //saves position value
          setColor(255, 255, 255);
        } else {
          setColor(0, 255, 255); //flash red
          delay(250);
          setColor(255, 255, 255);
        }

        Serial.println(count);
      }

      if (count == 3) {
        analogWrite(bluepin, 0); //flash blue

        servoLeft.writeMicroseconds(1690); // rolls forward
        servoRight.writeMicroseconds(1300);
        delay(250);

        analogWrite(bluepin, 255); //turns off blue

        servoLeft.writeMicroseconds(1500); //stops
        servoRight.writeMicroseconds(1500);
        delay(1000);

        float inches = getDistanceInches(pingPin); //reads ping sensor
        if (inches < 3.0) {
          Serial.println(inches);
          setColor(255, 0, 255); //flash green
          delay(250);
          position = 'S'; //saves value
          setColor(255, 255, 255);//turns off
        } else {
          setColor(0, 255, 255); //flash red
          delay(250);
          setColor(255, 255, 255);//turns off
        }

        Serial.println(count);
      }

      if (count == 4) {
        analogWrite(redpin, 0); //flash red

        servoLeft.writeMicroseconds(1690); //rolls forward
        servoRight.writeMicroseconds(1300);
        delay(250);

        analogWrite(redpin, 255);

        servoLeft.writeMicroseconds(1500); //stops
        servoRight.writeMicroseconds(1500);
        delay(500);

        float inches = getDistanceInches(pingPin);//reads Ping sensor
        if (inches < 3.0) {
          Serial.println(inches);
          setColor(255, 0, 255);//flash green
          delay(250);
          position = 'T';//stores value
          setColor(255, 255, 255);
        } else {
          setColor(0, 255, 255);//flash red
          delay(250);
          setColor(255, 255, 255);
        }

        Serial.println(count);
      }

      if (count == 5) {
        Serial.println(count);

        analogWrite(redpin, 0);
        analogWrite(greenpin, 0);//flash white
        analogWrite(bluepin, 0);

        servoLeft.writeMicroseconds(1700);//roll forward
        servoRight.writeMicroseconds(1300);
        delay(250);

        servoLeft.writeMicroseconds(1500);//stop
        servoRight.writeMicroseconds(1500);
        delay(1000);

        float inches = getDistanceInches(pingPin);//gets data from ping sensor
        if (inches < 3.0) {
          Serial.println(inches);
          setColor(255, 0, 255); //flash green
          delay(250);
          position = 'U';
          setColor(255, 255, 255);
        } else {
          setColor(0, 255, 255);
          delay(250); // flash red
          setColor(255, 255, 255);
        }

        analogWrite(redpin, 255); //flash white
        analogWrite(greenpin, 255);
        analogWrite(bluepin, 255);

        servoLeft.writeMicroseconds(1700);//roll forward
        servoRight.writeMicroseconds(1300);
        delay(200);
      }
      else {
        servoLeft.writeMicroseconds(1500);//stay still
        servoRight.writeMicroseconds(1500);
        delay(200);

        servoLeft.writeMicroseconds(1700);//roll forward
        servoRight.writeMicroseconds(1300);
        delay(200);

        Serial.println(count);
      }
      break;

    case 1: //dark dark light, line on left, moves slightly left
      servoLeft.writeMicroseconds(1500); 
      servoRight.writeMicroseconds(1425);
      break;

    case 2: //highly unlikley case DLD
      break;

    case 3: // DLL light on left, move left
      servoLeft.writeMicroseconds(1500);
      servoRight.writeMicroseconds(1425);
      break;

    case 4: // LDD line on right, move slightly right
      servoLeft.writeMicroseconds(1600);
      servoRight.writeMicroseconds(1500);
      break;

    case 5: //Centered on line, LDL
      servoLeft.writeMicroseconds(1700);
      servoRight.writeMicroseconds(1300);
      break;

    case 6: // LLD line on the right, move right
      servoLeft.writeMicroseconds(1625);
      servoRight.writeMicroseconds(1500);
      break;

    case 7: //completly off line
      servoLeft.writeMicroseconds(1500); // stops
      servoRight.writeMicroseconds(1500);
      if (count == 5) {
        servoLeft.writeMicroseconds(1700); //move forward
        servoRight.writeMicroseconds(1300);
        Serial.print(count); //here for debugging
      }
      break;
  }
}

// Defines function 'rcTime' to read value from QTI sensor
long rcTime(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(1);
  pinMode(pin, INPUT);
  digitalWrite(pin, LOW);

  long time = micros();
  while (digitalRead(pin));
  time = micros() - time;

  return time;
}

float microsecondsToInches(long microseconds) {
  return microseconds / 74.074 / 2;
}

// float microsecondsToCentimeters(long microseconds) {
//   return microseconds / 29.155 / 2.0;
// }

void setColor(int red, int green, int blue) {
  analogWrite(redRGB, red);
  analogWrite(greenRGB, green);
  analogWrite(blueRGB, blue);
}

float getDistanceInches(int pingPin) {
  long duration;

  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  return microsecondsToInches(duration);
}

int sum_array(int arr[], int size) {
  int sum = 0;
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  return sum;
}


void final_task(int score) {
  if (score >= 10) {
    light_show();
  }
  if (score < 10) {
    playSong();
  }
}

void light_show() {
  for (int i = 0; i <= 5; i++) {
    setColor(0, 255, 255); //red
    delay(500);

    setColor(0, 150, 255);
    delay(500);

    setColor(0, 50, 255);
    delay(500);

    setColor(255, 0, 255);
    delay(500);

    setColor(255, 0, 100);
    delay(500);

    setColor(255, 255, 0);
    delay(500);

    setColor(0, 255, 0);
    delay(500);
  }
}

int durs[] = {
  212, 212, 212, 211, 211,
  212, 211, 211, 213,
  212, 212, 212, 211, 211,
  212, 211, 211, 213
};

int octs[] = {
  216, 216, 216, 215, 216,
  216, 215, 216, 216,
  216, 216, 216, 216, 216,
  215, 215, 216, 216
};

int notes[] = {
  220, 220, 220, 228, 223,
  220, 228, 223, 220,
  227, 227, 227, 228, 223,
  231, 228, 223, 220
};

int numNotes = 18;

void playSong() {
  for (int i = 0; i < 2; i++) {
    for (long k = 0; k < numNotes; k++) {
      mySerial.write(durs[k]);
      mySerial.write(octs[k]);
      mySerial.write(notes[k]);

      int len = 214 - durs[k];
      float del = 2000 / pow(2, len);
      delay(int(del * 1.1));
    }
  }
}