//Code written by Faizan, Automated curtains can be controlled with google assistant or alexa, whatever is supported by ifttt.
//These are the pins for the l298n motor drivers
#define in1 14
#define in2 27
#define in3 26
#define in4 25

//These are the pins for the limit switches
int lo = 33;     // lo stands for left open
int lc = 32;     // left close
int ro = 35;     // right open
int rc = 34;     // right close
uint16_t state;  // This is the state variable for curtains
uint32_t x = 0;

static const BaseType_t pro_cpu = 0;  // these are for multithreading
static const BaseType_t app_cpu = 1;

#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
/************************* WiFi Access Point *********************************/

#define WLAN_SSID "enter your wifi username"
#define WLAN_PASS "enter your wifi password"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883  // use 8883 for SSL
#define AIO_USERNAME "enter your username"
#define AIO_KEY "enter your key here"
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
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");
  Serial.println(xPortGetCoreID());

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {  // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}

void doTaskH(void *pvParameters) {
  // This is the code which keeps track of the curtains
  // Its simple logic if the concurrent switch isnt pressed keep running the curtians
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  for (;;) {
    if (state == HIGH) {
      if (digitalRead(rc) == HIGH) {
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
      } else {
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
      }
      if (digitalRead(lc) == HIGH) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
      } else {
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
      }
    }

    if (state == LOW) {
      if (digitalRead(lo) == HIGH) {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
      } else {
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
      }
      if (digitalRead(ro) == HIGH) {
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
      } else {
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
      }
    }
  }
}

void doTaskL(void *pvParameters) {
  // This part keeps track of all the wifi connections
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  for (;;) {
    MQTT_connect();
    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt.readSubscription(5000))) {
      if (subscription == &onoffbutton) {
        Serial.print(F("Got: "));
        Serial.println((char *)onoffbutton.lastread);
        state = atoi((char *)onoffbutton.lastread);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(lc, INPUT_PULLUP);
  pinMode(ro, INPUT_PULLUP);
  pinMode(rc, INPUT_PULLUP);
  pinMode(lo, INPUT_PULLUP);

  Serial.println(F("Adafruit MQTT"));

  // Connect to WiFi access point.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);

  // Start Task L (low priority)
  xTaskCreatePinnedToCore(doTaskL,
                          "Task L",
                          2048,
                          NULL,
                          1,
                          NULL,
                          //                          pro_cpu);
                          tskNO_AFFINITY);

  // Start Task H (low priority)
  xTaskCreatePinnedToCore(doTaskH,
                          "Task H",
                          2048,
                          NULL,
                          2,
                          NULL,
                          //                          pro_cpu);
                          tskNO_AFFINITY);

  // Delete "setup and loop" task
  vTaskDelete(NULL); /* pin task to core 0 */
}

void loop() {
}
