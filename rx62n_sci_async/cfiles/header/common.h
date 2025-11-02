/* **** Motor parameter **** */
#pragma once

struct MotorParams {
    int speed;
    float p;
    float i;
    float d;
    
    int address;
    int len;
};
