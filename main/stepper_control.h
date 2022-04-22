#ifndef STEPPER_CONTROL__H_
#define STEPPER_CONTROL__H_

#define NUM_PINS 4
#define MAX_SPEED 412 // steps/s
#include "Arduino.h"

class StepperControl{
    public:
        StepperControl(int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4);
        void step();
        void set_direction(int d);
        void set_speed(int speed);
        int get_speed();
        void set_position(int pos);
        int get_steps();
        
    private:
        int motor_pins_[NUM_PINS];
        int i_on_ = 0; //index of pin currently on
        int direction_ = 0;
        int current_speed_ = 0;
        int current_steps_ = 0; //absolute steps
        void setup();
        void reset_pins();
};

#endif //STEPPER_CONTROL__H_

