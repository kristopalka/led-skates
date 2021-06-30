#include <FastLED.h>
#define MAX_LENGTH 20

class Command {
  private:

    char head[MAX_LENGTH];
    char val[MAX_LENGTH];

  public:
    void read() {
      char com[2 * MAX_LENGTH + 1];
      int len = Serial.readBytes(com, 2 * MAX_LENGTH + 1);

      int i = 0;
      int spacePos = -1;
      while (i < len) {
        if (com[i] == ' ') {
          spacePos = i;
          head[i] = 0;
        }
        else if (spacePos == -1) {
          head[i] = com[i];
        }
        else {
          val[i - spacePos - 1] = com[i];
        }
        i++;
      }

      if (spacePos == -1) {
        head[i] = 0;
        val[0] = 0;
      }
      else val[i - spacePos - 1] = 0;
    }


    bool is(char* s) {
      if (strcmp(head, s) == 0) return true;
      else return false;
    }

    bool isVal(char* s) {
      if (strcmp(val, s) == 0) return true;
      else return false;
    }

    int valToInt() {
      int i;
      sscanf(val, "%d", &i);
      return i;
    }

    CRGB valToCRGB() {
      if (isVal("black"))  return CRGB::Black;
      if (isVal("white"))  return CRGB::White;
      if (isVal("red"))    return CRGB::Red;
      if (isVal("blue"))   return CRGB::Blue;
      if (isVal("green"))  return CRGB::Green;
      if (isVal("yellow")) return CRGB::Yellow;
      if (isVal("pink"))   return CRGB::Fuchsia;
      if (isVal("aqua"))   return CRGB::Aqua;

      CRGB crgb = CRGB();
      sscanf(val, "%02x%02x%02x", &crgb.r, &crgb.g, &crgb.b);
      return crgb;
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define NUM_LEDS    14
#define BRIGHTNESS  150
#define DELAY       100

CRGB left[NUM_LEDS];
CRGB right[NUM_LEDS];

int delayTime = DELAY;
unsigned long lastTime = 0;
int counter = 0;

char mode;
CRGB color = CRGB::White;

//////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  FastLED.addLeds<WS2812B, 3, GRB>(left,  NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, 4, GRB>(right, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
  mode = 'h';
}

void loop() {
  readCommand();
  actualizeDiodes();
  actualizeCounter();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void actualizeCounter() {
  if (lastTime < millis() - delayTime) {
    counter++;
    lastTime = millis();
  }
}


void readCommand() {
  if (Serial.available()) {
    Command c;
    c.read();

    // ------------------- change parameters -------------------
    if      (c.is("bright"))    FastLED.setBrightness(c.valToInt());
    else if (c.is("delay"))     delayTime = c.valToInt();
    else if (c.is("color"))     color = c.valToCRGB();

    // ------------------- static modes -------------------
    else if (c.is("static")) {
      setStatic(left, c);
      setStatic(right, c);
    }
    else if (c.is("left"))      setStatic(left, c);
    else if (c.is("right"))     setStatic(right, c);

    // ------------------- dynamic modes -------------------
    else if (c.is("hue"))        {
      mode = 'h';
      delayTime = 100;
    }
    else if (c.is("rainbow"))    {
      mode = 'r';
      delayTime = 20;
    }
    else if (c.is("dot"))        {
      mode = 'd';
      delayTime = 60;
    }
    else if (c.is("rotdot"))     {
      mode = 'o';
      delayTime = 60;
    }
    else if (c.is("ladder"))     {
      mode = 'l';
      delayTime = 170;
    }
    else if (c.is("strobe"))     {
      mode = 's';
      delayTime = 170;
    }
  }
}

void setStatic(CRGB* leds, Command c) {
  mode = "-";
  if (c.isVal("rainbow"))   fill_rainbow(leds, NUM_LEDS, 0, 255 / NUM_LEDS);
  else                      fill_solid(leds, NUM_LEDS, c.valToCRGB());
}

void actualizeDiodes() {
  switch (mode)
  {
    case '-': break;
    case 'h': doHue(); break;
    case 'r': doRainbow(); break;
    case 'd': doRunningDot(); break;
    case 'o': doRotatingDot(); break;
    case 'l': doRunningLadder(); break;
    case 's': doStrobe(); break;
  }


  FastLED.show();
}

void doHue() {
  fill_solid(left,  NUM_LEDS, CHSV(counter & 255, 255, 255));
  fill_solid(right, NUM_LEDS, CHSV(counter, 255, 255));
}

void doRainbow() {
  fill_rainbow(left,  NUM_LEDS, counter % 255, 255 / NUM_LEDS);
  fill_rainbow(right, NUM_LEDS, counter % 255, 255 / NUM_LEDS);
}

void doRunningDot() {
  fill_solid(left,  NUM_LEDS, CRGB::Black);
  fill_solid(right, NUM_LEDS, CRGB::Black);
  int number = counter % (NUM_LEDS / 2);
  left[NUM_LEDS/2 + number] = color;
  left[NUM_LEDS/2 - 1 - number] = color;
  right[NUM_LEDS/2 + number] = color;
  right[NUM_LEDS/2 - 1 - number] = color;
}

void doRotatingDot() {
  fill_solid(left,  NUM_LEDS, CRGB::Black);
  fill_solid(right, NUM_LEDS, CRGB::Black);
  int number = counter % NUM_LEDS;
  left[number] = color;
  right[number] = color;
}

void doRunningLadder() {
  for (int i = 0 ; i < NUM_LEDS; i++) {
    if (i % 2 == counter % 2) {
      left[i] = color;
      right[i] = color;
    }
    else {
      left[i] = CRGB::Black;
      right[i] = CRGB::Black;
    }
  }
}

void doStrobe() {
  if(counter%2 == 0) {
    fill_solid(left,  NUM_LEDS, CRGB::Black);
    fill_solid(right, NUM_LEDS, CRGB::Black);
  }
  else {
    fill_solid(left,  NUM_LEDS, color);
    fill_solid(right, NUM_LEDS, color);
  }
}
