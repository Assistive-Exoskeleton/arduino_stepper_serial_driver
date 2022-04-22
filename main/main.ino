#include "stepper_control.h"

/*=========
  INTERRUPT
  =========*/
#define TIMER_FREQ 10000
#define CLOCK_FREQ 16000000

/*====================
  SERIAL COMMUNICATION
  ====================*/
#define HEADER 's'
#define TAIL 'e'
#define WHOAMI "Da"
#define PKG_LEN 5 // ['s', command, data1, data0, 'e']
#define COMMAND_SET_VELOCITY 'v'
#define COMMAND_SET_POSITION 'p'
#define COMMAND_WHOAMI 'w'

#define STATE_POSITION 0
#define STATE_VELOCITY 1
#define STATE_IDLE 2


unsigned char received[PKG_LEN] = {0};
char state = STATE_IDLE;

long timer_p_current = 0;
long timer_p_total = 0;
int steps = 0;

StepperControl motor(8,9,10,11);
void setup() {
  interrupt_init();
  Serial.begin(115200);
}

void loop() {
  read_pkg(received);
  delay(5);
}

ISR(TIMER1_COMPA_vect){
  if (state != STATE_IDLE){
    timer_p_current++;
    if (timer_p_current >= timer_p_total){
      if (state == STATE_POSITION && motor.get_steps() == steps){
        state = STATE_IDLE;
      }
      else{
        motor.step();
        timer_p_current = 0;
      }
    }
  }
}

void interrupt_update()
{
  timer_p_total = stepsFreq2TimerPeriods(motor.get_speed());
}

int stepsFreq2TimerPeriods (int steps_freq)
{
  return round((double)TIMER_FREQ / (double)steps_freq); 
}

void parse(unsigned char * received)
{
  if (received[1] == COMMAND_WHOAMI){
    for (int i = 2; i<PKG_LEN-1; i++){
      received[i] = WHOAMI[i-2]; //send whoami as data of echo
    }
  }
  else{
    state = STATE_IDLE;
    int data = 0;
    for (int i = 2; i<PKG_LEN-1; i++){
      data = (data << 8) | received[i];
    }
    //Serial.print(data);

    switch(received[1])
    {
      case (COMMAND_SET_VELOCITY):
        motor.set_direction(data);
        data = abs(data);
        motor.set_speed(data);
        interrupt_update();
        state = STATE_VELOCITY;
        break;
      case (COMMAND_SET_POSITION):
        steps = data;
        motor.set_position(steps);
        state = STATE_POSITION;
        break;
    }
  }

  //send echo
  Serial.write(received,PKG_LEN);
}

void read_pkg(unsigned char * received)
{
  int i = 0;
  while(Serial.available()>0)
  {
    received[i] = Serial.read();
    if (i > 0 || received[i] == HEADER){
      i++;
      if (i == PKG_LEN){
        if (received[i-1] == TAIL){ //package OK
          parse(received);
        }
        i = 0;
      }
    }
  }
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
