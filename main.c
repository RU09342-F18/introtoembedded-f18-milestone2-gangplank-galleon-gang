#include <msp430.h> 
#include <msp430f5529.h>
/* Coded by Ian Moffitt and Joe McNatt
 * 
 * Board used: MSP430F5529
*/

//Some global variables
float currentTemp;
float vIn;
int adcIn;
int target;

//Some define statements to be able to read the code a little easier
#define Recieve BIT5  //Port 4.5
#define Transmit BIT4 //Port 4.4
#define FanOut BIT0     //Port 2.0
#define FanControl TA1CCR1
#define currentByte UCA1RXBUF
#define transferByte UCA1TXBUF


void ADCSetup(){
    REFCTL0 &= ~REFMSTR; //reset REFMASTER to hand over control of internal reference voltage to ADC_12
    //initialize control register ADC12CTL0
    ADC12CTL0 = ADC12SHT0_4 + ADC12REFON + ADC12REF2_5V + ADC12ON;
    //initialize control register ADC12CTL1
    ADC12CTL1 = ADC12SHP + ADC12SSEL_3;
    //set control register of conversion memory
    ADC12MCTL0 = ADC12SREF_1 + ADC12INCH0;
    //configure the port to be used as an ADC input
    P6SEL |= 0x01;
    //enable interrupt
    P6DIR &= ~BIT0;
    ADC12IE = 0x01;
    //enable conversion
    ADC12CTL0 |= ADC12ENC;
    //start sampling
}

void UARTSetup(){//9600 baud
    UCA1CTL0 = UCMODE_0;
    UCA1CTL1 = UCSWRST;
    UCA1CTL1 |= UCSSEL_2;
    UCA1BR0 = 104;
    UCA1BR1 = 0;
    UCA1MCTL = UCBRS_1;
    UCA1CTL1 &= ~UCSWRST;
    UCA1IE |= UCRXIE;
}

void TimerSetup(){//sets up a timer for PWM and a timer for sampling
        TA1CTL = TASSEL_2 + MC_1 + TACLR;
        TA1CCTL1 = OUTMOD_3; //places timer in Reset/Set mode for easy PWM
        TA1CCR0 = 100;
        FanControl = 100;
        TA2CTL = TASSEL_2 + MC_1 + TACLR + TAIE; //Interrupt Enabled
        TA2CCR0 = 100;
}

void changeFan(float current, int target){
    if(current >= target){
        if (current - target > 1){
            FanControl = 0; //above 1 degree puts the fan on full blast
        }
        else if(current - target <= 1){
            FanControl = 50;  //within 1 degree puts fan down to 50%
        }
    }
    else if(current < target){
        if(target - current > 1){
            FanControl = 100; //below 1 degree turns fan completely off
        }
        else if(target - current <= 1){
            FanControl = 50; //within 1 degree puts fan down to 50%
        }
    }
}

float convertTemp(int adcIn){ //temp = .01(temp) + .5 = vIn
    vIn = adcIn * .00061;
    return ((vIn - .5)/.01);
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    P4SEL |= Recieve + Transmit;    //set up Port 4 pins as I/O for USB UART
    P2SEL |= FanOut;    //set up Port 2 PWM pin as I/O
    P4DIR = Transmit;   //set TX pin as an output pin
    P2DIR = FanOut;     //set fan PWM pin to output
    P2OUT = 0;

    TimerSetup();
    UARTSetup();
    ADCSetup();
    __bis_SR_register(GIE); //interrupt enabled
    while(1)
    {

    }

}

#pragma vector = USCI_A1_VECTOR
__interrupt void USCI1RX_ISR(void)
{
    target = currentByte;
    transferByte = target;
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC_ISR(void)
{
    adcIn = ADC12MEM0;
    currentTemp = convertTemp(adcIn);
    changeFan(currentTemp, target);//switches fan speed based on last and current value
    char out = (char)currentTemp;
    transferByte = out;
}

#pragma vector=TIMER2_A0_VECTOR
__interrupt void TIMER_A0_ISR()
{
    ADC12CTL0 |= ADC12SC; //start sampling and conversion
}

#pragma vector=TIMER2_A1_VECTOR
__interrupt void TIMER_A1_ISR()
{
    ADC12CTL0 |= ADC12SC; //start sampling and conversion
}
