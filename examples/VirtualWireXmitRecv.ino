/*
/  Example for Transmitting AND Receiving with Virtual Wire
/  
/  See Virtual Wire reference and library here:
/  http://www.airspayce.com/mikem/arduino/VirtualWire/
/  
/  @author roadster
*/

#define LEDPin 13  // used to flash LED during transmit

#define trigPin 8  // sonar trigger
#define echoPin 9  // sonar echo   

#define recvPin 11 // Receiver connected to Arduino Pin
#define xmitPin 12 // Transmitter connected to Arduino Pin

#define NAME "joe4" // name of ardiuno
                    // TODO Change to something unique

#include <VirtualWire.h>

// random used for avoid transmit collision
int randomDelay = 0;

// buffer for receiving data
uint8_t recvbuf[VW_MAX_MESSAGE_LEN];
uint8_t recvbuflen = VW_MAX_MESSAGE_LEN;
  
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
  
  // Start the receiver PLL running
  vw_rx_start();       

  // randomly pick a delay between transmits
  randomDelay = random(750, 1500);
  Serial.print("randomDelay=");
  Serial.println(randomDelay);
}

/*
/ After setup completes, this loop will run forever
*/
void loop() {
 
  //
  // This block looks to see if it hears
  // anyone else transmitting
  //

  // reset recv buffer length and recv flag
  recvbuflen = VW_MAX_MESSAGE_LEN;
  boolean msgrecvd = false;
  
  char recvmsg[20];
  recvmsg[0]='\0';
 
  // Non-blocking check if any data available
  if (vw_get_message(recvbuf, &recvbuflen)) {
    msgrecvd = true;

    // strobe LED to show received data
    digitalWrite(LEDPin, true);
    delay(200);    
    digitalWrite(LEDPin, false);
    delay(200);    
    digitalWrite(LEDPin, true);
    delay(200);    
    digitalWrite(LEDPin, false);

    // null terminate our message
    recvbuf[recvbuflen] = 0;

    // debug print to serial monitor
    Serial.print("Got: ");
    Serial.println((char *)recvbuf);
    
    // create a String so that we can more easily pull out a substring
    String s = String((char *)recvbuf);
    
    // we want to get just the NAME from the received string
    // so if we receive some JSON like this:
    //   {"i":"joe4", "d":97, "h":"herman789"}
    // All we care about is the joe4 part
    //
    //   {"i":"joe4", "d":97, "h":"herman789"}
    //   0123456789012345678901234567890123456
    //
    // in other words, we want characters 6 to 9 in the above example (starting with 0)
    
    // we know that the begiginning of or NAME is always 6 but we need to find the end character
    // The indexOf method will search for a character starting any where in the String.
    // So we want to find the quote at the end of the NAME. Start search at character position 6
    int idEnd = s.indexOf('"', 6);
    
    // now pull out a substring from character 6 to the end.
    // we substract 1 cause we don't want the quote at the end but the character before it
    s = s.substring(6, idEnd-1);
    s.toCharArray((char *)recvbuf, recvbuflen);
    Serial.println(s);
    
    // build a partial JSON string
    sprintf(recvmsg, "\"h\":\"%s\"", recvbuf);

    // debug print to serial monitor
    Serial.println((char *)recvmsg);
  }
 
  //
  // This block strobes the sonar and gets a distance
  //

  int duration, distance;
  char distmsg[20];
  distmsg[0]='\0';

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
    distance = 0;
  }

  // build a partial JSON string
  sprintf(distmsg, "\"d\":%d", distance);

  // debug print to serial monitor
  Serial.println((char *)distmsg);
    
  // build JSON string to transmit
  //
  // A good string will look like:
  //
  //     {"i":"joe4", "d":97, "h":"herman789"}
  //

  char msg[50];
  msg[0]='\0';

  // build ID and distance
  sprintf(msg, "{\"i\":\"%s\",%s", NAME, (char *)distmsg);

  // if we recvd data, add that to JSON
  if (msgrecvd) {
    sprintf(msg, "%s,%s}", msg, recvmsg);
 
  // else just close up JSON
  } else {
    sprintf(msg, "%s}", msg);
  }
  Serial.println(msg);

  // Flash LED to show transmitting
  digitalWrite(LEDPin, true);

  // send json string with virtual wire
  const char* temp = msg;
  vw_send((uint8_t *)temp, strlen(temp));

  // Wait until the whole message is gone
  vw_wait_tx(); 
  digitalWrite(LEDPin, false);
  
  // delay for some random time
  delay(randomDelay);
}
