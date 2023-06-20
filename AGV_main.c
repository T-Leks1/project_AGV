/*
 */
//branch
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


/*functionML (speedML, power):
{
    timer X top == |speedML|
    PWM_ML = power
}
functionMR (speedMR, power):
{
    timer Y top == |speedMR|
    PWM_MR = power
}

timer X overflow interrupt:
{
    if speedML != 0:
    {
        if iML == 1 step 1
        if iML == 2 step 2
        if iML == 3 step 3
        if iML == 4 step 4
        if speedML > 0 i++
        if speedML < 0 i--
        if iML <= 0 iML = 4
        if iML >= 5 iML = 1
    }
}
timer Y overflow interrupt:
{
    if speedMR != 0:
    {
        if iMR == 1 step 1
        if iMR == 2 step 2
        if iMR == 3 step 3
        if iMR == 4 step 4
        if speedMR > 0 i++
        if speedMR < 0 i--
        if iMR <= 0 iMR = 4
        if iMR >= 5 iMR = 1
    }
}*/
#define stepLa (1<<PA0)
#define stepLA (1<<PA1)
#define stepLb (1<<PA2)
#define stepLB (1<<PA3)
#define stepRa (1<<PA4)
#define stepRA (1<<PA5)
#define stepRb (1<<PA6)
#define stepRB (1<<PA7)
void init(void)
{
	PWM_init();
    Motor_init();
}

int abs(int waarde)
{
    if (waarde < 0)
    {
        waarde *= -1;
    }
    return waarde;
}

void PWM_init(void)
{

    DDRB |= (1<<PB5); // D11 is PB5, maak output
    PORTB |= (1<<PB5); // Zet initieel uit (active low)
    DDRB |= (1<<PB6); // D12 is PB6, maak output
    PORTB |= (1<<PB6); // Zet initieel uit (active low)
    TCCR1A = (1<<COM1A1) | (1<<COM1A0) | (1<<COM1B1) | (1<<COM1B0) | (1<<WGM11) | (0<<WGM10); //COM1xn is de PWM instelling voor de pins
    TCCR1B = (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10); // CS1n is de prescaler 8 WGM1n is de PWM instelling voor timer 1 (fastPWM)
    OCR1A = 630;
    OCR1B = 630;
    ICR1 = 1260;
}

void MotorL_timer_init(void)
{
    TCCR4A = 0;//timer motor
    // CSNx = prescaler = 1024 (OVF 23.8ms) WGMNx = timermode CTC(Clear Timer on Compare)
    TCCR4B = (1<<CS42) | (0<<CS41) | (1<<CS40) | (1<<WGM42) | (1<<WGM43);
    TIMSK4 = (1<<TOIE4);
    ICR4 = 65536;
}

void MotorR_timer_init(void)
{
    TCCR5A = 0;//timer motor
    // CSNx = prescaler = 1024 (OVF 23.8ms) WGMNx = timermode CTC(Clear Timer on Compare)
    TCCR5B = (1<<CS52) | (0<<CS51) | (1<<CS50) | (1<<WGM52) | (1<<WGM53);
    TIMSK5 = (1<<TOIE5);
    ICR5 = 65536;
}

void Motor_init(void)
{
    DDRA |= (1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4) | (1<<PA5) | (1<<PA6) | (1<<PA7);
    PWM_init();
    MotorL_timer_init();
    MotorR_timer_init();

}

volatile int TotTicksL = 0;
volatile int TotTicksR = 0;

void MotorL(int speed,int rotation)
{
    if ((speed > 10)&&(speed <=100))
    {
        ICR4 = (65536*speed/100);
    }
    TotTicksL = rotation;
}

ISR(TIMER4_OVF_vect)
{
    if (TotTicksL != 0)
    {
        if ((abs(TotTicksL % 4)) == 0)
        {
            PORTA |= stepLa | stepLb;
            PORTA &= ~stepLA & ~stepLB;
        }
        else if ((abs(TotTicksL % 4)) == 1)
        {
            PORTA |= stepLA | stepLb;
            PORTA &= ~stepLa & ~stepLB;
        }
        else if ((abs(TotTicksL % 4)) == 2)
        {
            PORTA |= stepLA | stepLB;
            PORTA &= ~stepLa & ~stepLb;
        }
        else if ((abs(TotTicksL % 4)) == 3)
        {
            PORTA |= stepLa | stepLB;
            PORTA &= ~stepLA & ~stepLb;
        }
        if (TotTicksL < 0) TotTicksL++;
        if (TotTicksL > 0) TotTicksL--;
    }
}

void MotorR(int speed,int rotation)
{
    if ((speed > 10)&&(speed <=100))
    {
        ICR5 = (65536*speed/100);
    }
    TotTicksR = rotation;
}

ISR(TIMER5_OVF_vect)
{
    if (TotTicksR != 0)
    {
        if ((abs(TotTicksR % 4)) == 0)
        {
            PORTA |= stepRa | stepRb;
            PORTA &= ~stepRA & ~stepRB;
        }
        else if ((abs(TotTicksR % 4)) == 1)
        {
            PORTA |= stepRA | stepRb;
            PORTA &= ~stepRa & ~stepRB;
        }
        else if ((abs(TotTicksR % 4)) == 2)
        {
            PORTA |= stepRA | stepRB;
            PORTA &= ~stepRa & ~stepRb;
        }
        else if ((abs(TotTicksR % 4)) == 3)
        {
            PORTA |= stepRa | stepRB;
            PORTA &= ~stepRA & ~stepRb;
        }
        if (TotTicksR < 0) TotTicksR++;
        if (TotTicksR > 0) TotTicksR--;
    }
}
int MotorLR(int speed,int rotation)
{
    MotorR(speed,rotation);
    MotorL(speed,rotation);
}

int stappen_aantal_l (int x)
{
    MotorL(50,x);

}
int stappen_aantal_r (int y)
{
    MotorR(50,y);
}


int graden_draaien (int l,int r)
{

    int x;
    int y;
    x = l/1.8;
    y = r/1.8;
    stappen_aantal_l(x);
    stappen_aantal_r(y);
}
int calibreren (void)
{
    graden_draaien(45,-45);
    graden_draaien(-45,45);
}
int vooruit_rijden(void)
{
   MotorLR(50,1);
}


int main(void)
{
    init();



    while(1)
     {



     }

    return 0;
}
