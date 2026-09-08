#include <M5Unified.h>
#include <GDTouchKeyboard.h>

void setup()
{
  auto config = M5.config();
  M5.begin(config);
}

void loop()
{
  String t = GDTK.run();
  Serial.println(t);
  delay(1000);
}
