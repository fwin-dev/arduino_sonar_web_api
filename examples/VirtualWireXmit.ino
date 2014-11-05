/*
/  Example for Transmitting with Virtual Wire
/  
/  See Virtual Wire reference and library here:
/  http://www.airspayce.com/mikem/arduino/VirtualWire/
/  
/  @author roadster
*/

#define LEDPin 13  // used to flash LED during transmit

#define trigPin 8  // sonar trigger
#define echoPin 9  // sonar echo   

#define xmitPin 12 // Transmitter connected to Arduino Pin

#define NAME "herman789" // name of ardiuno
                    // TODO Change to something unique

#include <VirtualWire.h>

// random used for avoid transmit collision
int randomDelay = 0;

/*
/ Setup Arduino
*/
void setup() {

  // this is for debug output to Serial Monitor
  Serial.begin(9600);

  // setup Sonar pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	     // Bits per sec
  
  // randomly pick a delay between transmits
  randomDelay = random(450, 650);
  Serial.print("randomDelay=");
  Serial.println(randomDelay);
}

/*
/ After setup completes, this loop will run forever
*/
void loop() {
  
  int duration, distance;

  // put the sonar trigger pin high for 1 second
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  
  // set trigger low
  digitalWrite(trigPin, LOW);

  // read the echo
  duration = pulseIn(echoPin, HIGH);

  // the distance is 1/2 the time divided by speed of light
  distance = (duration/2) / 29.1;
  Serial.print("distance=");
  Serial.println(distance);

  // beyond 200cm or less than 0 is pretty much noise
  if (distance >= 200 || distance <= 0) {
    Serial.println("out of range");
  
  } else {
    
    // build JSON string to tranmit
    //
    // A good string will look like:
    //
    //     {"id":"joe4", "dist":97}
    //
    char msg[50];
    msg[0]='\0';
    sprintf(msg, "{\"id\":\"%s\", \"dist\":%d}", NAME, distance);
    Serial.println(msg);

    // Flash LED to show transmitting
    digitalWrite(LEDPin, true);

    // send json string with virtual wire
    uint8_t *temp = (uint8_t *)msg;
    vw_send(temp, strlen(temp));

    // Wait until the whole message is gone
    vw_wait_tx(); 
    digitalWrite(LEDPin, false);
    delay(200);    
  }
  
  delay(randomDelay);
}
