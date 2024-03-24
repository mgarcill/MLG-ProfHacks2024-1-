//ProfHacks 2024 - Matthew Garcillano; Victor Martinez

#include <Servo.h>
#include <IRremote.h>

Servo myservo;
int Echo_Pin=A0;  // ultrasonic module   ECHO to A0
int Trig_Pin=A1;  // ultrasonic module  TRIG to A1
int RECV_PIN = 12;
IRrecv irrecv(RECV_PIN);
decode_results results;

int mode = 0;
int autoButtonState = 0;
int manButtonState = 0;

#define IR_Go      0x00ff629d
#define IR_Back    0x00ffa857
#define IR_Left    0x00ff22dd
#define IR_Right   0x00ffc23d
#define IR_Stop    0x00ff02fd

#define IR_Auto    0x00ff42bd
#define IR_Man     0x00ff52ad

#define Lpwm_pin  5     //pin of controlling speed---- ENA of motor driver board
#define Rpwm_pin  6    //pin of controlling speed---- ENB of motor driver board

int pinLB=2;             //pin of controlling turning---- IN1 of motor driver board
int pinLF=4;             //pin of controlling turning---- IN2 of motor driver board
int pinRB=7;            //pin of controlling turning---- IN3 of motor driver board
int pinRF=8;            //pin of controlling turning---- IN4 of motor driver board
volatile int D_mix;
volatile int D_mid;
volatile int D_max;
volatile int Front_Distance;
volatile int Left_Distance;
volatile int Right_Distance;
volatile int Right_IR_Value;
volatile int Left_IR_Value;

float checkdistance() {
  digitalWrite(Trig_Pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_Pin, LOW);
  float distance = pulseIn(Echo_Pin, HIGH) / 58.00;
  delay(10);
  return distance;
}

void Detect_Left_and_Right__distance() {
  myservo.write(180);
  delay(400);
  Left_Distance = checkdistance();
  delay(600);
  Serial.print("Left_Distance:");
  Serial.println(Left_Distance);
  myservo.write(0);
  delay(400);
  Right_Distance = checkdistance();
  delay(600);
  Serial.print("Right_Distance:");
  Serial.println(Right_Distance);
  myservo.write(90);
}

void Ultrasonic_obstacle_avoidance()
{
  Front_Distance=checkdistance(); //obtain the value detected by ultrasonic sensor 
  if((Front_Distance < 20)&&(Front_Distance > 0))//if the distance is greater than 0 and less than 20  
{
    stopp();//stop
    delay(100);
    myservo.write(180);
    delay(500);
    Left_Distance=checkdistance();//measure the distance
    delay(100);
    myservo.write(0);
    delay(500);
    Right_Distance=checkdistance();//measure the distance
    delay(100);
if(Left_Distance > Right_Distance)//if distance a1 is greater than a2
    {
      rotate_left(20);//turn left
      myservo.write(90);
      delay(300); 
    }
    else //if the right distance is greater than the left
    {
      rotate_right(20);// turn right
      myservo.write(90);
      delay(300); 
    }
  }
  else//otherwise
  {
    go_forward(50);//go forward
  }
}


void Obstacle_Avoidance_Main() 
{
  Ultrasonic_obstacle_avoidance();
}



void setup(){
  myservo.attach(A2);
  irrecv.enableIRIn(); // Start the receiver
  Serial.begin(9600);
  D_mix = 10;
  D_mid = 20;
  D_max = 100;
  Front_Distance = 0;
  Left_Distance = 0;
  Right_Distance = 0;
  myservo.write(90);
  pinMode(Echo_Pin, INPUT);      
  pinMode(Trig_Pin, OUTPUT);    
  pinMode(pinLB,OUTPUT); // /pin 2
  pinMode(pinLF,OUTPUT); // pin 4
  pinMode(pinRB,OUTPUT); // pin 7
  pinMode(pinRF,OUTPUT);  // pin 8
  pinMode(Lpwm_pin,OUTPUT);  // pin 5 (PWM) 
  pinMode(Rpwm_pin,OUTPUT);  // pin 6(PWM)   
}

void loop(){

    // Check for mode switch commands from the IR remote
  if (irrecv.decode(&results)) {
    unsigned long key = results.value;
    if (key == IR_Auto) {
      mode = 0; // Switch to automatic mode
      Serial.println("Switching to automatic mode");
    } else if (key == IR_Man) {
      mode = 1; // Switch to manual mode
      Serial.println("Switching to manual mode");
    }
    irrecv.resume(); // Receive the next value
  }

  // Execute corresponding mode functions based on the current mode
  if (mode == 0) {
    // Automatic mode
    Obstacle_Avoidance_Main();
  } 
  else if (mode == 1) {
    // Manual mode
    IR_Control();
  }
}



void go_backward(unsigned char speed_val)    // speed_val：0~255
    {digitalWrite(pinRB,HIGH); 
     digitalWrite(pinRF,LOW);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,LOW);
     analogWrite(Lpwm_pin,speed_val);
     analogWrite(Rpwm_pin,speed_val);
     
      
    }

void go_forward(unsigned char speed_val)    // speed_val：0~255
    {
     digitalWrite(pinRB,LOW);  
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,LOW);  
     digitalWrite(pinLF,HIGH);
     analogWrite(Lpwm_pin,speed_val);
     analogWrite(Rpwm_pin,speed_val);
    }
    
void rotate_right(unsigned char speed_val)        // speed_val：0~255
    {digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,LOW );  
     digitalWrite(pinLB,LOW); 
     digitalWrite(pinLF,HIGH);
     analogWrite(Lpwm_pin,speed_val);
     analogWrite(Rpwm_pin,speed_val);
      
     
    }
void rotate_left(unsigned char speed_val)    // speed_val：0~255
    {
      digitalWrite(pinRB,LOW);  
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,LOW);  
     analogWrite(Lpwm_pin,speed_val);
     analogWrite(Rpwm_pin,speed_val);
     
    }    
void stopp()        //stop
    {
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,HIGH);
    }

void IR_Control(void) //manual control
{
   unsigned long Key;
   if(irrecv.decode(&results)) //judging if serial port receives data   
 {
     Key = results.value;
    switch(Key)
     {
       case IR_Go:go_forward(150);   //UP
       break;
       case IR_Back:go_backward(150);   //back
       break;
       case IR_Left:rotate_left(100);   //Left    
       break;
       case IR_Right:rotate_right(100); //Righ
       break;
       case IR_Stop:stopp();   //stop
       break;
       default: 
       break;      
     } 
     irrecv.resume(); // Receive the next value
    } 
  
}

void IR_Mode(void)
{
  unsigned long Key;
   if(irrecv.decode(&results)) //judging if serial port receives data   
 {
     Key = results.value;
    switch(Key)
     {
       case IR_Auto:mode = 0;   
       break;
       case IR_Man:mode = 1;   
       break;
       default: 
       break;      
     }
     irrecv.resume();
}
}