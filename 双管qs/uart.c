#include <C8051F040.h>                 // SFR declarations
#include <stdio.h>
#include <uart.h>

//unsigned char UART_Buffer[5]={0xF1,0x33,0x30,0x15,0x3C};
unsigned char UART_Buffer[5]={0,0,0,0,0};
unsigned char UART_Buffer_Size = 0;
unsigned char Upload_Flag = 0;//�ش�
unsigned char Selfcheck_Flag = 0;
unsigned char Loopcheck_Flag = 0;
unsigned char Fire_Flag = 0;

unsigned char StatusCode = 0;

//-----------------------------------------------------------------------------
// UART0_Init   Variable baud rate, Timer 2, 8-N-1
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure UART0 for operation at <BAUDRATE> 8-N-1 using Timer2 as
// baud rate source.
//
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   char SFRPAGE_SAVE;

   SFRPAGE_SAVE = SFRPAGE;             // Preserve SFRPAGE

   SFRPAGE = TMR2_PAGE;

   TMR2CN = 0x00;                      // Timer in 16-bit auto-reload up timer
                                       // mode
   TMR2CF = 0x08;                      // SYSCLK is time base; no output;
                                       // up count only
   RCAP2 = - ((long) SYSTEMCLOCK/BAUDRATE/16);
   TMR2 = RCAP2;
   TR2= 1;                             // Start Timer2

   SFRPAGE = UART0_PAGE;

   SCON0 = 0x50;                       // 8-bit variable baud rate;
                                       // 9th bit ignored; RX enabled
                                       // clear all flags
   SSTA0 = 0x15;                       
   ES0 = 1;
   IP |= 0x10;
   SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// UART0_Interrupt
//-----------------------------------------------------------------------------
//
// This routine is invoked whenever a character is entered or displayed on the
// Hyperterminal.
//
//-----------------------------------------------------------------------------

void UART0_Interrupt (void) interrupt 4
{
	uint8 i=0;
	uint16 j=0;
	uint8 checksum=0;          //֡����У���      
	static unsigned char UART_Buffer_temp[5];//�ݴ�У��ǰ����
	
   SFRPAGE = UART0_PAGE;

   if (RI0 == 1)
   {
		for(i=0;i<8;i++);    //��ʱ
		if(RI0 == 1)
		{
			for(i=0;i<5;i++)    //0��1Ϊ֡ͷ��2Ϊѡ�ܺţ�3Ϊִ��ָ�4Ϊ��У��У���
			{
				j=0;
				while((RI0==0)&&(j<0x0FFF))//��������źŵ���δ�յ�����֡�����ڴ�
				{j++;}
				RI0=0;
				UART_Buffer_temp[i]=SBUF0;//��ҪУ��
				if((i==2) ||(i==3))
				{checksum+=SBUF0;}
				else;
			}
		
//			if((UART_Buffer_temp[0]==0xEB)&&(UART_Buffer_temp[1]==0x90))//֡ͷ��У�����ȷ��ֵ����ȫ�ֱ���
			if((UART_Buffer_temp[0]==0xEB)&&(UART_Buffer_temp[1]==0x90)&&(checksum==UART_Buffer_temp[4]))//֡ͷ��У�����ȷ��ֵ����ȫ�ֱ���
			{
				for(i=0;i<5;i++)
				{
					UART_Buffer[i]=UART_Buffer_temp[i];
				}
				
				switch(UART_Buffer[3])
				{
					case 0x11:
						Selfcheck_Flag=1;
						break;
					case 0x13:
						Loopcheck_Flag=1;
						StatusCode&=~0xF0;//�Լ���������״̬λ���Ƶ�״̬λ
						break;
					case 0x14:
						Fire_Flag=1;
						break;
					default:
						break;
				}
				StatusCode&=~0x01;
			}else
			{
				Upload_Flag=1;//�յ���������ֱ�ӻش�
				StatusCode|=0x01;
			}//�յ��źŴ���
		}else;
   }
	SCON0 &= ~0x01;//����жϱ�־λ�����ٽ����ж�
}

void OSCILLATOR_Init (void)
{
   int i;                              // Software timer

   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   OSCICN = 0x80;                      // Set internal oscillator to run
                                       // at its slowest frequency

   CLKSEL = 0x00;                      // Select the internal osc. as
                                       // the SYSTEMCLOCK source

   // Initialize external crystal oscillator to use 22.1184 MHz crystal

   OSCXCN = 0x67;                      // Enable external crystal osc.
   for (i=0; i < 256; i++);            // Wait at least 1ms

   while (!(OSCXCN & 0x80));           // Wait for crystal osc to settle

   CLKSEL = 0x01;                      
   // Select external crystal as SYSTEMCLOCK source

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// Pinout:
//
// P0.0   digital   push-pull     UART TX
// P0.1   digital   open-drain    UART RX
// P1.6   digital   push-pull     LED
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
	char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

	SFRPAGE = CONFIG_PAGE;              // Set SFR page

	XBR0     = 0x04;                    // Enable UART0
	XBR1     = 0x00;
	XBR2     = 0x40;                    // Enable crossbar and weak pull-up

	
	P0MDOUT |= 0x01;                    // Set TX pin to push-pull

	P7MDOUT = 0x0F;                    // P7.0-P7.3�����p5.4-p5.7Ĭ��Ϊ����
	P7 = 0xFF;            //��ʼ��������P7

	P5MDOUT  = 0x00;    //P5MDOUT:0-��©�����1-�������	  P5:��©���ģʽ����1�л�Ϊ��������
	P5       = 0xFF;		 //0/1/2/3/4/5/6/7ΪDATA_ZT�����

	
	SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
	

}

void delay_us(uint16 n)   //��ʱKus
{
   while(--n);
}

void delay_ms(uint16 n)
{
   unsigned int i;
   for(;n>0;n--)
   {
     for(i=2211;i>0;i--); 
   }
}
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------   