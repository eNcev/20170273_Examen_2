#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "uart/uartbrazo.h"
#include "adc/potbrazo.h"
#include "pwm/servobrazo.h"

// Funcion reutilizada para realizar periodos de espera.
void delay(uint32_t seg)
{
	for(uint32_t i = 0; i < seg*100000; i++);
}

// Codigo principal a ejecutar.
int main()
{
    // Inicializacion de variables a usar.
    uint8_t valorLectura;
    int i = 0;
    int j = 0;
    int k = 0;
    uint32_t valorADC = 0;

    uint8_t ultimoValorLectura;
    uint8_t flagFull = 0;
    uint8_t flag = 0;
    uint8_t valorPare;

    // Inicialización del temporizador 3, del ADC 1 y del UART3.
    TIM3_Init();
    initADC();
    inicializarUart();

    // Se inicializa un arreglo de tamaño 10x4 con el valor 6000 ...
    // a ser usado en el codigo.
    uint32_t arregloNumeros[10][4];
    for(j=0; j<10; j++)
    {
        for(k=0; k<4; k++)
        {
            arregloNumeros[j][k] = 6000;
        }
    }

    // Se envia el mensaje de bienvenida por el periférico UART.
    enviarPalabra("BIENVENIDOS AL PROGRAMA DEL BRAZO ROBOTICO");

    while(1){

        valorLectura = leerCaracter();
        i = 0;
        j = 0;
        k = 0;

        // Se usa un Do-While para no interrumpir el código que se este ...
        // ejecutando, en caso se detecte un nuevo caracter. Para ello, ...
        // se monitorea el estado del bit 5 del registro USART_SR.
        do
        {
            if (valorLectura == "a")
            {
                // En caso se seleccione la opción "a", solo por una vez ...
                // se enviara el mensaje de que el Brazo se encuentra ...
                // inhabilitado.
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
                // que se tenga un nuevo caracter en el UART.
                for(i=0; i < 4; i++)
                {
                    // Lectura de los puertos ADC (0; 1; 2 y 3).
                    valorADC = leerPuerto(i);

                    // Se pasa el valor leido en el Puerto ADC i a la ...
                    // funcion cambiarAngulo, donde internamente hace ...
                    // la conversion del valorADC al del contador.
                    // (resolucion ADC: 4096 => valor max: 10000).

                    // Se aumenta en 1 el valor de "i" debido a que ...
                    // los canales del temporizador 3 son: 1; 2; 3 y 4.
                    cambiarAngulo(i+1,valorADC);
                }
            }

            else if (valorLectura == "g")
            {
                if(flagFull == 0)
                {
                    flagFull = 1;
                    for(j=0; j < 10; j++)
                    {
                        // La bandera flag se va a usar como señal para ...
                        // guardar una posicion cuando cambie a 1.
                        flag = 0;
                        do
                        {
                            for(k=0; k < 4; k++)
                            {
                                // Lectura de los puertos ADC (0; 1; 2 y 3).
                                arregloNumeros[j][k] = leerPuerto(k);
                                // Se envian los datos a los temporizadores para ...
                                // que el usuario pueda ver las posiciones que ...
                                // esta guardando.
                                cambiarAngulo(k+1,arregloNumeros[j][k]);
                            }

                            // Cuando el caracter 'r' se envie por el periferico UART ...
                            // se cambia el estado de la bandera 'flag' a 1 indicando ...
                            // que se guarde esa posicion de los 4 servomotores.
                            if (USART1->SR & (1<<5))
                            {
                                valorPare = leerCaracter();
                                if (valorPare == "r")
                                {
                                    flag = 1;
                                }
                                // En el caso se envie un caracter incorrecto, se envia ...
                                // un mensaje de error.
                                else
                                {
                                    enviarPalabra("Presione 'r' para guardar la posicion.");
                                }
                            }
                        }while(flag == 0);
                    }
                }
                else
                {
                    enviarPalabra("ARREGLO LLENO");
                }
            }
            else if (valorLectura == "h")
            {
                // Se utiliza un bucle for con el contador "j" para indicar ...
                // la fila del arreglo de donde se obtendran los 4 datos de ...
                // posicion para los servomotores. En conjunto, estos 4 ...
                // valores, corresponderan a la posicion "j" de 10 posibles.
                for(j=0; j<10; j++)
                {
                    // Se utiliza el bucle for con el contador "k" para indicar ...
                    // la columna del arreglo que equivale al dato respectivo de ...
                    // cada servomotor.
                    for(k=0; k<4; k++)
                    {
                        // Usando la funcion cambiarAngulo, se envia uno por uno los ...
                        // datos de posicion, indicando el canal del temporizador ...
                        // correspondiente.
                        cambiarAngulo(k+1,arregloNumeros[j][k]);
                    }
                    // Se realiza un retraso de 5 segundos entre posicion y posicion ...
                    // de los 4 servosmotores para que el brazo pueda llegar a ellas ...
                    // sin problemas, antes de pasar a una nueva.
                    delay(5);
                }
            }
            else if (valorLectura == "i")
            {
                // Se reinicia el arreglo de memoria con valores ...
                // de 6000. Primer bucle de filas ...
                for(j=0; j<10; j++)
                {
                    // ... y segundo de columnas.
                    for(k=0; k<4; k++)
                    {
                        arregloNumeros[j][k] = 6000;
                    }
                }
                // Dado que es un reinicio, terminado este proceso ...
                // se envia al robot a la opción de inhabilitado y ...
                // se coloca en cero la bandera flagFull, para poder ...
                // leer nuevos datos en caso se requiera.
                valorLectura = "a";
                flagFull = 0;
                
                // Se pone en 0 el contador "i", para asegurar que la lógica ...
                // de presentar el mensaje de "Brazo robótico inhabilitado" ...
                // funcione correctamente.
                i = 0;

            }
            else
            {
                // En caso se envie un caracter incorrecto, se envia un mensaje ...
                // de error. Ademas, se vuelve a usar el caracter anterior al error, ...
                // para mantener el algoritmo que se estaba ejecutando.
                enviarPalabra("Caracter INCORRECTO. Intentar nuevamente.");
                valorLectura = ultimoValorLectura;
            }
        }while(!(USART1->SR & (1<<5)));

        // En esta variable se guarda el ultimo caracter leido y que sea uno ...
        // de los que inicia un modo de operación.
        // De este modo no se interrumpa el algoritmo actual en caso se envie ...
        // un caracter incorrecto.
        ultimoValorLectura = valorLectura;

    }

    return 0;
}