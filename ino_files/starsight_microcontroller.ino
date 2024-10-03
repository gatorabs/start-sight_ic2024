

// =========================
// STAR-SIGHT CODE 
// 03/01/2024
// Author: Gabriel José Spioni Estevão
// =========================

#include <Servo.h>
#include <Arduino.h>
#include <Ultrasonic.h> // https://github.com/meirarc/Ultrasonic/tree/main

#define TRIGGER_PIN  4
#define ECHO_PIN     5

float msecToCM;

// Objects instances

Servo servo_base, servo_ultrasonic, servo_cam; //servo_base is a continuous rotation servo (360)
                                               // so it follows the control parameters
                                               // 45 - Counter Clock-wise
                                               // 135 - Clock-wise
                                               // 90 - Stop

Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);

long interval_time;
unsigned long start_time;
int calibration_angle;
int ultrasonic_reading;

int actual_servo_position;

bool case_state;

void setup() {

  Serial.begin(115200);

  servo_base.attach(10);
  servo_ultrasonic.attach(9);
  servo_cam.attach(11);

  start_time = millis();
  interval_time = 20;
  
  case_state = true;
}

void loop() {

  unsigned long actual_time = millis(); // don't forget to reset the actual time before the time comparison

  // Time comparation to find the DELTA T
  if(actual_time - start_time > interval_time){
    if(Serial.available() > 0){
      char incomingByte = Serial.read();

      switch (incomingByte){
        
        case '1': //default system, that will only try to find the irregular element

          if(case_state){
            bool irregularObjectFound = false;
          
              for(int i = 50; i < 90; i++){
                incomingByte = Serial.read();

                if(incomingByte == 'E'){ // if the incomingByte be 'E', 
                                        // it means the irregular object was found
                  
                  case_state = false;
                  irregularObjectFound = true;
                  incomingByte = '2';
                  actual_servo_position = i;
                  break;
                }

                main_system_loop(i, 135, 20); 
                
              }
              if (!irregularObjectFound){
                for(int j = 90; j > 50; j--){
                  incomingByte = Serial.read();
                  if(incomingByte == 'E'){ // if the incomingByte be 'E', 
                                            // it means the irregular object was found
                    
                    case_state = false;
                    irregularObjectFound = true;
                    incomingByte = '2';
                    actual_servo_position = j;
                    break;
                  }
                  main_system_loop(j, 135, 20);       

                }
              }
          }
          break;
        case '2':
          main_system_loop(actual_servo_position, 90, 0);
          Serial.println("Distance: ");
          Serial.println(ultrasonic_reading);
          break;
      }
      

    }
      

  }
  

  
  
}

void main_system_loop(int actual_loop_index, int servo_360_state, int calibration_servo_angle){

  ultrasonic_reading = ultrasonic_distance();
  servo_base.write(servo_360_state);
  servo_ultrasonic.write(actual_loop_index);
  servo_cam.write(actual_loop_index + calibration_angle); // you need to find the same angle from servo_ultrasonic
                                                      // after that, you just need to do the calibration calculus
}


int ultrasonic_distance(){
  long microsec = ultrasonic.timing();
  msecToCM = ultrasonic.convert(microsec, Ultrasonic::CM);
  Serial.print(msecToCM);

  return msecToCM;
}

