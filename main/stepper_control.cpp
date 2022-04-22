#include "stepper_control.h"

StepperControl::StepperControl(int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4)
{
    motor_pins_[0] = motor_pin_1;
    motor_pins_[1] = motor_pin_2;
    motor_pins_[2] = motor_pin_3;
    motor_pins_[3] = motor_pin_4;    
    setup();
}

void StepperControl::setup()
{
    for (int pin : motor_pins_){
        pinMode(pin, OUTPUT);
    }
    reset_pins();
}

void StepperControl::reset_pins()
{
    for (int pin : motor_pins_){
        digitalWrite(pin, LOW);
    }
}

void StepperControl::set_direction(int d)
{
  if (d == 0)
    direction_ = 0;
  else
    direction_ = d > 0 ? 1 : -1; 
}

void StepperControl::set_speed(int speed)
{
  if (speed > MAX_SPEED){
    speed = MAX_SPEED;
  }
  current_speed_ = speed;
}
int StepperControl::get_speed(){
  return current_speed_;
}

void StepperControl::set_position(int pos)
{
  set_direction(pos - current_steps_);
}

int StepperControl::get_steps()
{
  return current_steps_;
}

void StepperControl::step()
{
    digitalWrite(motor_pins_[i_on_], LOW);
    i_on_ = (i_on_ + NUM_PINS - direction_) % NUM_PINS; //cycle in the provided direction
    digitalWrite(motor_pins_[i_on_], HIGH);

    current_steps_ += direction_;
}