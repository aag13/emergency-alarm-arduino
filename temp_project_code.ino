#include <elapsedMillis.h>
#include <Servo.h>

// for SMS
//#include <SoftwareSerial.h>
//SoftwareSerial SIM900(7, 8);  // RX, TX


//Servo motor;
//int pos = 0;      //initial motor position
//int deg = 50;

elapsedMillis timerT;
int interval = 30000;         //30s

int ledPin = 12;                // choose the pin for the LED

int motionLedPin = 11;          // to indicate motion

int switchInputPin = 3;         // to get switch input
int current = 0;                   // current=0 means door is OPEN, current=1 means door is CLOSED......... DOOR IS INITIALIZED WITH 0 OPEN

int pirInputPin = 7;              // choose the input pin (for PIR sensor)
int sval = 0;                      // variable for reading the switch input
int pval = 0;                     // variable for reading the PIR input

int pinSpeaker = 9;           //Set up a speaker on a PWM pin (digital 9, 10, or 11)

//sonar pin numbers
const int trigPin = 10;
const int echoPin = 2;     //11

// sonar variables
long duration = 0;
int distance = 0;
int boundary = 120;

//assuming the boundary distance is 10 feet or 120 inches......
//before raising alarm, check whether the current distance is <= boundary-10(considering errors) inches, if it is, then someone is standing still although not moving
// if it is > boundary-10, then raise the alarm and others.......


void setup() {

  //motor.attach(9);   //speaker is at 9

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(pirInputPin, INPUT);    //PIR
  pinMode(ledPin, OUTPUT);        // LED
  pinMode(motionLedPin, OUTPUT);

  pinMode(switchInputPin, INPUT); //SWITCH
  timerT = 0;
  pinMode(pinSpeaker, OUTPUT);

//for SMS
//SIM900.begin(19200);                
//  delay(2000);

  
  Serial.begin(9600);
}

void loop() {

  sval = digitalRead(switchInputPin);  // read input value
  if (sval == HIGH) {
    //switch input is HIGH, so, switch is pressed ON....now check the door status...open or closed

    Serial.println("SWITCHED IS PRESSED ON");

    if (current == 0) { //CLOSE THE DOOR
      // door is open and switch input is HIGH, so it means close the door
      //run the motor to CLOSE the door, initialize the timer to 0

      Serial.println("GOING TO CLOSE");

      //closeMotor();
      current = 1;  // door is closed
      timerT = 0;

    } else { // CHECK FOR EMERGENCY
      //current = 1, so door is closed and switch input is high(meaning door is locked from inside, and it is still in lock mode)
      // take PIR input=> if low,then check whether timer is greater than interval,( if yes,then emergency,else keep timer running), else high, so reset the timer

      pval = digitalRead(pirInputPin);

      if (pval == HIGH) {            // PIR input HIGH , motion, means reset the timer

        Serial.println("MOTION MOTION");
        digitalWrite(motionLedPin, HIGH);
        timerT = 0;
        digitalWrite(motionLedPin, LOW);
      } else {                     // PIR input LOW , no motion, means keep the timer running

        if (timerT >= interval) {
          boolean flag = sonar();
          if(flag){
            // so emergency, raise alarm, send sms, open the door
          // buzzer
          //send sms
          
          //SMS();
          
          //openMotor();
          digitalWrite(ledPin, HIGH);  // turn LED ON
          Serial.println("EMERGENCY EMERGENCY EMERGENCY");
          playTone(3000);   // this delay is for the buzzer
          //after this amount of time, reset everything
          //pos = 0;
          timerT = 0;
          current = 0;
          sval = 0;
          pval = 0;

          delay(10000);
          digitalWrite(ledPin, LOW);  // turn LED OFF
         }else{
          timerT = 0;
          sval = 0;
          pval = 0;
          
          }
          

        }


      }

    }

  } else {
    // switch input is LOW, so, switch is pressed OFF....now check the door status...open or closed

    if (current == 0) { //DO NOTHING
      // door is open and switch input is LOW, so nothing to do
      Serial.println("DOING NOTHING");


    } else { //OPEN THE DOOR
      //current = 1, so door is closed and switch input is LOW(so it means open the door)
      //reset everything to initial state

      Serial.println("SWITCHED IS PRESSED OFF");
      //openMotor();
      Serial.println("OPEN THE DOOR");
      //pos = 0;
      timerT = 0;
      current = 0;   //door is open
      sval = 0;
      pval = 0;

    }

  }

}

boolean sonar() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  if(distance <= (boundary-10)){
     // someone's there, but not moving, so NO PROBLEM...
     return false;
   }else{
    //someone is in, and NOT moving and the sonar boundary is reached...so RAISE ALARM
    return true;
    }
  
}


//void closeMotor() {
//  for (pos = 0; pos < deg; pos++) {
//    motor.write(pos);
//    delay(10);
//  }
//}
//
//void openMotor() {
//  for (pos = deg; pos >= 1 ; pos--) {
//    motor.write(pos);
//    delay(10);
//  }
//}



void playTone(int duration) {
  digitalWrite(pinSpeaker, HIGH);
  delay(duration);
  digitalWrite(pinSpeaker, LOW);
}




//void SMS(){
//  SIM900.begin(19200);  //Default serial port setting for the GPRS modem is 19200bps 8-N-1
//  SIM900.print("\r");
//  delay(1000);                    //Wait for a second while the modem sends an "OK"
//  SIM900.print("AT+CMGF=1\r");    //Because we want to send the SMS in text mode
//  delay(1000);
//
//  //Serial.print("AT+CSCA=\"+919032055002\"\r");  //Setting for the SMS Message center number,  
//  //delay(1000);                                  //uncomment only if required and replace with
//                                                  //the message center number obtained from
//                                                  //your GSM service provider.
//                                                  //Note that when specifying a tring of characters
//                                                  // " is entered as \"
//
//  SIM900.print("AT+CMGS=\"+8801673235388\"\r");    //Start accepting the text for the message
//                                                  //to be sent to the number specified.
//                                                  //Replace this number with the target mobile number.
//  delay(1000);
//  SIM900.print("SIM900 and Arduino say Hi!\r");   //The text for the message
//  delay(1000);
//  SIM900.write(0x1A);  //Equivalent to sending Ctrl+Z 
//}
