#include <FastLED.h>
#include <EEPROM.h>

#define FASTLED_ALLOW_INTERRUPTS 1
#define FASTLED_INTERRUPT_RETRY_COUNT 1


#define LED_PIN             2
#define NUMBER_OF_LEDS      3
#define DELAY_TIME_MS       20
#define COLOR_MAX           255
#define BUTTON_PIN          4
#define TOTAL_MODES         13
#define MODE_ADDRESS        0

int           buttonState;                      // the current reading from the input pin
int           lastButtonState = LOW;            // the previous reading from the input pin
unsigned long lastDebounceTime = 0;   // the last time the output pin was toggled
unsigned long debounceDelay = 50;     // the debounce time; increase if the output flickers
int           mode = 0;

CRGB leds[NUMBER_OF_LEDS];


void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUMBER_OF_LEDS);  

  //mode = EEPROM.read(MODE_ADDRESS);
  mode = 12;

  if (mode > TOTAL_MODES || mode < 0) {
    // probably the first time ever reading EEPROM
    EEPROM.write(MODE_ADDRESS, 0);
    mode = 0;
  }  

  Serial.println("Startup");
  Serial.print("Mode is ");
  Serial.println(mode);
}


void showme (char * tag, int r, int g, int b) {
  Serial.print(tag);
  Serial.print(r);
  Serial.print(", ");
  Serial.print(g);
  Serial.print(", ");
  Serial.println(b);
}


