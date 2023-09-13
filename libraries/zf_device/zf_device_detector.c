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

#include "zf_driver_uart.h"
#include "zf_common_fifo.h"
#include "zf_device_wireless_uart.h"
#include "zf_device_bluetooth_ch9141.h"
#include "zf_device_wifi_uart.h"
#include "zf_device_wifi_spi.h"

#include "zf_device_detector.h"


typedef uint32 (*detector_transfer_callback_function)   (const uint8 *buff, uint32 length);
typedef uint32 (*detector_receive_callback_function)    (uint8 *buff, uint32 length);

detector_transfer_callback_function     detector_transfer_callback;                         // 数据发送函数指针
detector_receive_callback_function      detector_receive_callback;                          // 数据接收函数指针
                    
                    
static detector_oscilloscope_struct     detector_oscilloscope_data;                         // 虚拟示波器数据
static detector_camera_struct           detector_camera_data;                               // 图像上位机协议数据
static detector_camera_dot_struct       detector_camera_dot_data;                           // 图像上位机打点协议数据
static fifo_struct                      detector_fifo;
static uint8                            detector_buffer[DETECTOR_BUFFER_SIZE];              // 数据存放数组
static float                            detector_parameter[DETECTOR_SET_PARAMETR_COUNT];    // 参数设置

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滴答客有线串口发送函数
// 参数说明     *buff           需要发送的数据地址
// 参数说明     length          需要发送的长度
// 返回参数     uint32          剩余未发送数据长度
// 使用示例     
//-------------------------------------------------------------------------------------------------------------------
uint32 detector_wired_uart_transfer(const uint8 *buff, uint32 length)
{
    uart_write_buffer(DETECTOR_UART, buff, length);
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滴答客有线串口接收函数
// 参数说明     *buff           需要接收的数据地址
// 参数说明     length          需要接收的长度
// 返回参数     uint32          实际接收长度
// 使用示例     
//-------------------------------------------------------------------------------------------------------------------
uint32 detector_wired_uart_receive(uint8 *buff, uint32 length)
{
    //uart_write_buffer(DETECTOR_UART, buff, length);
    // 需要自行实现数据接收
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滴答客发送函数
// 参数说明     *send_data      需要发送的数据地址
// 参数说明     send_length     需要发送的长度
// 返回参数     uint32          剩余未发送数据长度
// 使用示例     
//-------------------------------------------------------------------------------------------------------------------
static uint32 detector_transfer(void *send_data, uint32 send_length)
{
    return detector_transfer_callback((const uint8 *)send_data, send_length);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滴答客求和函数
// 参数说明     *buffer         需要校验的数据地址
// 参数说明     length          校验长度
// 返回参数     uint8           和值
// 使用示例     
//-------------------------------------------------------------------------------------------------------------------
static uint8 detector_sum(uint8 *buffer, uint32 length)
{
    uint8 temp_sum = 0;
    
    while(length--)
    {
        temp_sum += *buffer++;
    }
    
    return temp_sum;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滴答客 虚拟示波器发送函数
// 参数说明     channel_num     有效通道数量
// 参数说明     data1           通道1的数据
// 参数说明     data2           通道2的数据
// 参数说明     data3           通道3的数据
// 参数说明     data4           通道4的数据
// 参数说明     data5           通道5的数据
// 参数说明     data6           通道6的数据
// 参数说明     data7           通道7的数据
// 参数说明     data8           通道8的数据
// 返回参数     void           
// 使用示例     detector_oscilloscope_data_send(3, 13.0, 10.2, 10, 0, 0, 0, 0, 0);
//-------------------------------------------------------------------------------------------------------------------
void detector_oscilloscope_data_send(uint8 channel_num, float data1, float data2, float data3, float data4, float data5, float data6, float data7, float data8)
{
    uint32 packet_size;
    
    // 帧头
    detector_oscilloscope_data.head         = DETECTOR_SEND_HEAD;

    // 写入包长度信息
    packet_size                             = sizeof(detector_oscilloscope_struct) - (DETECTOR_SET_OSCILLOSCOPE_COUNT - channel_num) * 4;
    detector_oscilloscope_data.length3      = (packet_size >> 0x10) & 0xff;
    detector_oscilloscope_data.length2      = (packet_size >> 0x08) & 0xff;
    detector_oscilloscope_data.length1      = (packet_size >> 0x00) & 0xff;
    
    detector_oscilloscope_data.function     = DETECTOR_CAMERA_OSCILLOSCOPE;
    detector_oscilloscope_data.reserve      = 0;
    detector_oscilloscope_data.channel_num  = channel_num;
    
    // 和校验计算
    detector_oscilloscope_data.check_sum    = 0;
    detector_oscilloscope_data.check_sum    = detector_sum((uint8 *)&detector_oscilloscope_data, packet_size);
    
    // 写入需要发送的数据
    detector_oscilloscope_data.data[0]      = data1;
    detector_oscilloscope_data.data[1]      = data2;
    detector_oscilloscope_data.data[2]      = data3;
    detector_oscilloscope_data.data[3]      = data4;
    detector_oscilloscope_data.data[4]      = data5;
    detector_oscilloscope_data.data[5]      = data6;
    detector_oscilloscope_data.data[6]      = data7;
    detector_oscilloscope_data.data[7]      = data8;
    
    detector_transfer((uint8 *)&detector_oscilloscope_data, packet_size);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滴答客 图像发送函数
// 参数说明     camera_type     摄像头类型
// 参数说明     *image_addr     图像首地址
// 参数说明     border_num      图像中包含边界数量
// 参数说明     width           图像宽度
// 参数说明     height          图像高度
// 返回参数     void           
// 使用示例     
//-------------------------------------------------------------------------------------------------------------------
void detector_camera_data_send(detector_camera_type_enum camera_type, void *image_addr, uint8 border_num, uint16 width, uint16 height)
{
    uint32 packet_size;
    
    detector_camera_data.head               = DETECTOR_SEND_HEAD;
    
    switch(camera_type)
    {
        case DETECTOR_OV7725_BIN:
        {
            packet_size = sizeof(detector_camera_struct) + width * height / 8;
        }break;
        
        case DETECTOR_MT9V03X:
        {
            packet_size = sizeof(detector_camera_struct) + width * height;
        }break;
        
        case DETECTOR_SCC8660:
        {
            packet_size = sizeof(detector_camera_struct) + width * height * 2;
        }break;
    }
    
    // 写入包长度信息
    detector_camera_data.length3            = (packet_size >> 0x10) & 0xff;
    detector_camera_data.length2            = (packet_size >> 0x08) & 0xff;
    detector_camera_data.length1            = (packet_size >> 0x00) & 0xff;

    detector_camera_data.function           = DETECTOR_CAMERA_FUNCTION;
    detector_camera_data.camera_type        = camera_type;
    detector_camera_data.border_num         = border_num;
    detector_camera_data.image_width        = width;
    detector_camera_data.image_height       = height;

    // 判断是否需要发送原始图像 与 画线数量
    if(NULL == image_addr)
    {
        detector_camera_data.border_num |= 0x10;
    }
    
    // 首先发送帧头、功能、摄像头类型、以及宽度高度等信息
    detector_transfer(&detector_camera_data, sizeof(detector_camera_struct));
    
    // 发送图像数据
    if(NULL != image_addr)
    {
        detector_transfer(image_addr, packet_size);
    }
    
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滴答客 图像边线绘制函数
// 参数说明     border_id       边线ID
// 参数说明     dot_num         点数量
// 参数说明     *dot_x          横坐标数据首地址
// 参数说明     *dot_y          纵坐标数据首地址
// 参数说明     width           图像宽度
// 参数说明     height          图像高度
// 返回参数     void           
// 使用示例     
//-------------------------------------------------------------------------------------------------------------------
void detector_camera_dot_send(uint8 border_id, uint16 dot_num, void *dot_x, void *dot_y, uint16 width, uint16 height)
{
    uint32 packet_size;
    uint16 dot_bytes;
    detector_camera_dot_data.head           = DETECTOR_SEND_HEAD;

    detector_camera_dot_data.dot_type       = 0;
    if(NULL != dot_y)
    {
        detector_camera_dot_data.dot_type |= 0x10;  // 包含X和Y的坐标数据
        dot_bytes = dot_num;
        if((255 < width) || (255 < height))
        {
            detector_camera_dot_data.dot_type |= 0x01;
            dot_bytes *= 2;
            
        }
        packet_size = sizeof(detector_camera_dot_struct) + dot_bytes * 2;
    }
    else
    {
        // 只包含X坐标数据
        dot_bytes = dot_num;
        if(255 < width)
        {
            detector_camera_dot_data.dot_type |= 0x01;
            dot_bytes *= 2;
        }
        packet_size = sizeof(detector_camera_dot_struct) + dot_bytes;
    }
    
    // 写入包长度信息
    detector_camera_dot_data.length3        = (packet_size >> 0x10) & 0xff;
    detector_camera_dot_data.length2        = (packet_size >> 0x08) & 0xff;
    detector_camera_dot_data.length1        = (packet_size >> 0x00) & 0xff;
    
    detector_camera_dot_data.function       = DETECTOR_CAMERA_DOT_FUNCTION;
    detector_camera_dot_data.border_id      = border_id;
    detector_camera_dot_data.dot_num        = dot_num;

    // 首先发送帧头、功能、边界编号、坐标长度、点个数
    detector_transfer(&detector_camera_dot_data, sizeof(detector_camera_dot_struct));
    
    // 发送横坐标数据
    detector_transfer(dot_x, dot_bytes);
    
    // 判断是否发送纵坐标数据
    if(NULL != dot_y)
    {
        // 如果没有纵坐标数据，则表示每一行只有一个边界
        // 指定了横纵坐标数据，这种方式可以实现同一行多个边界的情况，例如搜线算法能够搜索出回弯。
        detector_transfer(dot_y, dot_bytes);
    }
}

void detector_data_analysis(void)
{
    uint8  temp_sum;
    uint32 read_length;
    detector_parameter_struct *receive_packet;
    ALIGN(4) uint8  temp_buffer[DETECTOR_BUFFER_SIZE];

    // 尝试读取数据
    read_length = detector_receive_callback(temp_buffer, DETECTOR_BUFFER_SIZE);
    if(read_length)
    {
        // 将读取到的数据写入FIFO
        fifo_write_buffer(&detector_fifo, temp_buffer, read_length);
    }
    
    while(sizeof(detector_parameter_struct) <= fifo_used(&detector_fifo))
    {
        read_length = sizeof(detector_parameter_struct);
        fifo_read_buffer(&detector_fifo, temp_buffer, &read_length, FIFO_READ_ONLY);
        
        if(DETECTOR_RECEIVE_HEAD != temp_buffer[0])
        {
            // 没有帧头则从FIFO中去掉第一个数据
            read_length = 1;
            fifo_read_buffer(&detector_fifo, NULL, &read_length, FIFO_READ_AND_CLEAN);
        }
        else
        {
            // 找到帧头
            receive_packet = (detector_parameter_struct *)temp_buffer;
            temp_sum = receive_packet->check_sum;
            receive_packet->check_sum = 0;
            if(temp_sum == detector_sum(temp_buffer, sizeof(detector_parameter_struct)))
            {
                // 和校验成功保存数据
                detector_parameter[receive_packet->channel] = receive_packet->data;
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滴答客自定义发送函数
// 参数说明     *buff           需要发送的数据地址
// 参数说明     length          需要发送的长度
// 返回参数     uint32          剩余未发送数据长度
// 使用示例     如果数据传输方式并不在支持范围则可以自行实现
//-------------------------------------------------------------------------------------------------------------------
uint32 detector_custom_transfer(const uint8 *buff, uint32 length)
{
    uint32 send_length;
    send_length = length;
    // 自行实现数据发送

    return send_length;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滴答客自定义接收函数
// 参数说明     *buff           需要发送的数据地址
// 参数说明     length          需要发送的长度
// 返回参数     uint32          实际接收的长度
// 使用示例     
//-------------------------------------------------------------------------------------------------------------------
uint32 detector_custom_receive(uint8 *buff, uint32 length)
{
    uint32 receive_length;
    receive_length = length;
    // 自行实现数据接收

    return receive_length;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滴答客 初始化
// 参数说明     transfer_type   选择使用哪种方式传输数据
// 返回参数     void            
// 使用示例     
//-------------------------------------------------------------------------------------------------------------------
void detector_init(detector_transfer_type_enum transfer_type)
{
    
    fifo_init(&detector_fifo, FIFO_DATA_8BIT, detector_buffer, DETECTOR_BUFFER_SIZE);
    
    switch(transfer_type)
    {
        case DETECTOR_WIRED_UART:
        {
            detector_transfer_callback = detector_wired_uart_transfer;
            detector_receive_callback = detector_wired_uart_receive;
            // 有线串口暂不支持接收数据，可自行实现detector_wired_uart_receive函数
        }break;
        
        case DETECTOR_WIRELESS_UART:
        {
            detector_transfer_callback = wireless_uart_send_buffer;
            detector_receive_callback = wireless_uart_read_buffer;
        }break;
        
        case DETECTOR_CH9141:
        {
            detector_transfer_callback = bluetooth_ch9141_send_buffer;
            detector_receive_callback = bluetooth_ch9141_read_buffer;
        }break;
        
        case DETECTOR_WIFI_UART:
        {
            detector_transfer_callback = wifi_uart_send_buffer;
            detector_receive_callback = wifi_uart_read_buffer;
        }break;
        
        case DETECTOR_WIFI_SPI:
        {
            detector_transfer_callback = wifi_spi_send_buffer;
            detector_receive_callback = wifi_spi_read_buffer;
        }break;
        
        case DETECTOR_CUSTOM:
        {
            // 根据自己的需求 自行实现detector_custom_transfer函数，完成数据的传输
            detector_transfer_callback = detector_custom_transfer;
            detector_receive_callback = detector_custom_receive;
        }break;
    }
}