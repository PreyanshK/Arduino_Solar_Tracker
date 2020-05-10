#include <Servo.h> // include Servo library 
#include <SoftwareSerial.h> // TX RX software library for bluetooth

#define horizontal_servo 8
#define vertical_servo 9
#define pushButton_Pin 6

//---- Servo Motor pin connections + Initial Setup ----
Servo horizontal; // horizontal servo
int servoH = 180;  //Variable

int servoH_LimitHigh = 180;  //MAX angle for Horizontal servo
int servoH_LimitLow = 65;    //MIN angle for Horizonatal servo

Servo vertical;   // vertical servo 
int servoV = 10;    //   90;     // stand vertical servo

int servoV_LimitHigh = 50;  //MAX angle for Vertical servo
int servoV_LimitLow = 5;    //MIN angle for Vertical servo

//---- Photoresistor pin connections ----
int photoTL = 0; //Photoresistor on Top LEFT 
int photoBL = 1; //Photoresistor on Bottom LEFT 
int photoTR = 2; //Photoresistor on Top RIGHT
int photoBR = 3; //Photoresistor on Bottom RIGHT

//---- Bluetooth pin connections ----
int bluetoothTX = 10; // bluetooth TX pin
int bluetoothRX = 11; // bluetooth RX pin

SoftwareSerial bluetooth(bluetoothTX, bluetoothRX);
/*
int tl = 0; // Stores top left Photoresistor readings
int tr = 0; // Stores top right photoresistor readings
int bl = 0; // Stores bottom left photoresistor readings
int br = 0; // Store bottom right photoresistor readings
  
int avgtop = 0; // average of top-side photoresistor readings
int avgbot = 0; // average of bottom-side photoresistor readings
int avgleft = 0; // average of left-side photoresistor readings
int avgright = 0; // average of right-side photoresistor readings

int diffvert = 0; // difference of Average of top and bottom
int diffhoriz = 0; //difference of Average of left and right
*/
int delaytime = 10; //delaytime for every cycle - 10 milli seconds
int tol = 50;       //tolerance - if the difference is within tolerance NO rotating is necessary, stay still 
               
int pb = 0;   //stores pushbutton value 
int mode = 0; //indicates mode -> 1 = automatic; 2 = manual 

void setup() {
  //Setup usb serial connection to computer
  Serial.begin(9600);

  //Setup Bluetooth serial connection to android
  bluetooth.begin(9600);
  
  horizontal.attach(horizontal_servo); //Horizontal Servo motor variable for dynamic mode
  vertical.attach(vertical_servo);     //Vertical Servo motor variable for dynamic mode
 
  horizontal.write(90);  //Initial angle for horizontal servo
  vertical.write(30);    //Initial angle for vertical servo
  delay(3000);
}

void loop() {
  pb = digitalRead(pushButton_Pin);  //Switching between MODE 0 and MODE 1 using pushbutton
  if (pb == 0){
    
    if (mode == 0) 
    {
      mode = 1;
    } 
    
    else {
      mode = 2;
    }
    do {
      pb = digitalRead(pushButton_Pin);
    } while (pb == 0);
  }
   
  Serial.print("Mode: ");
  Serial.println(mode);
      
  if (mode == 0) //MODE 1: AUTO Mode - Use Light Sensors to detect light source
  {
    int tl = analogRead(photoTL);
    int tr = analogRead(photoTR); 
    int bl = analogRead(photoBL); 
    int br = analogRead(photoBR); 
  
    int avgtop = (tl + tr) / 2;   //Finding the averages of 4 directions: 1. TOP
    int avgbot = (bl + br) / 2;   //2. BOTTOM
    int avgleft = (tl + bl) / 2;  //3. LEFT
    int avgright = (tr + br) / 2; //4. RIGHT

    int diffvert = avgtop - avgbot;     // check the difference of top and bottom
    int diffhoriz = avgleft - avgright; // check the difference of left and right
  
    Serial.print("AvgTop: "); //Debugging purposes - checking for photoresistor readings
    Serial.print(avgtop);
    Serial.print("  AvgBot: ");
    Serial.print(avgbot);
    Serial.print("  AvgLeft: ");
    Serial.print(avgleft);
    Serial.print("  AvgRight: ");
    Serial.println(avgright);
  
    //------------------------ VERTICAL MOTION ------------------------  
    if (-1*tol > diffvert || diffvert > tol) // check if the difference is in the tolerance else change vertical angle
  
    {
      if (avgtop > avgbot)  //If the top lighting is > bottom lighting rotate vertical motor "UP"
      {
        servoV = ++servoV;  //"Increasing angle by 1" for vertical motor, i.e. "rotating up"
      
        if (servoV > servoV_LimitHigh) //If the new angle is GREATER than MAX of vertical motor
        { 
          servoV = servoV_LimitHigh; // STOP rotating because new angle is overwritten as MAX 
        }
      }
    
      else if (avgtop < avgbot) //If the bottom lighting is > top lighting rotate vertical motor "DOWN"
      {
        servoV = --servoV; //"Decrease angle by 1" of vertical motor, i.e. "rotating down"
      
        if (servoV < servoV_LimitLow) //If the new angle is LESS than MIN of vertical motor
        {
          servoV = servoV_LimitLow; //STOP rotating because new angle is overwritten as MIN
        }
      }

      vertical.write(servoV); //Rotate Vertical Motor to "NEW" angle
    }

    //------------------------ HORIZONTAL MOTION ------------------------
    if (-1*tol > diffhoriz || diffhoriz > tol) // check if the difference is in the tolerance else change horizontal angle
  
    {
      if (avgright > avgleft) //If Right lighting is > Left lighting rotate horizontal motor "RIGHT"
      {
        servoH = ++servoH; //Increasing angle by "1" of horizontal motor, i.e. "rotating right" 
      
        if (servoH > servoH_LimitHigh) //If the new angle is GREATER than MAX of horizontal motor
        {
          servoH = servoH_LimitHigh; //STOP rotating because new angle is overwritten as MAX
        }
      }
    
      else if (avgright < avgleft) //If the Left lighting is > Right lighting, rotate horizontal motor "LEFT"
      {
        servoH = --servoH; //Decreasing angle by "1" of horizontal motor, i.e. "rotating left" 
      
        if (servoH < servoH_LimitLow) //If the new angle is LESS than MIN of horizontal motor
        {
          servoH = servoH_LimitLow; //STOP rotating because new angle is overwritten as MIN
        }
      }
   
      horizontal.write(servoH);//Rotate Horizontal Motor to "NEW" angle
    }
    delay(delaytime);
  }

  else //MODE 2: Manual Mode: Remote Control via Bluetooth
  {
      if(bluetooth.available()>= 2 )
      {
        unsigned int servopos = bluetooth.read(); 
        unsigned int servopos1 = bluetooth.read();
        unsigned int realservo = (servopos1 *256) + servopos;
        Serial.println(realservo);
 
        if (realservo >= 1000 && realservo <1180) {
          int servo1 = realservo;
          servo1 = map(servo1, 1000, 1180, 0, 180);
          if (servo1 < 0)
          {
            servo1 = 0;
          }
          horizontal.write(servo1);
          delay(delaytime);
        }
        
        if (realservo >= 2000 && realservo <2180) {
          int servo2 = realservo;
          servo2 = map(servo2, 2000, 2180, 0, 180);
          if (servo2 < 5)
          {
            servo2 = 5;
          }

          if (servo2 > 50)
          {
            servo2 = 50;
          }     
          vertical.write(servo2);
          delay(delaytime);
        }
      }
  }        
 }
