#include "stepper_control.h"

namespace stepper_control
{
    /**************** STEPPER ****************/
    Stepper::Stepper(const unsigned int max_speed)
    : max_speed_(max_speed)
    {}
    void Stepper::set_speed(int speed)
    {
        set_direction(speed);
        if (speed != 0){
            speed = abs(speed);
            if (speed > max_speed_){
                speed = max_speed_;
            }
            current_speed_ = speed;
            T_current_ = 0;
            T_step_ = 1.0/current_speed_;
            set_state(STATE_VELOCITY);
        }
        else{
            set_state(STATE_IDLE);
        }
        
    }
    void Stepper::set_position(int pos)
    {
        target_steps_ = pos;
        set_direction(pos - current_steps_);
        if (direction_ != 0)
            set_state(STATE_POSITION);
        else{
            set_state(STATE_IDLE);
        }
    }
    void Stepper::set_state(char state)
    {
      state_ = state;
    }
    
    void Stepper::step_if(double t_timer){
        if (state_ != STATE_IDLE){
            if (state_ == STATE_POSITION && current_steps_ == target_steps_){
                set_state(STATE_IDLE);
            }
            else{
                T_current_ += t_timer;
                if (T_current_ >= T_step_){
                    T_current_ -= T_step_;
                    step();
                }
            }
        }
    }

    /**************** CHEAP STEPPER ****************/
    CheapStepper::CheapStepper(int pin0, int pin1, int pin2, int pin3)
    : Stepper(CHEAP_MAX_SPEED)
    {
        pins_[0] = pin0;
        pins_[1] = pin1;
        pins_[2] = pin2;
        pins_[3] = pin3;
        for (int i = 0; i < CHEAP_PINS; i++){
            pinMode(pins_[i], OUTPUT);
            digitalWrite(pins_[i], LOW);
        }
    }
    void CheapStepper::set_direction(int d)
    {
        if (d == 0)
            direction_ = 0;
        else
            direction_ = d > 0 ? 1 : -1; 
    }
    void CheapStepper::step()
    {
        digitalWrite(pins_[i_on_], LOW);
        i_on_ = (i_on_ + CHEAP_PINS - direction_) % CHEAP_PINS; //cycle in the provided direction
        digitalWrite(pins_[i_on_], HIGH);
        current_steps_ += direction_;
    }

    /**************** NANO STEPPER ****************/
    NanoStepper::NanoStepper(int pin_pul, int pin_dir, int pin_enable)
    : Stepper(NANO_MAX_SPEED)
    {
        pin_pul_ = pin_pul;
        pin_dir_ = pin_dir;
        pin_enable_ = pin_enable;
        pinMode(pin_pul_, OUTPUT);
        pinMode(pin_dir_, OUTPUT);
        pinMode(pin_enable_, OUTPUT);
        digitalWrite(pin_pul_, HIGH);
        digitalWrite(pin_dir_, HIGH);
        digitalWrite(pin_enable_, LOW);
    }

    void NanoStepper::set_direction(int d)
    {
        if (d == 0)
            direction_ = 0;
        else if (d > 0){
            direction_ = 1;
            digitalWrite(pin_dir_, LOW);
        }
        else{
            direction_ = -1;
            digitalWrite(pin_dir_,HIGH);
        }
    }
    void NanoStepper::step()
    {
        if (direction_ != 0){
            digitalWrite(pin_pul_,HIGH);
            digitalWrite(pin_pul_, LOW);
            current_steps_ += direction_;
        }
    }

    void NanoStepper::set_state(char state)
    {
        state_ = state;
        if (state_ == STATE_IDLE){
            digitalWrite(pin_enable_, LOW);
        }
        else{
            digitalWrite(pin_enable_, HIGH);
        }
    }

} // namespace stepper_control