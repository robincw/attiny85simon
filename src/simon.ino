int level = 1;
int pattern[100];

const int BUTTON_WAIT = 0;
const int BUTTON_DOWN = 1;
const int BUTTON_UP   = 2;

void setup() {
  randomSeed(analogRead(0));
}

void loop() {
  // make pattern
  for(int i=level; i<level; i++) {
    pattern[i] = random(3);
    flash(pattern[i]);
  }

  // play
  for(int i=0; i<level; i++) {
    int button = getButtonPress();
    if(button != pattern[i]) {
      gameOver(level);
      break
    }
  }

  // next level!
  delay(1000);
  level ++;
}

void gameOver(int level) {
  for(int i=0; i<level; i++) {
    on(0);
    on(2);
    on(1);
    delay(500);
    off(0);
    off(2);
    off(1);
    delay(300);
  }
  level = 0;
}

void on(int pin) {
  digitalWrite(pin, HIGH);
}

void off(int pin) {
  digitalWrite(pin, LOW);
}

void flash(int pin) {
  on(pin);
  delay(200);
  off(pin);
  delay(300);
}

int getButtonValue() {
  return (analogRead(A0)*3+512)/1024; // http://www.technoblogy.com/show?LSE
}

int getButtonPress() {
  // read the state of the pushbutton value:
  int buttonState = BUTTON_WAIT;
  int buttonValue = 0;

  while(buttonState != BUTTON_UP) {
    if(buttonState == BUTTON_WAIT) {
      if(buttonValue == 0) {
        buttonValue = getButtonValue();
      } else {
        buttonState = BUTTON_DOWN;
        on(buttonValue);
      }
    } elseif(buttonState == BUTTON_DOWN && buttonValue == 0) {
      buttonState = BUTTON_UP;
      off(buttonValue);
    }
  }

  return buttonValue;
}