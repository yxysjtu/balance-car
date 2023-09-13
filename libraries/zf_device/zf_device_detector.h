/*********************************************************************************************************************
* RT1064DVL6A Opensourec Library 即（RT1064DVL6A 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
* 
* 本文件是 RT1064DVL6A 开源库的一部分
* 
* RT1064DVL6A 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
* 
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
* 
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
* 
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
* 
* 文件名称          zf_device_detector
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          IAR 8.32.4 or MDK 5.33
* 适用平台          RT1064DVL6A
* 店铺链接          https://seekfree.taobao.com/
* 
* 修改记录
* 日期              作者                备注
* 2022-09-21        SeekFree            first version
********************************************************************************************************************/

#ifndef _zf_device_detector_h_
#define _zf_device_detector_h_

#include "zf_common_typedef.h"
#include "zf_common_debug.h"

// 定义接收FIFO大小
#define DETECTOR_BUFFER_SIZE             ( 0x16 )

// 定义示波器的最大通道数量
#define DETECTOR_SET_OSCILLOSCOPE_COUNT  ( 0x08 )

// 定义参数调试的最大通道数量
#define DETECTOR_SET_PARAMETR_COUNT      ( 0x08 )

// 单片机往上位机发送的
#define DETECTOR_SEND_HEAD               ( 0xAA )

// 摄像头类
#define DETECTOR_CAMERA_FUNCTION         ( 0x02 ) 
#define DETECTOR_CAMERA_DOT_FUNCTION     ( 0x03 ) 
#define DETECTOR_CAMERA_OSCILLOSCOPE     ( 0x10 )

// 上位机往单片机发送的
#define DETECTOR_RECEIVE_HEAD            ( 0x55 )
#define DETECTOR_RECEIVE_SET_PARAMETER   ( 0x20 )

#define DETECTOR_UART                    DEBUG_UART_INDEX
//#define DETECTOR_UART_BAUDRATE           DEBUG_UART_BAUDRATE
//#define DETECTOR_UART_TX_PIN             DEBUG_UART_TX_PIN
//#define DETECTOR_UART_RX_PIN             DEBUG_UART_RX_PIN

// 数据发送设备枚举
typedef enum
{
    DETECTOR_WIRED_UART,        // 有线串口    使用的串口由DETECTOR_UART宏定义指定
    DETECTOR_WIRELESS_UART,     // 无线转串口
    DETECTOR_CH9141,            // 9141蓝牙
    DETECTOR_WIFI_UART,         // WIFI转串口
    DETECTOR_WIFI_SPI,          // 高速WIFI SPI
    DETECTOR_CUSTOM,            // 自定义通讯方式 需要自行detector_custom_transfer函数中实现数据发送
}detector_transfer_type_enum;


// 摄像头类型枚举
typedef enum
{
    DETECTOR_OV7725_BIN = 1,
    DETECTOR_MT9V03X,
    DETECTOR_SCC8660,
}detector_camera_type_enum;


typedef struct
{
    uint8 head;
    uint8 length1;
    uint8 length2;
    uint8 length3;
    uint8 function;
    uint8 reserve;
    uint8 channel_num;          // 通道数量
    uint8 check_sum;
    float data[DETECTOR_SET_OSCILLOSCOPE_COUNT];
}detector_oscilloscope_struct;


typedef struct
{
    uint8 head;
    uint8 length1;
    uint8 length2;
    uint8 length3;
    uint8 function;
    uint8 camera_type;
    uint8 border_num;           // 低四位表示边界数量 第四位表示是否有图像数据  例如0x13：其中3表示一副图像有三条边界（通常是左边界、中线、右边界）、1表示没有图像数据
    uint16 image_width;
    uint16 image_height;
}detector_camera_struct;


typedef struct
{
    uint8 head;
    uint8 length1;
    uint8 length2;
    uint8 length3;
    uint8 function;
    uint8 border_id;            // 边界编号
    uint8 dot_type;             // 点类型  BIT0:1：坐标是16位的  0：坐标是8位的  BIT4:0:只有X坐标   1：X和Y坐标都有
    uint16 dot_num;             // 画点数量
}detector_camera_dot_struct;

typedef struct
{
    uint8 head;
    uint8 function;
    uint8 channel;              // 通道
    uint8 check_sum;            // 和校验
    float data;                 // 数据
}detector_parameter_struct;











void    detector_oscilloscope_data_send     (uint8 channel_num, float data1, float data2, float data3, float data4, float data5, float data6, float data7, float data8);
void    detector_camera_data_send           (detector_camera_type_enum camera_type, void *image_addr, uint8 border_num, uint16 width, uint16 height);
void    detector_camera_dot_send            (uint8 border_id, uint16 dot_num, void *dot_x, void *dot_y, uint16 width, uint16 height);

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滴答客发送摄像头图像并附带三条边界（只有横坐标）信息
// 参数说明     camera_type     图像数组
// 参数说明     image_addr      图像地址
// 参数说明     width           图像宽度
// 参数说明     height          图像高度
// 参数说明     dot_x           存放边界横坐标的地址
// 返回参数     void
// 使用示例     tft180_displayimage03x(mt9v03x_image[0], 94, 60);
// 备注信息     如果要显示二值化图像就去调用 tft180_show_gray_image 函数
//-------------------------------------------------------------------------------------------------------------------
#define detector_camera_with_3_x_border(camera_type, image_addr, width, height, dot_x1, dot_x2, dot_x3) detector_camera_data_send(camera_type, image_addr, 3, width, height);\
                                                                                                        detector_camera_dot_send(0, height, dot_x1, NULL, width, height);\
                                                                                                        detector_camera_dot_send(1, height, dot_x2, NULL, width, height);\
                                                                                                        detector_camera_dot_send(2, height, dot_x3, NULL, width, height);





void    detector_data_analysis              (void);
void    detector_init                       (detector_transfer_type_enum transfer_type);



#endif

