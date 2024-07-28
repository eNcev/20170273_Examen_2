#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart/uartbrazo.h"
#include "adc/potbrazo.h"


int main()
{
    uint16_t valorLectura = 0;
    uint8_t i = 2;
    uint32_t angulo_inicial = 600;
	uint32_t angulo_final = 2400;
    uint32_t anguloADC = 0;
    TIM4_Init();
    initADC();
    inicializarUart();
    enviarPalabra("BIENVENIDOS AL PROGRAMA DEL BRAZO ROBOTICO");
    while(1)
    {
        while(i < 7)
        {
            valorLectura = leerADC(i);
            anguloADC = angulo_inicial + ((valorLectura*(angulo_final - angulo_inicial))/4096);
            modificarDuty(i,anguloADC);

            if(i == 3)
            {
                i = i + 2;
            }
            else
            {
                i = i + 1;
            }
        }
        i = 2;
    }
    return 0;
}