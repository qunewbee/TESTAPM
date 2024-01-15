#ifndef __UART_H 
#define __UART_H

////////////////////////////////////定时器重载值
sfr16 RCAP2    = 0xca;                 // Timer2 capture/reload
sfr16 TMR2     = 0xcc;                 // Timer2

///////////////////////////////波特率与系统时钟设置值
#define BAUDRATE     9600    // Baud rate of UART in bps
#define SYSTEMCLOCK       (22118400L)
#define UART_BUFFERSIZE 64

///////////////////////////////数据类型定义
typedef unsigned char   uint8;
typedef signed   char   int8;
typedef unsigned short  uint16;
typedef signed   short  int16;
typedef unsigned int    uint32;
typedef signed   int    int32;

//////////////////////////////////////全局变量声明
extern unsigned char UART_Buffer[5];
extern unsigned char UART_Buffer_Size;						
extern unsigned char Upload_Flag;
extern unsigned char Selfcheck_Flag;//检测继电器是否正常
extern unsigned char Loopcheck_Flag;//有无弹检测,发射后自检（需更新哑弹状态）
extern unsigned char Fire_Flag;

extern unsigned char StatusCode;//bit0-7:接收状态；自检状态；装填1；装填2，发射1,；发射2；哑弹1；哑弹2


extern xdata  uint8 UART0_Receive_Buff[5];    

////////////////////////////////////////函数声明(中断函数不用声明)
void OSCILLATOR_Init (void);
void PORT_Init (void);
void UART0_Init (void);
void UART1_Init (void);
void delay_us(uint32 delay_us );
void delay_ms(uint16 delay_ms );

///////////////////////////////////////结构体声明


#endif /* end __UART_H */