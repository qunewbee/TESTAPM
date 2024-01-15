#ifndef __UART_H 
#define __UART_H

////////////////////////////////////��ʱ������ֵ
sfr16 RCAP2    = 0xca;                 // Timer2 capture/reload
sfr16 TMR2     = 0xcc;                 // Timer2

///////////////////////////////��������ϵͳʱ������ֵ
#define BAUDRATE     9600    // Baud rate of UART in bps
#define SYSTEMCLOCK       (22118400L)
#define UART_BUFFERSIZE 64

///////////////////////////////�������Ͷ���
typedef unsigned char   uint8;
typedef signed   char   int8;
typedef unsigned short  uint16;
typedef signed   short  int16;
typedef unsigned int    uint32;
typedef signed   int    int32;

//////////////////////////////////////ȫ�ֱ�������
extern unsigned char UART_Buffer[5];
extern unsigned char UART_Buffer_Size;						
extern unsigned char Upload_Flag;
extern unsigned char Selfcheck_Flag;//���̵����Ƿ�����
extern unsigned char Loopcheck_Flag;//���޵����,������Լ죨������Ƶ�״̬��
extern unsigned char Fire_Flag;

extern unsigned char StatusCode;//bit0-7:����״̬���Լ�״̬��װ��1��װ��2������1,������2���Ƶ�1���Ƶ�2


extern xdata  uint8 UART0_Receive_Buff[5];    

////////////////////////////////////////��������(�жϺ�����������)
void OSCILLATOR_Init (void);
void PORT_Init (void);
void UART0_Init (void);
void UART1_Init (void);
void delay_us(uint32 delay_us );
void delay_ms(uint16 delay_ms );

///////////////////////////////////////�ṹ������


#endif /* end __UART_H */