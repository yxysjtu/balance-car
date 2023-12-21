#include "remote.h"
//#include "ps2.h"

static uint8_t poll_data[] = {0x01,0x42,0,0,0,0,0,0,0};
static uint8_t enter_config[]={0x01,0x43,0x00,0x01,0x00,0x00,0x00,0x00,0x00};
static uint8_t set_mode[]={0x01,0x44,0x00,0x01,0xEE,0x00,0x00,0x00,0x00};
static uint8_t exit_config[]={0x01,0x43,0x00,0x00,0x5A,0x5A,0x5A,0x5A,0x5A};
static uint8_t type_read[]={0x01,0x45,0x00,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};
uint8_t rx_data[9] = {0}; 
#define SPI_FREQ    100*1000

#include "driver.h"

/**************************** SPI DRIVER**************************************/

void SPI_transfer(uint8_t *tx_data, uint8_t *rx_data, uint32_t len){
    //TODO DMA
    spi_write_itr(SPI_1, tx_data, rx_data, len, 1);
}

/*************************************************************************************************/

/************************************* PS2 DRIVER *************************************************************/
void PS2_ShortPoll(void)
{
    SPI_transfer(poll_data, rx_data, 5);
}
//进入配置
void PS2_EnterConfing(void)
{
    SPI_transfer(enter_config, rx_data, 9);
}
//发送模式设置
void PS2_TurnOnAnalogMode(void)
{
	SPI_transfer(set_mode, rx_data, 9);
}
//完成并保存配置
void PS2_ExitConfing(void)
{
    SPI_transfer(exit_config, rx_data, 9);
}
void PS2_readType(){
    //PS2_ShortPoll();
	//PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfing();		//进入配置模式
    SPI_transfer(type_read, rx_data, 9);
    PS2_ExitConfing();		//完成并保存配置
}

//手柄配置初始化
void PS2_SetInit(void)
{
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfing();		//进入配置模式
	PS2_TurnOnAnalogMode();	//“红绿灯”配置模式，并选择是否保存
	//PS2_VibrationMode();	//开启震动模式
	PS2_ExitConfing();		//完成并保存配置
}

void PS2_getData(){
	SPI_transfer(poll_data, rx_data, 9);
}

/*******************************************************************************************/



void Remote_init(Remote *this){
    //spi_init(SPI_1, SPI_MODE2, SPI_FREQ, SPI1_SCK_D12, SPI1_MOSI_D14, SPI1_MISO_D15, SPI1_CS0_D13);
    SPI_init(SPI_1, SPI_MODE2, SPI_FREQ, 1, 16000, SPI1_SCK_D12, SPI1_MOSI_D14, SPI1_MISO_D15, SPI1_CS0_D13);
    
    //PS2_SetInit();
}
void Remote_Handle(Remote *this){

    //TODO split transfer and decod, one in another loop, one in mainloop
    //decode
    switch (rx_data[1])
    {
    case 0x41: this->state = REMOTE_STOP; break;
    case 0x73: this->state = REMOTE_START; break;
    case 0xff: this->state = REMOTE_DISCONNECTED; break;
    default: this->state = REMOTE_ERROR;
    }

    if(rx_data[2] == 0x5a){
        this->SELECT = !GETBIT(rx_data[3], 0);
        this->START = !GETBIT(rx_data[3], 3);
        this->UP = !GETBIT(rx_data[3], 4);
        this->RIGHT = !GETBIT(rx_data[3], 5);
        this->DOWN = !GETBIT(rx_data[3], 6);
        this->LEFT = !GETBIT(rx_data[3], 7);

        this->L2 = !GETBIT(rx_data[4], 0);
        this->R2 = !GETBIT(rx_data[4], 1);
        this->L1 = !GETBIT(rx_data[4], 2);
        this->R1 = !GETBIT(rx_data[4], 3);
        this->Y = !GETBIT(rx_data[4], 4);
        this->B = !GETBIT(rx_data[4], 5);
        this->A = !GETBIT(rx_data[4], 6);
        this->X = !GETBIT(rx_data[4], 7);

        this->RX = rx_data[5];
        this->RY = 0xff - rx_data[6];
        this->LX = rx_data[7];
        this->LY = 0xff - rx_data[8];

    }

    SPI_transfer(poll_data, rx_data, 9);

}

