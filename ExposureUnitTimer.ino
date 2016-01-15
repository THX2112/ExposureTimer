//
//  Exposure Unit Timer
//
#include "Timer.h"
#include <Button.h>
#include <EEPROM.h>

Timer t;
Button button = Button(3,BUTTON_PULLUP);

//int button = 3;
int led = 0;
int toggle = 2;
int vSwitch = 1;
unsigned long duration = 1000; // Length of time for exposure. Will get true value from EEPROM.
int address = 0;
unsigned long startTime=0;
unsigned long now=0;
unsigned long endTime=0;
boolean buttonState = LOW; //  LOW is PUSHED
boolean toggleState = LOW;
boolean running = false;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(0, OUTPUT); //  LED
  pinMode(1, OUTPUT); //  12V Switch
  pinMode(2, INPUT); // Toggle
  pinMode(3, INPUT); //  Button
  pinMode(4, OUTPUT); //  Unused
  pinMode(5, INPUT); // was Button

  //buttonState = digitalRead(button);
  if (button.isPressed()) {
    t.oscillate(led, 100, HIGH);
    setDuration(); // if button is held down on bootup, calibrate timer.
  }
  EEPROM.get(address, duration);
  //duration = 2000; //test
}

// the loop routine runs over and over again forever:
void loop() {
//t.stop();
digitalWrite(led, HIGH);
delay(50);
digitalWrite(led, LOW);
delay(50);
digitalWrite(led, HIGH);
delay(50);
digitalWrite(led, LOW);
  //t.update();
  toggleState = digitalRead(toggle);

  if (toggleState == HIGH) {
    running=false;
    //t.stop(led);
    //t.oscillate(led, 500, HIGH);
    timed();
  }

  if (toggleState == LOW) {
    running=false;
    //t.stop(led);
    //t.oscillate(led, 1000, HIGH);
    manual();
  }
}

//
void timed() {
  while (toggleState == HIGH) {
    toggleState = digitalRead(toggle);
    t.update();
    now=millis();
    if (button.uniquePress() && running==false) {  //Turn it 
      startTime=millis();
      endTime=startTime+duration;
      running=true;
      t.stop(led);
      digitalWrite(led, HIGH);
      digitalWrite(vSwitch,HIGH); // turn on 12V      
    }
    if (running==true && now>endTime){
      running=false;
      digitalWrite(led, LOW);
      digitalWrite(vSwitch,LOW); // turn off 12V
    }

    
  }

}

//
void manual() {

  while (toggleState == LOW) {
    toggleState = digitalRead(toggle);
    t.update();
    if (button.uniquePress() && running==false) {  //Turn it ON
      running=true;
      t.stop(led);
      digitalWrite(led, HIGH);
      digitalWrite(vSwitch,HIGH); // turn on 12V
    }
    if (button.uniquePress() && running==true) {  //Turn it OFF
      running=false;
      digitalWrite(led, LOW);
      digitalWrite(vSwitch,LOW); // turn off 12V
    }

  }
}
//
void setDuration() {
  running=true;
  while (running) {
    t.update();
    if (button.uniquePress()) {
      t.stop(led);
      digitalWrite(led, HIGH);
      bool timing = true;
      startTime = millis();
      while (timing) {
        if (button.uniquePress()) {
          duration = millis() - startTime;
          digitalWrite(led, LOW);
          timing = false;
        }
      }
      EEPROM.put(address, duration); //  Burn EEPROM with new duration value;
      running=false;
    }
  }
}

