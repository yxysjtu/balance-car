#ifndef REMOTE_H
#define REMOTE_H
#include <stdint.h>

typedef enum{
    REMOTE_DISCONNECTED=0,
    REMOTE_STOP,
    REMOTE_START,
    REMOTE_ERROR
} remote_state;

typedef struct{
    remote_state state;
    uint8_t id;

    uint8_t UP;
    uint8_t DOWN;
    uint8_t LEFT;
    uint8_t RIGHT;

    uint8_t Y;
    uint8_t A;
    uint8_t X;
    uint8_t B;

    uint8_t L1;
    uint8_t L2;
    uint8_t R1;
    uint8_t R2;

    uint8_t LX;
    uint8_t LY;
    uint8_t RX;
    uint8_t RY;

    uint8_t START;
    uint8_t SELECT;

} Remote;

//void PS2_readType();


void Remote_init(Remote *this);
void Remote_Handle(Remote *this);

#endif