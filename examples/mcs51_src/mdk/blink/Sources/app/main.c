#include <REG51.H>

sbit led = P0^0;

void main(void)
{
    while(1)
    {
			led = 0;
			led = 1;
    }
}
