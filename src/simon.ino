#include <avr/sleep.h>
#define BODS 7                     //BOD Sleep bit in MCUCR
#define BODSE 2                    //BOD Sleep enable bit in MCUCR

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
    flash(ledFor(pattern[i]));
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
    on(1);
    on(2);
    on(3);
    delay(500);
    off(1);
    off(2);
    off(3);
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

int ledFor(int buttonValue) {
  return buttonValue +1;
}

int getButtonValue() {
  goToSleep();
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
        on(ledFor(buttonValue);
      }
    } elseif(buttonState == BUTTON_DOWN && buttonValue == 0) {
      buttonState = BUTTON_UP;
      off(ledFor(buttonValue);
    }
  }

  return buttonValue;
}

// power saving, from https://gist.github.com/JChristensen/5616922
void goToSleep(void)
{
    byte adcsra, mcucr1, mcucr2;

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    MCUCR &= ~(_BV(ISC01) | _BV(ISC00));      //INT0 on low level
    GIMSK |= _BV(INT0);                       //enable INT0
    adcsra = ADCSRA;                          //save ADCSRA
    ADCSRA &= ~_BV(ADEN);                     //disable ADC
    cli();                                    //stop interrupts to ensure the BOD timed sequence executes as required
    mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);  //turn off the brown-out detector
    mcucr2 = mcucr1 & ~_BV(BODSE);            //if the MCU does not have BOD disable capability,
    MCUCR = mcucr1;                           //  this code has no effect
    MCUCR = mcucr2;
    sei();                                    //ensure interrupts enabled so we can wake up again
    sleep_cpu();                              //go to sleep
    sleep_disable();                          //wake up here
    ADCSRA = adcsra;                          //restore ADCSRA
}

//external interrupt 0 wakes the MCU
ISR(INT0_vect)
{
    GIMSK = 0;                     //disable external interrupts (only need one to wake up)
}