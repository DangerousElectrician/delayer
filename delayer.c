//delay analog input signal by at least 200 ms and output to resistor ladder DAC
/*
 * 18.5 kHz sample freq
 * 3968 bytes sram total
 * 3700 byte buffer
 * 200 ms delay within 5%
 *
 * quantitization 256 levels
 *
 * 10 bit reading from ADC scaled to 256
 *
*/

#define OFFSET 657 //577 //657
#include <pic18.h>

unsigned char flag = 0;
unsigned char counter =0;
int inputi = 0;
int outputi = 0;

void interrupt IntServe(void) {
    if (TMR0IF) {
        TMR0 = -500;
        flag = 1;
        TMR0IF = 0;
    }
    if (INT0IF) {
        if (++counter >5) counter =0;;
        inputi=0;
        if (counter ==0) {outputi =0; PORTC = 1;}
        if (counter ==1) {outputi =740; PORTC =2;}
        if (counter ==2) {outputi = 1480; PORTC =4;}
        if (counter ==3) {outputi = 2220; PORTC =8;}
        if (counter ==4) {outputi = 2960; PORTC =16;}
        if (counter ==5) {outputi = 3699; PORTC = 32;}
        INT0IF = 0;
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

unsigned char buffer[3939] = {0}; //initialize all to 0

void main(void) {

    // Initialize Timer0
    T0CS = 0;
    T0CON = 0x88; //PS=1
    TMR0ON = 1;
    TMR0IE = 1;
    TMR0IP = 1;
    PEIE = 1;

    // Initialize Int0
    INTEDG0 = 1;
    INT0IE =1;
    TRISB0 =1;
    TRISC = 0x00;

    TMR0 = -500; //54 us period
    GIE = 1;

    TRISD = 0; //port d output

    A2D_Init();
    int raw = 0;
    int scaled = 0;
    while (1) {

        PORTD = buffer[outputi];

        raw = A2D_Read(0);

//        if (raw > (OFFSET+127)) raw = OFFSET+127; //657
//        if (raw < (OFFSET-128)) raw = OFFSET-128;
//        scaled = raw - OFFSET - 128;
        if (raw > (OFFSET+127)) scaled = 255; //657
        else if (raw < (OFFSET-128)) scaled = 0;
        else scaled = raw - OFFSET - 128;

        buffer[inputi] = (unsigned char) scaled;

        if(inputi++ >= 3699) inputi = 0;
        if(outputi++ >= 3699) outputi = 0;

        flag = 0;
        while(flag ==0);
    }
}