int handleModeButton() {
  int oldMode = mode;
  int reading = digitalRead(BUTTON_PIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        mode = (mode + 1) % TOTAL_MODES;
        EEPROM.write(MODE_ADDRESS, mode);
        Serial.print("BUTTON, mode is ");
        Serial.println(mode);        
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;  
  if (oldMode != mode) {
    return 1;
  } else {
    return 0;
  }
}


void solid(CRGB color) {
  for (int x = 0; x < NUMBER_OF_LEDS; x++) {
    leds[x] = color;
  }  
  FastLED.show();
}

void twoTone(CRGB first, CRGB second) {
  leds[0] = first;
  leds[1] = first;
  leds[2] = first;
  leds[3] = second;
  leds[4] = second;
  leds[5] = second;
  FastLED.show();
  
}


int checkModeAndDelay(int delayTime) {
  int steps = delayTime / 10;
  int didModeSwitch = 0;
  for (int i = 0; i < steps; i++) {
    int result = handleModeButton();
    if (result == 1) {
      didModeSwitch = 1;
    }
    delay(10);
  }
  return didModeSwitch;
}


void fade() {
  int r, g, b;
  r = 255;
  g = 255;
  b = 255;

  // WHITE
  for (int x = 0; x < NUMBER_OF_LEDS; x++) {
    leds[x] = CRGB(r,g,b);
  }

  showme("start ", r,g,b);

  
  for (; r > 0; r--) {
    for (int x = 0; x < NUMBER_OF_LEDS; x++) {
      leds[x] = CRGB(r,g,b);
    }
    FastLED.show();
    
    if (checkModeAndDelay(DELAY_TIME_MS) == 1) {
      return;
    }
  }
  // now we have 0, 255, 255
  showme("after 1 ", r,g,b);
  
  for(; g > 0; g--) {
    for (int x = 0; x < NUMBER_OF_LEDS; x++) {
      leds[x] = CRGB(r,g,b);
    }
    FastLED.show();
    if (checkModeAndDelay(DELAY_TIME_MS) == 1) {
      return;
    }
  }
  // now we have 0, 0, 255
  showme("after 2 ", r,g,b);
  
  for (; r < COLOR_MAX; r++) {
    for (int x = 0; x < NUMBER_OF_LEDS; x++) {
      leds[x] = CRGB(r,g,b);
    }
    FastLED.show();
    if (checkModeAndDelay(DELAY_TIME_MS) == 1) {
      return;
    }
  }
  // now we have 255, 0, 255
  showme("after 3 ", r,g,b);

  for (; b > 0; b--) {
    for (int x = 0; x < NUMBER_OF_LEDS; x++) {
      leds[x] = CRGB(r,g,b);
    }
    FastLED.show();    
    if (checkModeAndDelay(DELAY_TIME_MS) == 1) {
      return;
    }
  }
  // now we have 255, 0, 0
  showme("after 4 ", r,g,b);

  for (; g < COLOR_MAX; g++) {
    for (int x = 0; x < NUMBER_OF_LEDS; x++) {
      leds[x] = CRGB(r,g,b);
    }
    FastLED.show();    
    if (checkModeAndDelay(DELAY_TIME_MS) == 1) {
      return;
    }
  }
  // now we have 255, 255, 0
  showme("after 5 ", r,g,b);

  for (; r > 0; r--) {
    for (int x = 0; x < NUMBER_OF_LEDS; x++) {
      leds[x] = CRGB(r,g,b);
    }
    FastLED.show();    
    if (checkModeAndDelay(DELAY_TIME_MS) == 1) {
      return;
    }
  }
  // now we have 0, 255, 0
  showme("after 6 ", r,g,b);

  for (; b < COLOR_MAX; b++, r++) {
    for (int x = 0; x < NUMBER_OF_LEDS; x++) {
      leds[x] = CRGB(r,g,b);
    }
    FastLED.show();    
    if (checkModeAndDelay(DELAY_TIME_MS) == 1) {
      return;
    }
  }
  // now we have 255, 255, 255
  showme("after 7 ", r,g,b);
  
}

void off() {
  for (int x = 0; x < NUMBER_OF_LEDS; x++) {
    leds[x] = CRGB::Black;
  }
  FastLED.show();  
}


void chase() {
  for (int count = 0; count < NUMBER_OF_LEDS; count++) {
    off();
    leds[count] = CRGB::Blue;  
    FastLED.show();
    checkModeAndDelay(250);
  } 
}

void rainbow() {
  for (int i = 0; i < NUMBER_OF_LEDS; i++) {
    leds[i] = CRGB::Red;
    leds[(i + 1) % NUMBER_OF_LEDS] = CRGB::DarkOrange;
    leds[(i + 2) % NUMBER_OF_LEDS] = CRGB::Yellow;
    leds[(i + 3) % NUMBER_OF_LEDS] = CRGB::Green;
    leds[(i + 4) % NUMBER_OF_LEDS] = CRGB::Blue;
    leds[(i + 5) % NUMBER_OF_LEDS] = CRGB::Purple;
    FastLED.show();    
    if (checkModeAndDelay(500) == 1) {
      return;
    }
  }
}



void loop() {
  handleModeButton();

  switch (mode) {
    case 0: 
    Serial.print("mode ");
    Serial.println(mode);
    fade();
    break;

    case 1: 
    Serial.print("mode ");
    Serial.println(mode);
    solid(CRGB::Red);
    break;

    case 2: 
    Serial.print("mode ");
    Serial.println(mode);
    solid(CRGB::Green);
    break;

    case 3: 
    Serial.print("mode ");
    Serial.println(mode);
    solid(CRGB::Blue);
    break;

    case 4:
    Serial.print("mode ");
    Serial.println(mode);
    solid(CRGB::Purple);
    break;

    case 5:
    Serial.print("mode ");
    Serial.println(mode);
    solid(CRGB::DarkOrange);
    break;

    case 6:
    Serial.print("mode ");
    Serial.println(mode);
    solid(CRGB::Yellow);
    break;
    
    case 7:
    Serial.print("mode ");
    Serial.println(mode);
    solid(CRGB::Pink);
    break;

    case 8:
    Serial.print("mode ");
    Serial.println(mode);
    twoTone(CRGB::Blue, CRGB::Green);
    break;

    case 9:
    Serial.print("mode ");
    Serial.println(mode);
    twoTone(CRGB::Red, CRGB::Blue);
    break;

    case 10:
    Serial.print("mode ");
    Serial.println(mode);
    chase();
    break;

    case 11:
    Serial.print("mode ");
    Serial.println(mode);
    rainbow();
    break;

    case 12:
    Serial.print("mode ");
    Serial.println(mode);
    solid(CRGB::White);
    break;
    
  }

  checkModeAndDelay(100);
  
}
