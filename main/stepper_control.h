#ifndef STEPPER_CONTROL__H_
#define STEPPER_CONTROL__H_

#include "Arduino.h"

#define STATE_POSITION 0
#define STATE_VELOCITY 1
#define STATE_IDLE 2

namespace stepper_control
{

class Stepper
{
    public:
        Stepper(const unsigned int max_speed);
        virtual void step() = 0;
        virtual void set_direction(int d) = 0;
        
        void set_speed(int speed);
        void set_position(int pos);
        virtual void set_state(char state);

        int current_speed_ = 0;
        int current_steps_ = 0; //absolute steps
        char state_ = STATE_IDLE;
        double T_step_ = 0;
        double T_current_ = 0;
        int target_steps_ = 0;
        
    protected:
        int direction_ = 0;
        const unsigned int max_speed_;
};

constexpr int CHEAP_PINS = 4;
constexpr int CHEAP_MAX_SPEED = 412;
class CheapStepper : public Stepper
{
    public:
        CheapStepper(int pin0, int pin1, int pin2, int pin3);
        void CheapStepper::set_direction(int d) override;
        void step() override;

    private:
        int pins_[CHEAP_PINS];
        int i_on_ = 0; //index of pin currently on    
};

constexpr int NANO_MAX_SPEED = 3200;
class NanoStepper : public Stepper
{
    public:
        NanoStepper(int pin_pul, int pin_dir, int pin_enable);
        void NanoStepper::set_direction(int d) override;
        void step() override;
        void set_state(char state) override;

    private:
        int pin_pul_;
        int pin_dir_;
        int pin_enable_;
};

} // namespace stepper_control
#endif //STEPPER_CONTROL__H_