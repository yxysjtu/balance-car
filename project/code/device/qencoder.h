#ifndef QENCODER_H
#define QENCODER_H

#include <stdint.h>

typedef struct{
    uint8_t encoder_index;
    uint8_t ENA_pin;
    uint8_t ENB_pin;
    int16_t angle_div;
} QEncoder;

void QEncoder_init(QEncoder *this);
float QEncoder_get_ang_inc(QEncoder *this); //get increment angle(degree)

#endif

