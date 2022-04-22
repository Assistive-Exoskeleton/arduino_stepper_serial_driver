#include "stepper_control.h"

#define TIMER_FREQ 10000
#define CLOCK_FREQ 16000000

#define MAX_SPEED 400 // steps/s

#define START_BYTE 's'
#define END_BYTE 'e'
#define WHOAMI_BYTE 'd'
#define PKG_LEN 5 // ['s', command, data1, data0, 'e']

#define COMMAND_SET_VELOCITY 'v'
#define COMMAND_SET_POSITION 'p'
#define COMMAND_WHOAMI 'w'

#define STATE_POSITION 0
#define STATE_VELOCITY 1
#define STATE_IDLE 2


int received[PKG_LEN] = {0};
int state = STATE_IDLE;

long long int timer_p_current = 0;
long long int timer_p_total = 0;
long long int steps_remaining = 0;

StepperControl stepper_control(8,9,10,11);
void setup() {
  interrupt_init();
  Serial.begin(115200);
  Serial.println("Ready.");
}

void loop() {
  read_pkg(received);
  delay(5);
}

ISR(TIMER1_COMPA_vect){

  if (state != STATE_IDLE){
    timer_p_current++;
    if (timer_p_current >= timer_p_total){
      stepper_control.step();
      timer_p_current = 0;
      if (state == STATE_POSITION){
        if (steps_remaining == 0){
          state = STATE_IDLE;
        }
        else{
          steps_remaining--;
        }
      }
    }
  }

}

void parse(int * received)
{
  if (received[1] == COMMAND_WHOAMI){
    Serial.print("I'm: ");
    Serial.write(WHOAMI_BYTE);
    Serial.println();
  }
  else{
    int data = 0;
    for (int i = 2; i<PKG_LEN-1; i++){
      data = (data << 8) | received[i];
    }
    Serial.print("data: ");
    Serial.write(data);
    Serial.println();
    if (data == 0){
      state = STATE_IDLE;
    }
    else{
      stepper_control.set_direction(data);
      data = abs(data);
      switch (received[1]){
        case (COMMAND_SET_VELOCITY):
          if (data > MAX_SPEED)
            data = MAX_SPEED;
          timer_p_total = stepsFreq2TimerPeriods(data);
          state = STATE_VELOCITY;
          break;
        case (COMMAND_SET_POSITION):
          steps_remaining = data;
          state = STATE_POSITION;
          break;
      }
    }
  }
}

void read_pkg(int * received)
{
  bool started = false;
  int i = 0;
  int rec;

  while(Serial.available()>0)
  {
    rec = Serial.read();

    if (started){
      received[i] = rec;
      if (i == PKG_LEN-1){
        if (received[i] == END_BYTE){
          parse(received);
        }
        started = false;
        i = 0;
      }
      else{
        i++;
      }
    }

    else if (rec == START_BYTE){
      started = true;
      received[0] = rec;
      i++;
    }
  }
}

int stepsFreq2TimerPeriods (int steps_freq)
{
  return round((double)TIMER_FREQ / (double)steps_freq); 
}

void interrupt_init()
{
  // TIMER 1 for interrupt frequency 1000000 Hz:
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for 1000000 Hz increments
  OCR1A = CLOCK_FREQ/ (1*TIMER_FREQ)-1;//(must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 1 prescaler
  TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts
}
