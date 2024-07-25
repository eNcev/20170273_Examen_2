#include <stdint.h>
#include "../incFiles/stm32f103x6.h"

void inicializarUart(void);
void enviarCaracter(uint8_t car);
void nuevaLinea(void);
void enviarPalabra(uint8_t palabra[]);
uint8_t leerCaracter(void);