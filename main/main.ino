#include "stepper_control.h"

/*=========
  INTERRUPT
  =========*/
#define F_TIMER 10000
#define T_TIMER 0.0001
#define F_CLOCK 16000000

/*====================
  SERIAL COMMUNICATION
  ====================*/
#define HEADER 's'
#define TAIL 'e'
#define WHOAMI "Da"
#define PKG_LEN 6 // ['s', motor_ID, command, data1, data0, 'e']
#define COMMAND_SET_VELOCITY 'v'
#define COMMAND_SET_POSITION 'p'
#define COMMAND_GET_POSITION 'I'
#define COMMAND_WHOAMI 'w'

#define NUM_MOTORS 2


unsigned char received[PKG_LEN] = {0};

stepper_control::Stepper *motors[NUM_MOTORS] = {new stepper_control::CheapStepper(8,9,10,11), new stepper_control::NanoStepper(6,7,5)};
void setup() {
  interrupt_init();
  Serial.begin(115200);
}

void loop() {
  read_pkg(received);
  delay(5);
}

ISR(TIMER1_COMPA_vect){
  for (int i = 0; i<NUM_MOTORS; i++){
    motors[i]->step_if(T_TIMER);
  }
}

void parse(unsigned char * received)
{
  int id = received[1];
  if (received[2] == COMMAND_WHOAMI){
    for (int i = 3; i<PKG_LEN-1; i++){
      received[i] = WHOAMI[i-3]; //send whoami as data of echo
    }
  }
  else if (received[2] == COMMAND_GET_POSITION){
    int current_pos = motors[id]->current_steps_;
    for (int i = PKG_LEN-2; i>=3; i--){
      received[i] = current_pos & 0xFF;
      current_pos >>= 8;
    }
  }
  else{
    motors[id]->set_state(STATE_IDLE);
    int data = 0;
    for (int i = 3; i<PKG_LEN-1; i++){
      data = (data << 8) | received[i];
    }
    switch(received[2])
    {
      case (COMMAND_SET_VELOCITY):
        motors[id]->set_speed(data);
        break;
      case (COMMAND_SET_POSITION):
        motors[id]->set_position(data);
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
  OCR1A = F_CLOCK/ (1*F_TIMER)-1;//(must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 1 prescaler
  TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts
}
