/******************************************************************************
 * 40m band frequency counter with audio CW output
 * 
 * 2017-4-16
 * Gerd Bartelt
 * www.sebulli.com
 * 
 * See project_information.txt for information about the hardware
 *                           
 ******************************************************************************/
#include <xc.h> 

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

        
#include <stdint.h>        


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
//#define DEBUG_FRQ

#define CW_OUT      PORTAbits.RA0
#define SHIFT_IN    PORTAbits.RA3
#define FRQREFIN    PORTAbits.RA2

typedef union {
    uint8_t byte;
    struct {
        unsigned b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
    };
} t_byte;

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void pause_sound() {
    for (uint8_t del2 = 128; del2; del2--) {NOP();} 
}
#ifdef DEBUG_FRQ
void debug_frq(uint16_t val) {
    CW_OUT = 1;
    pause_sound();
    CW_OUT = 0;
    pause_sound();
    
    for (uint8_t i=16; i; i--) {
        if (val & 0x8000)
            CW_OUT = 1;
        else
            CW_OUT = 0;
        pause_sound();
    }
    CW_OUT = 0;
}
#endif

#ifndef DEBUG_FRQ
void pause () {
    for (uint8_t del1 = 255; del1; del1--) {
        pause_sound();
    }    
}

void pause_long() {
    pause();
    pause();
}
void beep_short () {
    for (uint8_t del1 = 100; del1; del1--) {
        pause_sound();
        CW_OUT = 0;
        pause_sound();
        CW_OUT = 1;
    }
}
void beep_long () {
    beep_short();
    beep_short();
    beep_short();
}


void morse(uint8_t val) {
    for (int8_t ii=254; ii!=3; ii++) {
        if ((val+ii) > 250)
            beep_short();
        else
            beep_long();
        pause();
    }
    pause_long();
}
#endif

void main(void)
{
uint16_t freq;
uint16_t freq_h;
uint8_t bcd[5];
uint8_t delaycnt_8;
union u_period{
    struct {
        t_byte LSB, MSB;
    };
  uint16_t w;   // access period by word
} period;
uint8_t period_LSB;
uint8_t tmr0_offset;

    // RA0 is output of CW tone
    TRISA = 0xFE;
    // digital IO
    ANSELA = 0x00;
    // Weak Pull Up
    WPUA = 0x0F;
   
    // TMR0 CLKIN without prescaler 
    // Weak Pull Up enabled
    OPTION_REG = 0x7F;
    
    pause_sound();
    
    // Now we count the period of the RF signal
    // To achieve later on a frequency resolution of 100Hz at 7200kHz,
    // the resolution of the period must also be at least 72000 steps.
    // With a 1MHz reference clock we must wait 72000 x 1us = 72ms.
    // This are 129600 instruction cycles
    
    // Between 7000kHz and 7200kHz are 2000 steps with a 8 bit counter.
    // This counter overruns 7,8125 times. The coarse counter for MSB
    // should count exact 1 step during a period of the LSB (TMR0).
    // So the gate time must be 
    // 72ms/256 x 4 = 1125us = 2025 instruction cycles at 7200kHz fext
    
    // Read start value of TMR0
    tmr0_offset = TMR0;
    for (delaycnt_8=251; delaycnt_8; delaycnt_8--) {
        NOP();
    }
    NOP();NOP();NOP();NOP();NOP();
    // Read TMR0 2025 instructions after TMR0 was first read
    // (Measured with PIC simulator)
    period.MSB.byte = TMR0;

    for (delaycnt_8=122; delaycnt_8; delaycnt_8--) {
        pause_sound();
    }
    for (delaycnt_8=69; delaycnt_8; delaycnt_8--) {
        NOP();
    }
    NOP();
    // Read TMR0 129600 instructions after TMR0 was first read
    // (Measured with PIC simulator)
    period.LSB.byte = TMR0;

    // Subtract the start value of TMR0
    period.LSB.byte -= tmr0_offset;
    period.MSB.byte -= tmr0_offset;
    
    // TEST with 72000
    //period.MSB.byte = 0x65;
    //period.LSB.byte = 0x40;
    
    //==FLOOR((B...)/4)
    period.MSB.byte >>=2;
    
    // Store the last significant bit of period
    period_LSB = period.LSB.b0;
    
    // Divide it by 2 so it fits into an integer.
    // Don't worry, we've saves the last bit.
    period.w >>= 1;
    period.MSB.b7 = 1;
   
   
    // Calculate the frequency
    // Numerator: 129600 instructions at 8MHz flck => 129600 * 80000 / 2 / 2
    // this is also 72000*72000 / 2
    // We subtract 10000 so that 72000 fits in a unsigned int = 62000
  
    // The minimum frequency is 6328.2kHz (period = 9FFF)
    // The maximum frequency is limited by the maximum fclock = 16MHz
    // and the 5 digits. So it will be 9999kHz.
    // But out of the range of 40m band the accuracy will be reduced due to
    // some optimizations of this code
    freq = 2592000000UL / period.w - 10000 - period_LSB;
    
    // CW Shift -700Hz (lower sideband)
    if (SHIFT_IN == 0)
        freq -= 7;

#ifdef DEBUG_FRQ
    debug_frq(freq);
#endif       
    
    // convert integer to 5 digits BCD    
    for (uint8_t i=4; i != 0xFF; i--) {
        freq_h = freq;
        freq = freq/10;
        bcd[i] = freq_h-(freq*10);
    }
 
 
    // Add 1 to the first digit, because we have subtracted 10000
    
#ifndef DEBUG_FRQ
    morse(bcd[0]+1);
    pause_long();
    pause_long();
    
    morse(bcd[1]);
    morse(bcd[2]);
    morse(bcd[3]);
    pause_long();
    pause_long();
    
    morse(bcd[4]);
#endif        
    // Stop
    while(1);

}

