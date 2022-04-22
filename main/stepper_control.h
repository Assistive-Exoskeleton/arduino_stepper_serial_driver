#ifndef STEPPER_CONTROL__H_
#define STEPPER_CONTROL__H_

#define NUM_PINS 4
#define STEPS_TO_PI 1030
#include "Arduino.h"

class StepperControl{
    public:
        StepperControl(int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4);
        void step();
        void set_direction(int d);
        
    private:
        int motor_pins_[NUM_PINS];
        int i_on_ = 0; //index of pin currently on
        int direction_ = 0;
        void setup();
        void reset_pins();
};

#endif //STEPPER_CONTROL__H_

