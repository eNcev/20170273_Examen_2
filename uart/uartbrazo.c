#include "uartbrazo.h"

// Libreria para inicializar y usar el UART 3
// PB10 -> TX3
// PB11 -> RX3

// Funcion de inicializacion

void inicializarUart()
{
    // Habilitar laa señales alternativas de los puertos GPIO y ...
    // ... la señal de reloj del puerto B (bit 0 y 3 del registro ...
    // RCC_APB2ENR)
    RCC->APB2ENR |= (1<<0)|(1<<3);

    // Habilitar el modulo USART 3 (poner en alta el bit 18 del ...
    // registro RCC_APB1ENR)
    RCC->APB1ENR |= (1<<18);

    // Configuracion de los pines PB10 y PB11 como salida (alternativa) y ...
    // entrada (flotante) respectivamente (poner en alta los bits 9; 11 y ...
    // 14 del registro GPIOB_CRH)

    GPIOB->CRH &= ~(0xFF <<  8);
    GPIOB->CRH |= (1<<9)|(1<<11)|(1<<14);    

    // Configuracion de la velocidad de transmision a usar (9600 Bd)
    // (parte entera y parte "decimal" en el registro USART_BRR)

    USART1->BRR &= ~(0xFFFF);
    USART1->BRR |= (0x34 << 4);
    USART1->BRR |= (0x1 << 0);

    // Habilitacion de pines de Transmision, Recepcion y del modulo USART
    // (poner en alta los bits 2 [Recep.]; 3 [Trans.] y 13 [habilitar USART])

    USART1->CR1 |= (1<<2)|(1<<3)|(1<<13);
}

// Funcion para transmitir un solo caracter

void enviarCaracter(uint8_t car)
{
    // Primero se espera que el pin de Transmision este listo para enviar un dato, ...
    // monitoreando el estado del bit 7 del registro USART_SR
    while(!(USART1->SR & (1<<7)));
    // Una vez se esta listo, se envia el caracter deseado mediante el registro ...
    // ...USART_DR
    USART1->DR = car;
    // Finalmente, se espera que la transmision se halla completado monitoreando ..
    // el estado del bit 6 del registro USART_SR
    while(!(USART1->SR & (1<<6)));
}

// Se crea una funcion para poder enviar un salto de linea mediante el uso de la funcion ...
// enviarCaracter() establecida previamente
void nuevaLinea()
{
    enviarCaracter('\n');
    enviarCaracter('\r');
}

// Funcion para enviar una palabra o cadena de caracteres

void enviarPalabra(uint8_t palabra[])
{
    // Se hace uso de un ciclo for para transmitir caracter por caracter de la cadena ...
    // haciendo uso de la funcion enviarCaracter()
    uint32_t i = 0;
    for(i = 0; palabra[i] != '\0' ;i++)
    {
        enviarCaracter(palabra[i]);
    }
    nuevaLinea();
}

// Funcion para poder leer un caracter recibido en el pin PB11 (RX3)
uint8_t leerCaracter(void)
{
    // Se monitorea el bit 5 del registro USART_SR para poder determinar ...
    // si se recepciono un nuevo dato
    while(!(USART1->SR & (1<<5)));
    // Si hay un nuevo dato, se procede a retornar la informacion de los ...
    // ultimos 8 bits del registro USART_DR, donde se tiene la nueva data recepcionada
    return USART1->DR & (0xFF);
}
