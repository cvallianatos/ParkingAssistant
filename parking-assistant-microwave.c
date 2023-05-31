// -------------------------------------------------------------------------------------- //
// ParkingAssistant
// Using Microwave Motion Sensor Detector and UltraSonic Sensor HC-SR04 
// by Chris Vallianatos
// May 11, 2023
// -------------------------------------------------------------------------------------- //

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */

#define echoPin 18 // Echo of HC-SR04
#define trigPin 19 // Trig of HC-SR04

#define moveSensorPin 2 // Movement sensor

#define redPin 8
#define yellowPin 9
#define greenPin 10

// define global variables
long duration; // variable for the duration of sound wave travel
float filterArray[20]; // array to store data samples from sensor
float distance; // store the distance from sensor

void setup() 
{
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  pinMode(moveSensorPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
}

void greeting(int x)
{
   for (int i=1; i <= x; i++) 
   {
      digitalWrite(redPin, HIGH);
      digitalWrite(yellowPin, HIGH);
      digitalWrite(greenPin, HIGH);
      delay(500);
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, LOW);
      digitalWrite(greenPin, LOW);
      delay(500);
   }
}

float ultrasonicMeasure() {

  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // measure duration of pulse from ECHO pin
  float duration_us = pulseIn(echoPin, HIGH);

  // calculate the distance
  float distance_cm = 0.017 * duration_us;

  return distance_cm;
}

void parkAssist() 
{ 
   // 1. TAKING MULTIPLE MEASUREMENTS AND STORE IN AN ARRAY
  for (int sample = 0; sample < 20; sample++) {
    filterArray[sample] = ultrasonicMeasure();
    delay(30); // to avoid untrasonic interfering
  }

  // 2. SORTING THE ARRAY IN ASCENDING ORDER
  for (int i = 0; i < 19; i++) {
    for (int j = i + 1; j < 20; j++) {
      if (filterArray[i] > filterArray[j]) {
        float swap = filterArray[i];
        filterArray[i] = filterArray[j];
        filterArray[j] = swap;
      }
    }
  }

  // 3. FILTERING NOISE
  // + the five smallest samples are considered as noise -> ignore it
  // + the five biggest  samples are considered as noise -> ignore it
  // ----------------------------------------------------------------
  // => get average of the 10 middle samples (from 5th to 14th)
  double sum = 0;
  for (int sample = 5; sample < 15; sample++) {
    sum += filterArray[sample];
  }

  distance = sum / 10;
  
  // Depending on the distance recorded the appropriate LED bank is set to ON and the other two are set to OFF
  if (distance < 143)
  {
     digitalWrite(redPin, HIGH);
     digitalWrite(yellowPin, LOW);
     digitalWrite(greenPin, LOW);   
  }
  if (distance > 143 && distance < 160)
  {
     digitalWrite(redPin, LOW);
     digitalWrite(yellowPin, HIGH);
     digitalWrite(greenPin, LOW);   
  }
  if (distance > 160)
  {
     digitalWrite(redPin, LOW);
     digitalWrite(yellowPin, LOW);
     digitalWrite(greenPin, HIGH);   
  }
}

void myTimer() 
{
   uint32_t triggerTime = millis();
   
   while ((millis() - triggerTime) <= 60000)
   {
      parkAssist();
   }
}

void loop() 
{
   if (analogRead(moveSensorPin) > 0)
   {
      greeting(3);
//      myTimer();
   }
   else
   {
      digitalWrite(redPin, LOW);  
      digitalWrite(yellowPin, LOW);
      digitalWrite(greenPin, LOW);
   }

}

int main(void) {

  // -------- Inits --------- //
  DDRB |= 0b00000001;            /* Data Direction Register B:
                                   writing a one to the bit
                                   enables output. */

  // ------ Event loop ------ //
  while (1) {

    PORTB = 0b00000001;          /* Turn on first LED bit/pin in PORTB */
    _delay_ms(1000);                                           /* wait */

    PORTB = 0b00000000;          /* Turn off all B pins, including LED */
    _delay_ms(1000);                                           /* wait */

  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}