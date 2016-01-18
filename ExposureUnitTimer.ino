//
//  Exposure Unit Timer
//

#include "Timer.h"
#include <EEPROM.h>
Timer t;
int button = 2;
int led = 0;
int toggle = 3;
int vSwitch = 1;
unsigned long duration = 1000; // Length of time for exposure. Will get true value from EEPROM.
int address = 0;
unsigned long startTime = 0;
unsigned long now = 0;
unsigned long endTime = 0;
boolean buttonState = LOW; //  LOW is PUSHED
boolean toggleState = LOW;
boolean running = false;
boolean lastButtonState;
boolean justPressed;
boolean timing;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(0, OUTPUT); //  LED
  pinMode(1, OUTPUT); //  12V Switch
  pinMode(2, INPUT); // Button
  pinMode(3, INPUT); //  Toggle
  pinMode(4, OUTPUT); //  Unused
  pinMode(5, INPUT); // was Button

  buttonState = digitalRead(button);
  if (buttonState == LOW) {

    setDuration(); // if button is held down on bootup, calibrate timer.
  }
  EEPROM.get(address, duration);
  //duration = 2000; //test
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led, HIGH);    //  Flash LEDs to show we're alive.
  delay(50);
  digitalWrite(led, LOW);
  delay(50);
  digitalWrite(led, HIGH);
  delay(50);
  digitalWrite(led, LOW);
  buttonState = digitalRead(button);
  while (buttonState == LOW) {    //  Wait for button to be released if now pressed.
    buttonState = digitalRead(button);
  }
  lastButtonState = HIGH;
  toggleState = digitalRead(toggle);
  if (toggleState == HIGH) {
    delay(100);
    running = false;
    timed();
  }

  if (toggleState == LOW) {
    running = false;
    manual();
  }
}

//
void timed() {
  while (toggleState == HIGH) {
    toggleState = digitalRead(toggle);
    buttonState = digitalRead(button);
    if (buttonState == LOW && lastButtonState == HIGH) {
      lastButtonState = LOW;
      startTime = millis();
      endTime = startTime + duration;
      running = true;
      digitalWrite(led, HIGH);
      digitalWrite(vSwitch, HIGH); // turn on 12V
      delay(200);
    }
    now = millis();
    if (running == true && now > endTime) {
      digitalWrite(led, LOW);
      digitalWrite(vSwitch, LOW); // turn off 12V
      running = false;
    }
    if (buttonState == HIGH && lastButtonState == LOW) {
      lastButtonState = HIGH;
      delay(200);
    }
  }
  //Make sure everything is turned off if toggle is flipped during burn
  digitalWrite(led, LOW);
  digitalWrite(vSwitch, LOW); // turn off 12V
}

//
void manual() {
  running = false;
  justPressed = false;
  while (toggleState == LOW) {
    toggleState = digitalRead(toggle);
    buttonState = digitalRead(button);
    if (buttonState == LOW && lastButtonState == HIGH) {
      justPressed = true;
      if (running == true && justPressed == true) { //Turn it OFF
        running = false;
        digitalWrite(led, LOW);
        digitalWrite(vSwitch, LOW); // turn off 12V
        justPressed = false;
      }
      if (running == false && justPressed == true) { //Turn it ON
        running = true;
        digitalWrite(led, HIGH);
        digitalWrite(vSwitch, HIGH); // turn on 12V
        justPressed = false;
      }
      lastButtonState = LOW;
      delay(200);
    }
    if (buttonState == HIGH && lastButtonState == LOW) {
      lastButtonState = HIGH;
      delay(200);
    }
  }
}
//
void setDuration() {
  t.oscillate(led, 100, HIGH);
  while (buttonState == LOW) {  //  Wait for button to be released.
    t.update();
    buttonState = digitalRead(button);
    if (buttonState == HIGH) {
      lastButtonState = HIGH;
      running = true;
      delay(100);
    }
  }
  while (running) {
    buttonState = digitalRead(button);
    t.update();
    if (buttonState == LOW && lastButtonState == HIGH) {    //  If timer started
      lastButtonState = LOW;
      t.stop(led);
      digitalWrite(led, HIGH);
      timing = true;
      startTime = millis();
      delay(200);
      while (timing) {
        buttonState = digitalRead(button);
        if (buttonState == LOW && lastButtonState == HIGH) {  //  If timer is stopped
          lastButtonState = LOW;
          duration = millis() - startTime;
          digitalWrite(led, LOW);
          EEPROM.put(address, duration); //  Burn EEPROM with new duration value;
          timing = false;
          running = false;
          delay(200);
        }
        if (buttonState == HIGH && lastButtonState == LOW) {
          lastButtonState = HIGH;
          delay(200);
        }
      }
    }
    if (buttonState == HIGH && lastButtonState == LOW) {
      lastButtonState = HIGH;
      delay(200);
    }
  }
}
