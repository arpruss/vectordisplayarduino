  #include <VectorDisplay.h>

VectorDisplay<decltype(Serial)> SerialDisplay(Serial);

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);

  delay(5000);
  SerialDisplay.clear();
}

struct {
  uint16_t x0;
  uint16_t y0;
  uint16_t x1;
  uint16_t y1;
} __attribute__((packed)) line = { 0, 0, 511, 511 };

uint16_t y=0;

void loop() {

  digitalWrite(LED_BUILTIN, 0);
//  delay(100);
  digitalWrite(LED_BUILTIN, 1);
//  delay(100);
//  Serial.write("MHello", 7);
  y++;
  line.y0 = (line.y0+1)%512;
  SerialDisplay.line(0, y, 511, 511);
  SerialDisplay.text(50, 20, String(y));
  delay(5);
}

