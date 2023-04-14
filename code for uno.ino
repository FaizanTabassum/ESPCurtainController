int buttonlo = 7;
int buttonlc = 6;
int buttonro = 4;
int buttonrc = 8;
const int CCW = 2; // do not change
const int CW  = 1; // do not change
#include <Robojax_L298N_DC_motor.h>
#define motor 12
#define CHA 0
#define ENA A0 // this pin must be PWM enabled pin if Arduino board is used
#define IN1 13
#define IN2 12

// motor 2 settings
#define IN3 11
#define IN4 10
#define ENB A1// this pin must be PWM enabled pin if Arduino board is used
#define CHB 1

#define motor1 1 // do not change
#define motor2 2 // do not change
Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);
void setup() {
  Serial.begin(9600);
  robot.begin();
  pinMode(motor,OUTPUT);
  pinMode(9,INPUT);//ro
  pinMode(8,INPUT);//rc
  pinMode(7,INPUT);//lo
  pinMode(6,INPUT);//lc
  pinMode(5,INPUT);

}

void loop() {
  while(digitalRead(9) == HIGH){
     if((digitalRead(buttonrc) == LOW) || (digitalRead(buttonlc) == LOW)){
            if(digitalRead(buttonlc) == LOW ){robot.rotate(motor1, 100, CW);}
            if(digitalRead(buttonlc) == HIGH ){robot.brake(1);}
            
            if(digitalRead(buttonrc) == LOW ){robot.rotate(motor2, 100, CW);}
            if(digitalRead(buttonrc) == HIGH ){robot.brake(2);}
            
            }

  }
 while(digitalRead(9) == LOW){
   if((digitalRead(buttonro) == LOW) || (digitalRead(buttonlo) == LOW)){
            if(digitalRead(buttonlo) == LOW ){robot.rotate(motor1, 100, CCW);}
            if(digitalRead(buttonlo) == HIGH ){robot.brake(1);}
            
            if(digitalRead(buttonro) == LOW ){robot.rotate(motor2, 100, CCW);}
            if(digitalRead(buttonro) == HIGH ){robot.brake(2);}
            
            }
 }

}
