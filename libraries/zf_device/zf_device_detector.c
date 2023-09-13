/*********************************************************************************************************************
* RT1064DVL6A Opensourec Library ����RT1064DVL6A ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
* 
* ���ļ��� RT1064DVL6A ��Դ���һ����
* 
* RT1064DVL6A ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
* 
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
* 
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
* 
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
* 
* �ļ�����          zf_device_detector
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          IAR 8.32.4 or MDK 5.33
* ����ƽ̨          RT1064DVL6A
* ��������          https://seekfree.taobao.com/
* 
* �޸ļ�¼
* ����              ����                ��ע
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

detector_transfer_callback_function     detector_transfer_callback;                         // ���ݷ��ͺ���ָ��
detector_receive_callback_function      detector_receive_callback;                          // ���ݽ��պ���ָ��
                    
                    
static detector_oscilloscope_struct     detector_oscilloscope_data;                         // ����ʾ��������
static detector_camera_struct           detector_camera_data;                               // ͼ����λ��Э������
static detector_camera_dot_struct       detector_camera_dot_data;                           // ͼ����λ�����Э������
static fifo_struct                      detector_fifo;
static uint8                            detector_buffer[DETECTOR_BUFFER_SIZE];              // ���ݴ������
static float                            detector_parameter[DETECTOR_SET_PARAMETR_COUNT];    // ��������

//-------------------------------------------------------------------------------------------------------------------
// �������     �δ�����ߴ��ڷ��ͺ���
// ����˵��     *buff           ��Ҫ���͵����ݵ�ַ
// ����˵��     length          ��Ҫ���͵ĳ���
// ���ز���     uint32          ʣ��δ�������ݳ���
// ʹ��ʾ��     
//-------------------------------------------------------------------------------------------------------------------
uint32 detector_wired_uart_transfer(const uint8 *buff, uint32 length)
{
    uart_write_buffer(DETECTOR_UART, buff, length);
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �δ�����ߴ��ڽ��պ���
// ����˵��     *buff           ��Ҫ���յ����ݵ�ַ
// ����˵��     length          ��Ҫ���յĳ���
// ���ز���     uint32          ʵ�ʽ��ճ���
// ʹ��ʾ��     
//-------------------------------------------------------------------------------------------------------------------
uint32 detector_wired_uart_receive(uint8 *buff, uint32 length)
{
    //uart_write_buffer(DETECTOR_UART, buff, length);
    // ��Ҫ����ʵ�����ݽ���
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �δ�ͷ��ͺ���
// ����˵��     *send_data      ��Ҫ���͵����ݵ�ַ
// ����˵��     send_length     ��Ҫ���͵ĳ���
// ���ز���     uint32          ʣ��δ�������ݳ���
// ʹ��ʾ��     
//-------------------------------------------------------------------------------------------------------------------
static uint32 detector_transfer(void *send_data, uint32 send_length)
{
    return detector_transfer_callback((const uint8 *)send_data, send_length);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �δ����ͺ���
// ����˵��     *buffer         ��ҪУ������ݵ�ַ
// ����˵��     length          У�鳤��
// ���ز���     uint8           ��ֵ
// ʹ��ʾ��     
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
// �������     �δ�� ����ʾ�������ͺ���
// ����˵��     channel_num     ��Чͨ������
// ����˵��     data1           ͨ��1������
// ����˵��     data2           ͨ��2������
// ����˵��     data3           ͨ��3������
// ����˵��     data4           ͨ��4������
// ����˵��     data5           ͨ��5������
// ����˵��     data6           ͨ��6������
// ����˵��     data7           ͨ��7������
// ����˵��     data8           ͨ��8������
// ���ز���     void           
// ʹ��ʾ��     detector_oscilloscope_data_send(3, 13.0, 10.2, 10, 0, 0, 0, 0, 0);
//-------------------------------------------------------------------------------------------------------------------
void detector_oscilloscope_data_send(uint8 channel_num, float data1, float data2, float data3, float data4, float data5, float data6, float data7, float data8)
{
    uint32 packet_size;
    
    // ֡ͷ
    detector_oscilloscope_data.head         = DETECTOR_SEND_HEAD;

    // д���������Ϣ
    packet_size                             = sizeof(detector_oscilloscope_struct) - (DETECTOR_SET_OSCILLOSCOPE_COUNT - channel_num) * 4;
    detector_oscilloscope_data.length3      = (packet_size >> 0x10) & 0xff;
    detector_oscilloscope_data.length2      = (packet_size >> 0x08) & 0xff;
    detector_oscilloscope_data.length1      = (packet_size >> 0x00) & 0xff;
    
    detector_oscilloscope_data.function     = DETECTOR_CAMERA_OSCILLOSCOPE;
    detector_oscilloscope_data.reserve      = 0;
    detector_oscilloscope_data.channel_num  = channel_num;
    
    // ��У�����
    detector_oscilloscope_data.check_sum    = 0;
    detector_oscilloscope_data.check_sum    = detector_sum((uint8 *)&detector_oscilloscope_data, packet_size);
    
    // д����Ҫ���͵�����
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
// �������     �δ�� ͼ���ͺ���
// ����˵��     camera_type     ����ͷ����
// ����˵��     *image_addr     ͼ���׵�ַ
// ����˵��     border_num      ͼ���а����߽�����
// ����˵��     width           ͼ����
// ����˵��     height          ͼ��߶�
// ���ز���     void           
// ʹ��ʾ��     
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
    
    // д���������Ϣ
    detector_camera_data.length3            = (packet_size >> 0x10) & 0xff;
    detector_camera_data.length2            = (packet_size >> 0x08) & 0xff;
    detector_camera_data.length1            = (packet_size >> 0x00) & 0xff;

    detector_camera_data.function           = DETECTOR_CAMERA_FUNCTION;
    detector_camera_data.camera_type        = camera_type;
    detector_camera_data.border_num         = border_num;
    detector_camera_data.image_width        = width;
    detector_camera_data.image_height       = height;

    // �ж��Ƿ���Ҫ����ԭʼͼ�� �� ��������
    if(NULL == image_addr)
    {
        detector_camera_data.border_num |= 0x10;
    }
    
    // ���ȷ���֡ͷ�����ܡ�����ͷ���͡��Լ���ȸ߶ȵ���Ϣ
    detector_transfer(&detector_camera_data, sizeof(detector_camera_struct));
    
    // ����ͼ������
    if(NULL != image_addr)
    {
        detector_transfer(image_addr, packet_size);
    }
    
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �δ�� ͼ����߻��ƺ���
// ����˵��     border_id       ����ID
// ����˵��     dot_num         ������
// ����˵��     *dot_x          �����������׵�ַ
// ����˵��     *dot_y          �����������׵�ַ
// ����˵��     width           ͼ����
// ����˵��     height          ͼ��߶�
// ���ز���     void           
// ʹ��ʾ��     
//-------------------------------------------------------------------------------------------------------------------
void detector_camera_dot_send(uint8 border_id, uint16 dot_num, void *dot_x, void *dot_y, uint16 width, uint16 height)
{
    uint32 packet_size;
    uint16 dot_bytes;
    detector_camera_dot_data.head           = DETECTOR_SEND_HEAD;

    detector_camera_dot_data.dot_type       = 0;
    if(NULL != dot_y)
    {
        detector_camera_dot_data.dot_type |= 0x10;  // ����X��Y����������
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
        // ֻ����X��������
        dot_bytes = dot_num;
        if(255 < width)
        {
            detector_camera_dot_data.dot_type |= 0x01;
            dot_bytes *= 2;
        }
        packet_size = sizeof(detector_camera_dot_struct) + dot_bytes;
    }
    
    // д���������Ϣ
    detector_camera_dot_data.length3        = (packet_size >> 0x10) & 0xff;
    detector_camera_dot_data.length2        = (packet_size >> 0x08) & 0xff;
    detector_camera_dot_data.length1        = (packet_size >> 0x00) & 0xff;
    
    detector_camera_dot_data.function       = DETECTOR_CAMERA_DOT_FUNCTION;
    detector_camera_dot_data.border_id      = border_id;
    detector_camera_dot_data.dot_num        = dot_num;

    // ���ȷ���֡ͷ�����ܡ��߽��š����곤�ȡ������
    detector_transfer(&detector_camera_dot_data, sizeof(detector_camera_dot_struct));
    
    // ���ͺ���������
    detector_transfer(dot_x, dot_bytes);
    
    // �ж��Ƿ�������������
    if(NULL != dot_y)
    {
        // ���û�����������ݣ����ʾÿһ��ֻ��һ���߽�
        // ָ���˺����������ݣ����ַ�ʽ����ʵ��ͬһ�ж���߽����������������㷨�ܹ����������䡣
        detector_transfer(dot_y, dot_bytes);
    }
}

void detector_data_analysis(void)
{
    uint8  temp_sum;
    uint32 read_length;
    detector_parameter_struct *receive_packet;
    ALIGN(4) uint8  temp_buffer[DETECTOR_BUFFER_SIZE];

    // ���Զ�ȡ����
    read_length = detector_receive_callback(temp_buffer, DETECTOR_BUFFER_SIZE);
    if(read_length)
    {
        // ����ȡ��������д��FIFO
        fifo_write_buffer(&detector_fifo, temp_buffer, read_length);
    }
    
    while(sizeof(detector_parameter_struct) <= fifo_used(&detector_fifo))
    {
        read_length = sizeof(detector_parameter_struct);
        fifo_read_buffer(&detector_fifo, temp_buffer, &read_length, FIFO_READ_ONLY);
        
        if(DETECTOR_RECEIVE_HEAD != temp_buffer[0])
        {
            // û��֡ͷ���FIFO��ȥ����һ������
            read_length = 1;
            fifo_read_buffer(&detector_fifo, NULL, &read_length, FIFO_READ_AND_CLEAN);
        }
        else
        {
            // �ҵ�֡ͷ
            receive_packet = (detector_parameter_struct *)temp_buffer;
            temp_sum = receive_packet->check_sum;
            receive_packet->check_sum = 0;
            if(temp_sum == detector_sum(temp_buffer, sizeof(detector_parameter_struct)))
            {
                // ��У��ɹ���������
                detector_parameter[receive_packet->channel] = receive_packet->data;
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------------------------
// �������     �δ���Զ��巢�ͺ���
// ����˵��     *buff           ��Ҫ���͵����ݵ�ַ
// ����˵��     length          ��Ҫ���͵ĳ���
// ���ز���     uint32          ʣ��δ�������ݳ���
// ʹ��ʾ��     ������ݴ��䷽ʽ������֧�ַ�Χ���������ʵ��
//-------------------------------------------------------------------------------------------------------------------
uint32 detector_custom_transfer(const uint8 *buff, uint32 length)
{
    uint32 send_length;
    send_length = length;
    // ����ʵ�����ݷ���

    return send_length;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �δ���Զ�����պ���
// ����˵��     *buff           ��Ҫ���͵����ݵ�ַ
// ����˵��     length          ��Ҫ���͵ĳ���
// ���ز���     uint32          ʵ�ʽ��յĳ���
// ʹ��ʾ��     
//-------------------------------------------------------------------------------------------------------------------
uint32 detector_custom_receive(uint8 *buff, uint32 length)
{
    uint32 receive_length;
    receive_length = length;
    // ����ʵ�����ݽ���

    return receive_length;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �δ�� ��ʼ��
// ����˵��     transfer_type   ѡ��ʹ�����ַ�ʽ��������
// ���ز���     void            
// ʹ��ʾ��     
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
            // ���ߴ����ݲ�֧�ֽ������ݣ�������ʵ��detector_wired_uart_receive����
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
            // �����Լ������� ����ʵ��detector_custom_transfer������������ݵĴ���
            detector_transfer_callback = detector_custom_transfer;
            detector_receive_callback = detector_custom_receive;
        }break;
    }
}