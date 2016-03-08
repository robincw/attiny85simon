#include <avr/sleep.h>
#define BODS 7                     //BOD Sleep bit in MCUCR
#define BODSE 2                    //BOD Sleep enable bit in MCUCR

int level = 0;
int pattern[100];

const int BUTTON_WAIT = 0;
const int BUTTON_DOWN = 1;
const int BUTTON_UP   = 2;
const int BUTTONS_PIN = A3;

void setup() {
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(BUTTONS_PIN, INPUT);
  randomSeed(analogRead(0));
  //gameOver(5);
  //delay(3000);
}

void loop() {
  level ++;
  
  // make pattern
  for(int i=level-1; i<level; i++) {
    pattern[i] = random(1);
    flash(ledFor(pattern[i]));
  }

  // play
  for(int i=0; i<level; i++) {
    int button = getButtonPress();
    if(button != pattern[i]) {
      gameOver(level);
      delay(3000);
      level = 0;
      break;
    }
  }

  // next level!
  delay(1000);
}

void gameOver(int level) {
  for(int i=0; i<level; i++) {
    on(ledFor(0));
    on(ledFor(1));
    on(ledFor(2));
    delay(500);
    off(ledFor(0));
    off(ledFor(1));
    off(ledFor(2));
    delay(300);
  }
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

int ledFor(int buttonValue) {
  return buttonValue;
}

int getButtonValue() {
  goToSleep();
  return (analogRead(BUTTONS_PIN)*3+512)/1024; // http://www.technoblogy.com/show?LSE
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
        on(ledFor(buttonValue));
      }
    } else if(buttonState == BUTTON_DOWN && getButtonValue() == 0) {
      buttonState = BUTTON_UP;
      off(ledFor(buttonValue));
    }
  }
  buttonState = BUTTON_WAIT;
  return buttonValue;
}

// power saving, from https://gist.github.com/JChristensen/5616922
void goToSleep(void)
{
  GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep

  cli();                                  // Disable interrupts
  PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on

  sei();                                  // Enable interrupts
}

//external interrupt 0 wakes the MCU
ISR(PCINT0_vect)
{
    // yawn, drink tea
}
