#include <VectorDisplay.h>

const char* ssid = "********";
const char* password = "********";
const char* host = "192.168.1.110";

WiFiDisplayClass Display;
VectorDisplayMessage msg;

void setup() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Display.begin();
  Display.addButton('c', "Circle");
  Display.addButton('o', "Color");
}

void loop() {
  if (Display.readMessage(&msg) && msg.what == MESSAGE_BUTTON) {
    if (msg.data.button == 'c') {
  		Display.fillCircle(random(240),random(320),random(50));
  	}
  	else if (msg.data.button == 'o') {
  		Display.foreColor(0xFF000000 | (random(256)<<16) | (random(256)<<8) | random(256));
  	}
  }
}