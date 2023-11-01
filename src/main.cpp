#include <Arduino.h>

#define beepPin 2 
#define usTrig 12
#define usEcho 11
bool beepEnable = false;
int beepPeriod;

#define step1 150
#define step2 100
#define step3 50

#include <Arduino_FreeRTOS.h>
TaskHandle_t distanceRead_Handler;
TaskHandle_t beepSound_Handler;
void distanceRead( void *pvParameters );
void beepSound(void* pvParameters);

#include <NewPing.h>
NewPing sonar(usTrig, usEcho, 400);
int distance;

// #include <HCSR04.h>
// UltraSonicDistanceSensor distanceSensor(12, 13);
// int distance;

// #include <Ultrasonic.h>
// Ultrasonic ultrasonic(usTrig, usEcho);
// int distance;

void setup() {
  Serial.begin(9600);
  pinMode(beepPin, OUTPUT);
  
  xTaskCreate(
    distanceRead
    ,  "Read Ultrasonic Sensor"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL //Parameters passed to the task function
    ,  2  // Priority, with 2 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  &distanceRead_Handler );//Task handle

  xTaskCreate(
    beepSound
    ,  "Control buzzer beep sound"
    ,  128  // Stack size
    ,  NULL //Parameters passed to the task function
    ,  1  // Priority
    ,  &beepSound_Handler );  //Task handle
}

void loop() {

}

void distanceRead(void *pvParameters)
{
  (void) pvParameters;
  for (;;)
  {
    // distance = ultrasonic.read();
    // distance = distanceSensor.measureDistanceCm();
    distance = sonar.ping_cm();
    if (distance <= step1) {
      beepEnable = true;
      if (distance <= step1 && distance > step2) {
        beepPeriod = 1000;
      } else if (distance <= step2 && distance > step3) {
        beepPeriod = 500;
      } else if (distance <= step3) {
        beepPeriod = 0;
      }
    } else {
      beepEnable = false;
    }
    Serial.println(distance);
    vTaskDelay( 200 / portTICK_PERIOD_MS );
  }
}

void beepSound(void *pvParameters)
{
  (void) pvParameters;
  pinMode(LED_BUILTIN, OUTPUT);
  for (;;)
  {
    if (beepEnable) {
      if (!digitalRead(beepPin)) {
        digitalWrite(beepPin, HIGH);  
      } else {
        if (beepPeriod == 0) {
          vTaskDelay( 1 / portTICK_PERIOD_MS );
          continue;
        }
        digitalWrite(beepPin, LOW);  
      }
      vTaskDelay( beepPeriod / portTICK_PERIOD_MS );
    } else {
      digitalWrite(beepPin, LOW);
    }
     
  }
}
