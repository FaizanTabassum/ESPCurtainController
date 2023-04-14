
int buttonlo = 23;
int buttonlc = 22;
int buttonro = 19;
int buttonrc = 18;

const int CCW = 2; // do not change
const int CW  = 1; // do not change

#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Robojax_L298N_DC_motor.h>
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "tabassum 2"
#define WLAN_PASS       "786simmi"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "faizan_tabassum"
#define AIO_KEY       "aio_UAMz92gqC1kxqpK4qdDrrQHpcJ7A"
#define motor 12
/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/motor");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
// motor 1 settings
#define CHA 0
#define ENA 13 // this pin must be PWM enabled pin if Arduino board is used
#define IN1 12
#define IN2 14

// motor 2 settings
#define IN3 27
#define IN4 26
#define ENB 25// this pin must be PWM enabled pin if Arduino board is used
#define CHB 1

#define motor1 1 // do not change
#define motor2 2 // do not change
Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  robot.begin();
  delay(10);
  pinMode(motor,OUTPUT);
  pinMode(buttonlo,INPUT_PULLUP);
  pinMode(buttonlc,INPUT_PULLUP);
  pinMode(buttonro,INPUT_PULLUP);
  pinMode(buttonrc,INPUT_PULLUP);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  
  
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
      uint16_t state = atoi((char *)onoffbutton.lastread);

          
        if(state == LOW){
          close();
          }
        if(state == HIGH){
         open();
          }
    }
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void close(){
while(digitalRead(buttonrc) == HIGH || digitalRead(buttonlc) == HIGH){
            if(digitalRead(buttonlc) == HIGH ){robot.rotate(motor1, 100, CW);}
            else{robot.brake(1);}
            if(digitalRead(buttonrc) == HIGH ){robot.rotate(motor2, 100, CW);}
            else{robot.brake(2);}
            if(digitalRead(buttonlc) == LOW && digitalRead(buttonrc) == LOW  ){break;}}
}
void open(){
while(digitalRead(buttonro) == HIGH || digitalRead(buttonlo) == HIGH){
            if(digitalRead(buttonlo) == HIGH ){robot.rotate(motor1, 100, CCW);}
            else{robot.brake(1);}
            if(digitalRead(buttonro) == HIGH ){robot.rotate(motor2, 100, CCW);}
            else{robot.brake(2);}
            if(digitalRead(buttonlo) == LOW && digitalRead(buttonro) == LOW  ){break;}}
}
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
