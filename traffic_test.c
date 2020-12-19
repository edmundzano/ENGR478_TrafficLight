
#include <stdint.h>
#include <stdbool.h>
#include "C:/ti/TivaWare_C_Series-2.1.4.178/inc/hw_types.h"
#include "C:/ti/TivaWare_C_Series-2.1.4.178/inc/hw_memmap.h"
#include "C:/ti/TivaWare_C_Series-2.1.4.178/inc/hw_gpio.h"
#include "C:/ti/TivaWare_C_Series-2.1.4.178/driverlib/sysctl.h"
#include "C:/ti/TivaWare_C_Series-2.1.4.178/driverlib/pin_map.h"
#include "C:/ti/TivaWare_C_Series-2.1.4.178/driverlib/gpio.h"
#include "C:/ti/TivaWare_C_Series-2.1.4.178/driverlib/timer.h"
#include "C:/ti/TivaWare_C_Series-2.1.4.178/driverlib/interrupt.h"
#include "C:/ti/TivaWare_C_Series-2.1.4.178/inc/tm4c123gh6pm.h"


volatile unsigned long count = 0;
int timer = 0;
int clock;

#define 	WestGo					0x01
#define		WestSlow				0x02
#define 	WestStop 				0x04
#define 	SouthGo					0x08
#define		SouthSlow				0x10
#define 	SouthStop 			0x20
#define		WalkWest				0x04
#define		DontWalkWest		0x08
#define 	OFF							0x00

//Globally enable interrupts

void Port_Init(void) {
	volatile unsigned long delay;
		SYSCTL_RCGC2_R=0x13;        //activate clock for port A,E,B
		delay=SYSCTL_RCGC2_R;       // no need to unlock port B,E,A
		GPIO_PORTB_AMSEL_R=0x00;          //disable analog on port B
		GPIO_PORTA_AMSEL_R=0x00;          //disable analog on port A
		GPIO_PORTB_PCTL_R=0x00000000;     //enable regular GPIO
		GPIO_PORTA_PCTL_R=0x00000000;     //enable regular GPIO
		GPIO_PORTB_DIR_R=0x3F;            //outputs on PB0-5
		GPIO_PORTA_DIR_R=0x0C;            //outputs on PA2 and PA3
		GPIO_PORTB_AFSEL_R=0x00;          //disable alternate function
		GPIO_PORTA_AFSEL_R=0x00;          //disable alternate function
		GPIO_PORTB_DEN_R=0x3F;            //enable digital I/O on PB0-5
		GPIO_PORTA_DEN_R=0x0C;            //enable digital I/O on PA2 and PA3
		
}

//Globally enable interrupts 
void IntGlobalEnable(void)
{
    __asm("    cpsie   i\n");
}


void Timer0A_Init(unsigned long period)
{   
	
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); 		// configure for 32-bit timer mode
  TimerLoadSet(TIMER0_BASE, TIMER_A, period -1);      //reload value
	IntPrioritySet(INT_TIMER0A, 0x00);  	 // configure Timer0A interrupt priority as 0
  IntEnable(INT_TIMER0A);    				// enable interrupt 19 in NVIC (Timer0A)
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);      // arm timeout interrupt
  TimerEnable(TIMER0_BASE, TIMER_A);      // enable timer0A
}


//interrupt handler for Timer0A

void Timer0A_Handler (void)
{
	
	// acknowledge flag for Timer0A timeout
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	//Increments counter by 1
	count ++;
	if(count == 4) {
		count = 0;
	}
}  


/** INTERRUPT CONFIGURATION***/

	
	int main(void)
{
    //initialize the GPIO ports    
    Port_Init();
        
    //configure the GPIOF interrupt
    //Interrupt_Init();
	
   unsigned long period = 48000000;	//three second delay
   SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
   clock = SysCtlClockGet();
	
    //initialize Timer0A and configure the interrupt
    Timer0A_Init(period);
    // Loop forever.
    //
    while(1)
    {
				//GPIOPinWrite(GPIO_PORTF_BASE, 0x0E, count << 1);
			
        switch (count) {
						
            case 0:
								GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, WestGo);
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4, OFF);
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, SouthStop); 
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, WalkWest);
								GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, OFF);
								
								timer = 0;
								while(timer < 8) {
										SysCtlDelay(2000000 * 5);
										GPIO_PORTA_DATA_R ^= 0x08;
										timer++;
								}

                break;
            case 1:
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4, OFF);
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, WestSlow);
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, SouthStop);
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, OFF);
								GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, DontWalkWest);
                break;
            case 2:
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5, OFF);
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, WestStop);
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, SouthGo);
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, OFF);
								GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, DontWalkWest);
								
								timer = 0;
								while(timer < 8) {
										SysCtlDelay(2000000 * 5);
										timer++;
								}
                break;
            case 3:
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_5, OFF);
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, WestStop);
								GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, SouthSlow);
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, OFF); 
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, OFF);
								GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, DontWalkWest);
                break;

        }
	}
	
	
}

