#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <wifi_provisioning/manager.h>

const char *service_name = "CURTAIN_PROV";
const char *pop = "123456";

static uint8_t gpio_reset = 0;
bool wifi_connected = false;

// Motor relay and limit switch pins
static uint8_t in1 = 19, in2 = 21, in3 = 22, in4 = 23;
static uint8_t lo = 33, lc = 32, ro = 35, rc = 34;

bool curtain_state = false;
bool last_curtain_state = false;
static Switch curtain_switch("Curtain", NULL);

void sysProvEvent(arduino_event_t *sys_event)
{
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
      Serial.printf("\n🔵 Provisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      Serial.println("📱 Scan QR code with ESP RainMaker app:");
      printQR(service_name, pop, "ble");
      break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("✅ Connected to Wi-Fi!");
      wifi_connected = true;
      break;

    case ARDUINO_EVENT_PROV_CRED_RECV:
      Serial.println("📡 Received Wi-Fi credentials");
      Serial.print("\tSSID: ");
      Serial.println((const char *) sys_event->event_info.prov_cred_recv.ssid);
      break;

    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      Serial.println("🎉 Provisioning successful!");
      break;
  }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
  if (strcmp(device->getDeviceName(), "Curtain") == 0) {
    if (strcmp(param->getParamName(), "Power") == 0) {
      curtain_state = val.val.b;
      Serial.printf("🎛️ Curtain command: %s\n", curtain_state ? "OPEN" : "CLOSE");
      param->updateAndReport(val);
    }
  }
}

void debugLimitSwitches() {
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 2000) {
    Serial.printf("Limit Switches - LO:%d LC:%d RO:%d RC:%d\n",
                  digitalRead(lo), digitalRead(lc), digitalRead(ro), digitalRead(rc));
    lastDebug = millis();
  }
}

void stopAllMotors() {
  // Active-HIGH relay board: LOW = OFF
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void setup()
{
  Serial.begin(115200);
  delay(3000);

  Serial.println("\n🚀 ESP32 Curtain Controller Starting...");


  pinMode(gpio_reset, INPUT);
  pinMode(in1, OUTPUT); pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT); pinMode(in4, OUTPUT);

  pinMode(lc, INPUT_PULLUP);
  pinMode(lo, INPUT_PULLUP);

  // Note: GPIO34 and GPIO35 are input-only and do not support internal pull-ups
  pinMode(ro, INPUT);
  pinMode(rc, INPUT);

  stopAllMotors();

  WiFi.mode(WIFI_OFF);
  delay(1000);

  Node my_node = RMaker.initNode("ESP32_Curtain");
  curtain_switch.addCb(write_callback);
  my_node.addDevice(curtain_switch);

  RMaker.enableOTA(OTA_USING_PARAMS);
  RMaker.enableTZService();
  RMaker.enableSchedule();

  Serial.println("🌧️ Starting RainMaker...");
  RMaker.start();

  WiFi.onEvent(sysProvEvent);

  Serial.println("🔵 Starting BLE Provisioning...");
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE,
                          WIFI_PROV_SCHEME_HANDLER_FREE_BTDM,
                          WIFI_PROV_SECURITY_1,
                          pop,
                          service_name);

  Serial.println("✅ Setup complete - QR code should appear above!");

  Serial.println("🔧 Initial limit switch states:");
  Serial.printf("LO(33):%d LC(32):%d RO(35):%d RC(34):%d\n",
                digitalRead(lo), digitalRead(lc), digitalRead(ro), digitalRead(rc));
}

void loop()
{
  debugLimitSwitches();

  if (curtain_state != last_curtain_state) {
    last_curtain_state = curtain_state;

    if (curtain_state) {
      Serial.println("🔄 Starting to OPEN curtains...");
    } else {
      Serial.println("🔄 Starting to CLOSE curtains...");
    }
  }

  if (curtain_state) {
    // OPEN curtains

    // Right motor - stop when RC limit switch is pressed (LOW)
    if (digitalRead(rc) == HIGH) {
      digitalWrite(in3, HIGH);  // ON
      digitalWrite(in4, LOW);   // OFF
    } else {
      digitalWrite(in3, LOW);   // OFF
      digitalWrite(in4, LOW);   // OFF
    }

    // Left motor - stop when LC limit switch is pressed (LOW)
    if (digitalRead(lc) == HIGH) {
      digitalWrite(in1, LOW);   // OFF
      digitalWrite(in2, HIGH);  // ON
    } else {
      digitalWrite(in1, LOW);   // OFF
      digitalWrite(in2, LOW);   // OFF
    }
  }
  else {
    // CLOSE curtains

    // Left motor - stop when LO limit switch is pressed (LOW)
    if (digitalRead(lo) == HIGH) {
      digitalWrite(in1, HIGH);  // ON
      digitalWrite(in2, LOW);   // OFF
    } else {
      digitalWrite(in1, LOW);   // OFF
      digitalWrite(in2, LOW);   // OFF
    }

    // Right motor - stop when RO limit switch is pressed (LOW)
    if (digitalRead(ro) == HIGH) {
      digitalWrite(in3, LOW);   // OFF
      digitalWrite(in4, HIGH);  // ON
    } else {
      digitalWrite(in3, LOW);   // OFF
      digitalWrite(in4, LOW);   // OFF
    }
  }

  if (digitalRead(gpio_reset) == LOW) {
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_reset) == LOW) delay(50);
    int duration = millis() - startTime;

    if (duration > 10000) {
      Serial.println("🔄 Factory Reset!");
      RMakerFactoryReset(2);
    }
  }

  delay(100);
}
