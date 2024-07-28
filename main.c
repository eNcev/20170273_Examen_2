#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart/uartbrazo.h"
#include "adc/potbrazo.h"
#include "pwm/servobrazo.h"

int main()
{
    uint8_t valorLectura;
    uint8_t i = 0;
    uint32_t valorADC = 0;

    uint8_t ultimoValorLectura;

    // Inicialización del temporizador 3, del ADC 1 y del UART3
    TIM3_Init();
    initADC();
    inicializarUart();

    enviarPalabra("BIENVENIDOS AL PROGRAMA DEL BRAZO ROBOTICO");

    while(1){
        
        valorLectura = leerCaracter();
        i = 0;

        // Se usa un Do-While para no interrumpir el código que se este ...
        // ejecutando, en caso se detecte un nuevo caracter. Para ello, ...
        // se monitorea el estado del bit 5 del registro USART_SR
        do
        {
            if (valorLectura == "a")
            {
                // En caso se seleccione la opción "a", solo por una vez ...
                // se enviara el mensaje de que el Brazo se encuentra ...
                // inhabilitado
                if (i = 0){
                    enviarPalabra("Opción A: Brazo robótico inhabilitado.");
                    i++;
                }
            }

            else if (valorLectura == "b")
            {
                // Se utiliza un bucle for, porque al encontrarse dentro ...
                // de un do-while y manteniendo el caracter "b", siempre ...
                // entrara en esta selectiva, repitiendo el ciclo hasta ...
                // que se tenga un nuevo caracter en el UART
                for(i=0; i < 4; i++)
                {
                    // Lectura de los puertos ADC (0; 1; 2 y 3)
                    valorADC = leerPuerto(i);

                    // Se pasa el valor leido en el Puerto ADC i a la ...
                    // funcion cambiarAngulo, donde internamente hace ...
                    // la conversion del valorADC al del contador.
                    // (resolucion ADC: 4096 => valor max: 10000)

                    // Se aumenta en 1 el valor de "i" debido a que ...
                    // los canales del temporizador 3 son: 1; 2; 3 y 4
                    cambiarAngulo(i+1,valorADC);
                }
            }

            else if (valorLectura == "g")
            {

            }
            else if (valorLectura == "h")
            {

            }
            else if (valorLectura == "i")
            {

            }
            else
            {
                // En caso se envie un caracter incorrecto, se envia un mensaje ...
                // de error. Ademas, se vuelve a usar el caracter anterior al error, ...
                // para mantener el algoritmo que se estaba ejecutando
                enviarPalabra("Caracter INCORRECTO. Intentar nuevamente.");
                valorLectura = ultimoValorLectura;
            }
        }while(!(USART1->SR & (1<<5)));

        // En esta variable se guarda el ultimo caracter leido y que sea correcto. ...
        // De este modo no se interrumpa el algoritmo actual en caso se envie ...
        // un caracter incorrecto
        ultimoValorLectura = valorLectura;
    }

    return 0;
}