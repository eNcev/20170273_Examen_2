#include "servobrazo.h"

// PA6 -> T3C1
// PA7 -> T3C2
// PB0 -> T3C3
// PB1 -> T3C4

void TIM3_Init()
{
    // Habilitar la señal de reloj del puerto GPIO A y B, asi como ...
    // las señales alternativas
	RCC->APB2ENR |= (1<<3)|(1<<2)|(1<<0);

    // Habilitar el temporizador 3 (bit 1 del registro RCC_APB1ENR)
	RCC->APB1ENR |= (1<<1);

    // Configuracion de los pines PA6, PA7, PB1 y PB2 como salidas ...
    // alternativas con una velocidad maxima de 2MHz

    GPIOA->CRL &= ~((0xF << 24)|(0xF << 28));
	GPIOA->CRL |= (0xA << 24)|(0xA << 28);

	GPIOB->CRL &= ~((0xF << 0)|(0xF << 4));
	GPIOB->CRL |= (0xA << 0)|(0xA << 4);
	
    //Se configura el preescalador para obtener una frecuencia de entrada ...
    // de 1 MHz hacia el temporizador 3
    // F = 8 000 000 / [ (PSC + 1) * (ARR + 1)]
    // Considerando: F = 50 Hz y ARR = 10000 => PSC = 15

    TIM3->PSC = 15;
	TIM3->ARR = 10000;

    // Configuracion de los canales a usar del temporizador 3: 
    // canal 1; 2; 3 y 4 (duty cycle = 0 %).
	TIM3->CCR1 = 0;
    TIM3->CCR2 = 0;
    TIM3->CCR3 = 0;
    TIM3->CCR4 = 0;
	
    // Configurar los canales 1 (PA6) y 2 (PA7) como salidas PWM (registro TIM3_CCMR1) ...
    // y los canales 3 (PB0) y 4 (PB1) (registro TIM3_CCMR2)
    TIM3->CCMR1 &= ~(0xFF << 0);
    TIM3->CCMR1 &= ~(0xFF << 8);
    TIM3->CCMR2 &= ~(0xFF << 0);
    TIM3->CCMR2 &= ~(0xFF << 8);

	TIM3->CCMR1 |= (0x68 << 0);
    TIM3->CCMR1 |= (0x68 << 8);
    TIM3->CCMR2 |= (0x68 << 0);
    TIM3->CCMR2 |= (0x68 << 8);
	
    // Habilitar el modo de comparacion de los 4 canales (poner en ALTA: el bit 0, ...
    // bit 4, bit 8 y bit 12 del registro TIM3_CCER)
    TIM3->CCER |= (1 << 12)|(1 << 8)|(1 << 4)|(1 << 0);

    // Habilitar el temporizador 3 y para permitir el cambio de valores del contador ...
    // (poner en ALTA los bits 0 y 7 del registro TIM3_CR1)
	TIM3->CR1 |= (1<<7)|(1<<0);

}

// Se define la funcion para transformar el valor leido de un ADC (res: 4096), al duty cycle ...
// de su salida PWM respectiva. Se debe resaltar, que se pone como un "angulo" inicial ...
// un valor de 1000 (para evitar que llegue a 0) y un "angulo" final con el valor ...
// maximo a contar (10000)

void cambiarAngulo(uint8_t canal, uint32_t valorADC)
{
    uint8_t k;
    uint32_t angulo;
    uint32_t angulo_inicial = 1000;
    uint32_t angulo_final = 10000;

    angulo = angulo_inicial * ((valorADC*(angulo_final-angulo_inicial))/4096);

    switch (canal)
    {
        case 1:
            TIM3->CCR1 = angulo;
            break;
        case 2:
            TIM3->CCR2 = angulo;
            break;
        case 3:
            TIM3->CCR3 = angulo;
            break;
        case 4:
            TIM3->CCR4 = angulo;
            break;
        default:
            k = 0;
            break;
    }
}