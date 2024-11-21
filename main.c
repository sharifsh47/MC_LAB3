#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include <stdio.h>


volatile uint32_t AVTEMP;

int main(void)
{

    configADC0();// ADC0 – Configuration (to be implemented as a function)
    configTimer1A(); // Timer – Configuration (to be implemented as a function)
    ADC0_InSeq0();

   while(1)
      {
       int i;
       unsigned int VALUE = 0;
              for ( i = 0; i < 4; i++) {
                VALUE += ADC0_InSeq0(); //  sum of 4 loops
                    }
                  AVTEMP = VALUE / 4; //  Average
               // Convert the digital ADC output to temperature
               double temperature = 147.5 -((75 * 3.3 * AVTEMP) / 4096.0);
               printf("temperature is %.4f°C\n", temperature);
               // Delay for 1 second using Timer
               while (!(TIMER1_RIS_R & 0x1)) {} // Wait for Timer1A timeout
               TIMER1_ICR_R = 0x1; // Clear Timer1A timeout flag
      }
}

   void configTimer1A() {
       // Enables timer1A
       SYSCTL_RCGCTIMER_R |= (1 << 1); //  TIMER 1
       while (!(SYSCTL_PRTIMER_R & (1 << 1))); // Wait for timer 1 activation

       TIMER1_CTL_R &= ~0x0001; // Disable Timer 1A
       TIMER1_CFG_R = 0x4; // 16-bit mode
       TIMER1_TAMR_R = 0x12; // one shot timer mode and match enable
       TIMER1_TAPR_R = 245 - 1; // Prescaler ceil(16MHz*1s/2^16)-1
       TIMER1_TAILR_R = 65307 - 1; // ILR interval load ceil(16MHz*1s/245)-1
       //TIMER1_TAMATCHR_R = 32653 - 1; // match value ceil(16MHz*0.5s/245)-1
       TIMER1_CTL_R |= 0x0001; // Enable Timer 1A
   }

   void configADC0() {
       SYSCTL_RCGCADC_R |= (1<<0); // ADC0 digital block,activate clock
       while(!(SYSCTL_PRADC_R & 0x01));//loops until ready

       // Chose clock for ADC module (“magic code”) provides stable and accurate clock
           SYSCTL_PLLFREQ0_R |= (1<<23); //Phase locked loop
           while (!(SYSCTL_PLLSTAT_R & 0x01));
           ADC0_CC_R |= 0x01; //waitcycle++;
           SYSCTL_PLLFREQ0_R &= ~(1<<23);

       ADC0_SSCTL0_R |= 0x0000000A; // 0x0000000A is 00000001010.END is set to 1,which ends the sample sequence.TS is set to 1,in order to sample the internal temperature sensor.


       ADC0_ACTSS_R |= 0x01; //  Activate sample sequencer 0 to deliver samples to FIFO

   }

   //------------ADC0_InSeq0------------
   // Analog to digital conversion
   // Output: 12-bit result of ADC conversion
   unsigned long ADC0_InSeq0(void){
   unsigned long data;
   ADC0_PSSI_R |= 0x01; // Start ADC0,initiates an AD conversion
   // wait for FIFO "NON-EMPTY"
   while(ADC0_SSFSTAT0_R & 0x000000100);
   data =(unsigned long) ADC0_SSFIFO0_R & 0xFFF;//0xFFF masks the all bits except for the 12 lower bits.contents of FIFO can be read
   //by repeatedly reading register ADC0_SSFIFO0_R
   //each time FIFO tail got read , it gets  updated with a new value
   return data;
   }
