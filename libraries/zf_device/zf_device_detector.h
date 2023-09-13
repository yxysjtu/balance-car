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

#ifndef _zf_device_detector_h_
#define _zf_device_detector_h_

#include "zf_common_typedef.h"
#include "zf_common_debug.h"

// �������FIFO��С
#define DETECTOR_BUFFER_SIZE             ( 0x16 )

// ����ʾ���������ͨ������
#define DETECTOR_SET_OSCILLOSCOPE_COUNT  ( 0x08 )

// ����������Ե����ͨ������
#define DETECTOR_SET_PARAMETR_COUNT      ( 0x08 )

// ��Ƭ������λ�����͵�
#define DETECTOR_SEND_HEAD               ( 0xAA )

// ����ͷ��
#define DETECTOR_CAMERA_FUNCTION         ( 0x02 ) 
#define DETECTOR_CAMERA_DOT_FUNCTION     ( 0x03 ) 
#define DETECTOR_CAMERA_OSCILLOSCOPE     ( 0x10 )

// ��λ������Ƭ�����͵�
#define DETECTOR_RECEIVE_HEAD            ( 0x55 )
#define DETECTOR_RECEIVE_SET_PARAMETER   ( 0x20 )

#define DETECTOR_UART                    DEBUG_UART_INDEX
//#define DETECTOR_UART_BAUDRATE           DEBUG_UART_BAUDRATE
//#define DETECTOR_UART_TX_PIN             DEBUG_UART_TX_PIN
//#define DETECTOR_UART_RX_PIN             DEBUG_UART_RX_PIN

// ���ݷ����豸ö��
typedef enum
{
    DETECTOR_WIRED_UART,        // ���ߴ���    ʹ�õĴ�����DETECTOR_UART�궨��ָ��
    DETECTOR_WIRELESS_UART,     // ����ת����
    DETECTOR_CH9141,            // 9141����
    DETECTOR_WIFI_UART,         // WIFIת����
    DETECTOR_WIFI_SPI,          // ����WIFI SPI
    DETECTOR_CUSTOM,            // �Զ���ͨѶ��ʽ ��Ҫ����detector_custom_transfer������ʵ�����ݷ���
}detector_transfer_type_enum;


// ����ͷ����ö��
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
    uint8 channel_num;          // ͨ������
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
    uint8 border_num;           // ����λ��ʾ�߽����� ����λ��ʾ�Ƿ���ͼ������  ����0x13������3��ʾһ��ͼ���������߽磨ͨ������߽硢���ߡ��ұ߽磩��1��ʾû��ͼ������
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
    uint8 border_id;            // �߽���
    uint8 dot_type;             // ������  BIT0:1��������16λ��  0��������8λ��  BIT4:0:ֻ��X����   1��X��Y���궼��
    uint16 dot_num;             // ��������
}detector_camera_dot_struct;

typedef struct
{
    uint8 head;
    uint8 function;
    uint8 channel;              // ͨ��
    uint8 check_sum;            // ��У��
    float data;                 // ����
}detector_parameter_struct;











void    detector_oscilloscope_data_send     (uint8 channel_num, float data1, float data2, float data3, float data4, float data5, float data6, float data7, float data8);
void    detector_camera_data_send           (detector_camera_type_enum camera_type, void *image_addr, uint8 border_num, uint16 width, uint16 height);
void    detector_camera_dot_send            (uint8 border_id, uint16 dot_num, void *dot_x, void *dot_y, uint16 width, uint16 height);

//-------------------------------------------------------------------------------------------------------------------
// �������     �δ�ͷ�������ͷͼ�񲢸��������߽磨ֻ�к����꣩��Ϣ
// ����˵��     camera_type     ͼ������
// ����˵��     image_addr      ͼ���ַ
// ����˵��     width           ͼ����
// ����˵��     height          ͼ��߶�
// ����˵��     dot_x           ��ű߽������ĵ�ַ
// ���ز���     void
// ʹ��ʾ��     tft180_displayimage03x(mt9v03x_image[0], 94, 60);
// ��ע��Ϣ     ���Ҫ��ʾ��ֵ��ͼ���ȥ���� tft180_show_gray_image ����
//-------------------------------------------------------------------------------------------------------------------
#define detector_camera_with_3_x_border(camera_type, image_addr, width, height, dot_x1, dot_x2, dot_x3) detector_camera_data_send(camera_type, image_addr, 3, width, height);\
                                                                                                        detector_camera_dot_send(0, height, dot_x1, NULL, width, height);\
                                                                                                        detector_camera_dot_send(1, height, dot_x2, NULL, width, height);\
                                                                                                        detector_camera_dot_send(2, height, dot_x3, NULL, width, height);





void    detector_data_analysis              (void);
void    detector_init                       (detector_transfer_type_enum transfer_type);



#endif

