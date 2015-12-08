//delay analog input signal by at least 200 ms and output to resistor ladder DAC
/*
 * 12.33 kHz sample freq
 * 3968 bytes sram total
 * 3700 byte buffer
 * 300 ms delay
 * 
 * quantitization 256 levels
 * 
 * 10 bit reading from ADC scaled to 256
 * 
*/

#include <pic18.h>

unsigned char flag = 0;

void interrupt IntServe(void) {
    if (TMR0IF) {
        TMR0 = -39062;
        flag = 1;
        TMR0IF = 0;
    }
}

unsigned int A2D_Read(unsigned char c) {
    unsigned char i;
    c = c & 0x0F;
    ADCON0 = (c << 2) + 0x01; // set Channel Select
    for (i = 0; i < 3; i++); // wait 2.4us (approx)
    GODONE = 1; // start the A/D conversion
    while (GODONE); // wait until complete (approx 8us)
    return (ADRES);
}

void A2D_Init(void) {
    TRISA = 0xFF;
    TRISE = 0x0F;
    ADCON2 = 0x85;
    ADCON1 = 0x07;
    ADCON0 = 0x01;
}

unsigned char buffer[3700] = {0}; //initialize all to 0

void main(void) {

    // Initialize Timer1
    T0CS = 0;
    T0CON = 0x87; //PS=256
    TMR0ON = 1;
    TMR0IE = 1;
    TMR0IP = 1;
    PEIE = 1;

    TMR0 = -11719; //300 millisecond period
    GIE = 1;

    A2D_Init();
    unsigned long raw = 0;
    while (1) {
        raw = A2D_Read(0);

    }
}
