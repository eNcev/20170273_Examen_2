#include "uart1.h"

void inicializarUart()
{
    RCC->APB2ENR |= (1<<0)|(1<<2)|(1<<14);
    GPIOA->CRH &= ~(0xFF <<  4);
    GPIOA->CRH |= (1<<5)|(1<<7)|(1<<10);

    USART1->BRR &= ~(0xFFFF);
    USART1->BRR |= (0x34 << 4);
    USART1->BRR |= (0x1 << 0);
    USART1->CR1 |= (1<<2)|(1<<3)|(1<<13);
}

void enviarCaracter(uint8_t car)
{
    while(!(USART1->SR & (1<<7)));
    USART1->DR = car;
    while(!(USART1->SR & (1<<6)));
}

void nuevaLinea()
{
    enviarCaracter('\n');
    enviarCaracter('\r');
}

void enviarPalabra(uint8_t palabra[])
{
    uint32_t i = 0;
    for(i = 0; palabra[i] != '\0' ;i++)
    {
        enviarCaracter(palabra[i]);
    }
    nuevaLinea();
}

uint8_t leerCaracter(void)
{
    while(!(USART1->SR & (1<<5)));
    return USART1->DR & (0xFF);
}