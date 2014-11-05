/*
/  Example for Reading distance from HC-SR04
/  
/  @author roadster
*/

#define trigPin 8 // HC-SR04 Trigger connected to Arduino Pin
#define echoPin 9 // HC-SR04 Echo connected to Arduino Pin

/*
/ Setup Arduino
*/
void setup() {

  // this is for debug output to Serial Monitor
  Serial.begin(9600);

  // setup Sonar pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
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
    Serial.print(distance);
    Serial.println(" cm");
  }
  
  // wait a second before reading again
  delay(1000);
}
