/*
This program simulates a home alarm/security system.


A keypad (pass: 1234) is used to switch between the system's "armed" and "disarmed" states, as indicated by RGB LED's colour and the LCD display.


In its disarmed state, an RGB LED is lit green, by pressing a button the user is able to toggle the display to view the last detection by the system (from its armed state).


In its armed state, an RGB LED is lit red, a PIR sensor and ultrasonic distance sensor detects and measures the distance of motion. If motion is detected, a buzzer will beep
and the RGB LED will alternatively switch from blue to white (the interval of this depends on the distance of the motion).


It is recommended to use a wide, opaque object when testing this system for the most accurate results (PIR sensor detection).


Code written by Megan Lee.
*/


#include <LiquidCrystal.h> //LCD Library
#include <Keypad.h> //Keypad Library


//defining variables of components & assigns pin #s
const int trigPin = 32;
const int echoPin = 33;
const int pirPin = 34;
const int toggleBtn = 38;
const int buzzerPin = 39;
const int redPin = 35;
const int greenPin = 36;
const int bluePin = 37;


//length of the password set to 4 (only the first 4 values inputted to keypad are received & printed)
const int passwordLength = 4;


//setting up LCD shield (pin #s)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


//setting up 4X4 Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};


//assigning pin #s for keypad
byte rowPins[ROWS] = {22, 24, 26, 28};
byte colPins[COLS] = {25, 27, 29, 31};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


//defines variables for values for functions
long duration; //variable for distance sensor's input
long distance; //variable for distance sensor's input (in cm)
int pirState = 0; //variable to check state of PIR sensor
int alarmState = 0; //variable to differentiate between "armed" and "disarmed" state
int displayState=1; //variable to toggle LCD display during "disarmed" state
int buttonState=1; //variable to check state of the toggle button
int interval; //variable to control the interval of flashes of the alarm
int buzzerLength; //variable to control the interval of beeps of the alarm
String password = "1234"; //keypad's password set to 1234
String enteredPassword = ""; //variable for keypad input


//set ups and determines INPUT/OUTPUT of all components
void setup(){
  lcd.begin(16,2);
  lcd.clear();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(toggleBtn, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}


//function to check keypad's input
void checkPassword(){
  char key = keypad.getKey();
  if (key>0){ //if keypad received input
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter password:"); //asks user to enter password on LCD display
    enteredPassword += key; //receives user input on the keypad
    lcd.setCursor(0, 1);
    lcd.print("                "); //clears second row of LCD display
    lcd.setCursor(0, 1);
    lcd.print(enteredPassword); //prints the inputted password by user
    delay(750);
    } if (enteredPassword.length() == passwordLength){ //if four values were inputted as the password
        if (enteredPassword == password){ //if password is correct
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Access granted"); //tells user that password was correct on LCD display
          delay(1000);
          lcd.clear();
          alarmState = !alarmState; //toggles alarm state (can switch from "disarmed" to "armed", vice versa)
          enteredPassword=""; //resets variable for user's password input
        } else{ //if password is incorrect (doesn't match "1234")
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Access denied"); //tells user that password was wrong on LCD display
            lcd.setCursor(0,1);
            lcd.print("Try Again.");
            delay(1000);
            lcd.clear();
            enteredPassword = ""; //resets variable for user's password input
        }
      }
}


//function to check distance of the detected motion
void checkDistance(){
  digitalWrite(trigPin, LOW); //initiate the sensor and
  delayMicroseconds(2);       //transmit the ultrasonic burst
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  //measure the time duration in microseconds
  duration = pulseIn(echoPin, HIGH); //
  distance = duration * 0.034 / 2; //converts distance sensor's input to cm
  distance = constrain(distance, 0, 300);  //constrains distance detected to only be within 0-300 (PIR sensor's detection limit)
}


//function to check state of PIR sensor
void checkMotion(){
  pirState = digitalRead(pirPin);
  delay(10);
}


//function for the "alarm"
void motionDetected(){
  interval = distance*100;  //sets a variable for interval of flashes/beeps
  buzzerLength= interval/2; //depending on the distance value
  tone(buzzerPin, 850, buzzerLength); //buzzer beeps
  digitalWrite(redPin, 255); //RGB LED turns white
  digitalWrite(greenPin, 255);
  digitalWrite(bluePin, 255);
  delay(interval);
  tone(buzzerPin, 850, buzzerLength);
  digitalWrite(redPin, 0);//RGB LED turns blue
  digitalWrite(greenPin, 0);
  digitalWrite(bluePin, 255);
  delay(interval);
}


//function to disable RGB LED and buzzer when no motion is detected
void noMotion(){
  noTone(buzzerPin);
  digitalWrite(redPin, 255);
  digitalWrite(greenPin, 0);
  digitalWrite(bluePin, 0);
}


//function to check state of the toggle button (during disarmed state)
void checkButton(){
  buttonState=digitalRead(toggleBtn);
  delay(50);
  if (buttonState==0){
    displayState=displayState*-1; //toggles display between showing system's status (+ve value) and last detection (-ve value)
  }
}
void loop(){
    if(alarmState==0){ //if system is disarmed
      digitalWrite(redPin, 0);
      digitalWrite(greenPin, 255); //lit green RGB LED
      digitalWrite(bluePin, 0);
      checkPassword(); //calls function to check for input to keypad
      if (enteredPassword==""){ //if there's no input to the keypad
        checkButton(); //calls function to check state of toggle button
        delay(50);
        if (displayState<0){
          lcd.setCursor(0,0);
          lcd.print("Last Detection: "); //printing last detection
          lcd.setCursor(0,1);
          if (distance==0){  //if no motion was detected yet (ex. system just started)
            lcd.print("N/A"); //printing N/A to LCD
          } else{
            lcd.print(distance); //if system has input from distance sensor
            lcd.print(" cm"); //distance value is in cm
          }
        } else{ //if toggle button has not been pressed (or been switched back)
          lcd.setCursor(0,1);
          lcd.print("     ");
          lcd.setCursor(0,0);
          lcd.print("System: Disarmed"); //printing status of system (Disarmed)
        }
      }
    }
    if (alarmState==1){ //if system is armed
      digitalWrite(redPin, 255); //lit red RGB LED
      digitalWrite(greenPin, 0);
      digitalWrite(bluePin, 0);
      checkPassword();
      if (enteredPassword==""){ //if there's no input to the keypad
        lcd.setCursor(0,1);
        lcd.print("         ");
        lcd.setCursor(0,0);
        lcd.print("System: Armed   "); //printing status of system (Armed)
        checkMotion();
        delay(50);
      if (pirState==1){ //if motion was detected by PIR sensor
        checkDistance(); //calls function to check distance of said motion
        motionDetected(); //calls function for the alarm
      } } if (pirState==0){ //if motion was not detected by PIR sensor
        noMotion(); //calls function for no alarm
      }
    }
}


