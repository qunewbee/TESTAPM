//repo use test 2024年1月15日


#include <C8051F040.h>                 // SFR declarations
#include <stdio.h>
#include <uart.h>

sbit  Volt12_out = P7^0; 
sbit  Volt5_out = P7^1;  
sbit  Lode_1 = P7^2; 
sbit  Lode_2 = P7^3;  

sbit  JC1 = P5^6;  
sbit  JC2 = P5^5;  
sbit  JC3 = P5^7;  

void main (void)
{
	int i;                              // Software timer
	uint16 j;
	unsigned char UART_Temp[5]={0,0,0,0,0};
	uint8 PAGE=SFRPAGE;

	WDTCN = 0xDE;                       // Disable watchdog timer
	WDTCN = 0xAD;

	PORT_Init ();
	OSCILLATOR_Init ();                 // Initialize oscillator
	delay_ms(1000);
	UART0_Init ();                      // Initialize UART0
	SFRPAGE=PAGE;

	EA = 1;//开启所有中断

	Selfcheck_Flag=1;
	Loopcheck_Flag=1;
	StatusCode&=~0xFF;//自检后清除发射状态位和哑弹状态位

   while (1)
   {
		if(Selfcheck_Flag==1)
		{
			PAGE=SFRPAGE;
			SFRPAGE=CONFIG_PAGE;
			Lode_1=0;
			Lode_2=0;
			delay_ms(10);
			if((JC1==0)&&(JC1==0))
			{StatusCode&=~0x02;}//回路正常
			else{StatusCode|=0x02;}//回路故障
			Lode_1=1;
			Lode_2=1;
			SFRPAGE=PAGE;	

			Selfcheck_Flag=0;
			Upload_Flag=1;//回路检测后回复
			
		}else;
		
		if(Loopcheck_Flag==1)
		{
			PAGE=SFRPAGE;
			SFRPAGE=CONFIG_PAGE;
			Lode_1=0;//检测管1
			delay_ms(10);
			if(JC3==0)
			{
				StatusCode|=0x04;
				if(StatusCode&0x10)//如果已发射1管
				{StatusCode|=0x40;}//哑弹
				else{StatusCode&=~0x40;}
			}//有弹
			else{StatusCode&=~0x04;}
			Lode_1=1;

			Lode_2=0;//检测管2
			delay_ms(10);
			if(JC3==0)
			{
				StatusCode|=0x08;
				if(StatusCode&0x20)
				{StatusCode|=0x80;}
				else{StatusCode&=~0x80;}
			}
			else{StatusCode&=~0x08;}
			Lode_2=1;
			SFRPAGE=PAGE;	

			Loopcheck_Flag=0;
			Upload_Flag=1;//自检后回复
		}else;

		if(Fire_Flag==1)
		{
			PAGE=SFRPAGE;
			SFRPAGE=CONFIG_PAGE;
			if(UART_Buffer[2]&0x04)
			{
				Lode_1=0;
				Volt12_out=0;
				Volt5_out=0;
				delay_ms(120);
				Lode_1=1;
				StatusCode|=0x10;//1管已发射
			}else;
			if(UART_Buffer[2]&0x08)
			{
				Lode_2=0;
				Volt12_out=0;
				Volt5_out=0;
				delay_ms(120);
				Lode_2=1;
				StatusCode|=0x20;//2管已发射
			}else;
			

			Lode_1=1;
			Lode_2=1;
			Volt12_out=1;
			Volt5_out=1;
			SFRPAGE=PAGE;	
			
			Fire_Flag=0;
			Loopcheck_Flag=1;//发射后进行自检
		}else;
		
		if(Upload_Flag==1)//接收到信息后,或者状态改变时才回传
		{
			UART_Temp[0]=0xEB;
			UART_Temp[1]=0x90;
			UART_Temp[2]=StatusCode;//控制字
			UART_Temp[3]=0x00;//控制字
			UART_Temp[4]=0x00;//校验字

			for(i=2;i<4;i++)  //校验和
			{
				UART_Temp[4]+=UART_Temp[i];
			}
			for(i=0;i<5;i++)  //回传信息
			{
				SCON0 &= ~0x02;
				SBUF0=UART_Temp[i];
				j=0;
				while(((SCON0&0x02)==0)&&(j<0x0FFF))//避免卡死在此
					{j++;}
			}
			Upload_Flag=0;
			
			SCON0 &= ~0x02;//清除中断标志位，不再进入中断
		}else;
   }
}
                                                                   
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------