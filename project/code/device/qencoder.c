#include "qencoder.h"

/*************************** driver layer ************************************/
#include "driver.h"

void QEncoder_init(QEncoder *this);
float QEncoder_get_ang_inc(QEncoder *this); //get increment angle(degree)

void QEncoder_init(QEncoder *this){
	encoder_quad_init(this->encoder_index, this->ENA_pin, this->ENB_pin);  
}

float QEncoder_get_ang_inc(QEncoder *this){
    float cnt = (float)encoder_get_count(this->encoder_index) * 360 / this->angle_div;
    encoder_clear_count(this->encoder_index);
    return cnt;
}
/*****************************************************************************/

