#define MYBRR F_CPU/16/BAUD-1
#define ADDRESS 0b10101010
#include <stdlib.h>
uint8_t _k = 0;
uint8_t _buffer[4];


void initalizeUART(void)
{
    uint16_t ubrr = MYBRR;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B |= 1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0;
    UCSR0C |= 3 << UCSZ00;
}

uint8_t receiveUART(void)
{
    while(!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void transmitUART(uint8_t data)
{
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

/*uint8_t receive_data_isr(void)
{
    volatile char c = UDR0;
    _buffer[_k] = c;
    for(int j = 0; j < 3; j++)
    {
        if(_buffer[(2+j)%3] == (uint8_t)(_buffer[(1+j)%3] + _buffer[(0+j)%3]) && (_buffer[(0+j)%3] == ADDRESS)) 
            return _buffer[(1+j)%3];
        else
            return 0;
    }
    _k = (_k + 1)%3;
}*/

void transmit_data(uint8_t data)
{
    for(int i = 0; i < 10; i++)
    { 
        transmitUART(ADDRESS);
        transmitUART(35);
        transmitUART(data);
        transmitUART((uint8_t)(ADDRESS + data + 35));
    }
}


void transmit_string(const char* string)
{
    uint8_t stringpos = 0;
    do
    {
        for(int i = 0; i < 4; i++)
        {
            transmitUART(ADDRESS);
            transmitUART(stringpos);
            transmitUART(*string);
            transmitUART((uint8_t)(ADDRESS + *string + stringpos));
        }
        stringpos++;
    }
    while(*string++ != '\0');
}

void transmit_sentence(const char* string)
{
    while(*string != '\0')
        transmitUART(*string++);
    transmitUART('\n');
}

void transmit_number(int number)
{
    char string[10] = {0};
    itoa(number, string, 10);
    transmit_sentence(string);
}
