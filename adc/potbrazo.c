#include "potbrazo.h"

// PA0 -> ADC0
// PA1 -> ADC1
// PA2 -> ADC2
// PA3 -> ADC3

// Función de inicialización

void initADC()
{
    // Habilitar laa señales alternativas de los puertos GPIO, ...
    // ... la señal de reloj del puerto A y el modulo ADC1 ...
    // (bit 0; 2 y 9 del registro RCC_APB2ENR)
    RCC->APB2ENR |= (1<<0)|(1<<2)|(1<<9);

    // Configuración de los pines PA0, PA1, PA2 y PA3 como entradas ...
    // (analogicas). Poner en baja los 4 bytes menos significativos ...
    // del registro GPIOA_CRL)

    GPIOA->CRL &= ~(0xFFFF << 0);

    // Se procede a apagar el módulo ADC (poner en baja el bit 0 del ...
    // registro ADC_CR2)
    ADC1->CR2 &= ~(0x1 << 0);

    // Se realiza la configuración de los secuenciadores del módulo ADC ...
    // a usar (poner en baja los 24 bits menos significativos del registro ...
    // ADC_SQR1 y poner en baja los 29 bits menos significativos del registro ...
    // ADC_SQR2)
    ADC1->SQR1 &= ~(0xFFFFFF);
    ADC1->SQR2 &= ~(0x3FFFFFFF);

    // La configuración

    // Del registro ADC_CR1, se colocan en baja los bits 0 al 19 y, 22 y 23
    ADC1->CR1 &= ~((0xFFFFF) | (0xC << 20));

    // Se realiza la configuración de los canales ADC0; 1; 2 y 3 con una velocidad ...
    // de conversión de 239.5 ciclos (registro ADC_SMPR2)
    ADC1->SMPR2 |= (0x7 << 0)|(0x7 << 3)|(0x7 << 6)|(0x7 << 9);

    // Finalmente, se coloca en ALTA el bit 0 del registro ADC_CR2 para encender ...
    // el ADC
    ADC1->CR2 |= (1 << 0);
}

uint16_t leerPuerto(uint8_t channel)
{

    // En el registro ADC_SQR3, se configurará el canal a ser leido, según la ...
    // variable de entrada
    ADC1->SQR3 &= ~(0x3FFFFFFF);
    ADC1->SQR3 |= channel << 0;

    // Del registro ADC_CR2, se coloca en ALTA el bit 0 y 22 para iniciar la ...
    // de datos analógicos a digitales en los canales previamente configurados:
    // PA0, PA1, PA2, PA3
    ADC1->CR2 |= (1 << 0);
    ADC1->CR2 |= (1 << 22);

    // Se monitorea el bit 1 del registro ADC_SR que, al estar en ALTA, indica ...
    // que la conversión en el ADC se completo.
    while(!(ADC1->SR & (1 << 1)));

    // Se realiza la lectura del valor convertido en el canal "channel"
    uint16_t valorADC = ADC1->DR & 0xFFF;

    // Se coloca en BAJA el bit 1 del registro ADC_SR para indicar que la converión ...
    // fue exitosa
    ADC1->SR &= ~(1 << 1);

    // Finalmente, se devuelve el valor leido.
    return valorADC;
}