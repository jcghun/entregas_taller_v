/*
 * main.c
 *
 *  Created on: Aug 18, 2022
 *      Author: camilo
 */

//Inicialización de variables
/**
 * 1. Se selecciona tipo char para la edad dado que no se conoce alguien con mas de 255 años.
 * Los valores de días por año y segundos por hora son constantes mayores a 255 (char) y
 * menores que 65535 (short).
 */
unsigned char numeroAnios = 0;
unsigned short DIASPORANIO = 0;
unsigned short SEGPORHORA = 0;

unsigned short diasDesdeNacimiento = 0;
unsigned int horasDesdeNacimiento = 0;
unsigned int segDesdeNacimiento = 0;

int main(void){

	//Asignación de valores a las variables
	numeroAnios = 27;
	DIASPORANIO = 365;
	SEGPORHORA = 3600;

	/**
	 * 2. Para calcular el número de días desde nacimiento hasta el 17 de ago 2022 se multiplica
	 * la edad (numeroAnios) con los días que hay en un año (DIASPORANIO) y se suman los días
	 * faltantes desde el 31 de mayo (78 días).
	 */

	//Creación de la variable que contiene los días desde el nacimiento
	diasDesdeNacimiento = numeroAnios * DIASPORANIO + 78;

	/**
	 * 3. Para calcular el número de horas desde el nacimiento se puede multiplicar el numero de
	 * días desde el nacimiento con las horas en un día (24h) y sumar las 12h para llegar al
	 * medio día del 17 de agosto. El valor supera las 200.000h por lo que se debe inicializar con
	 * tipo de variable unsigned int.
	 */

	//Creación y cálculo de la variable que contiene las horas desde el nacimiento
	horasDesdeNacimiento = diasDesdeNacimiento * 24 + 12;

	/**
	 * 4. Para el calculo del número de segundos transcurridos desde el nacimiento se puede realizar
	 * la multiplicación entre las horas transcurridas (horasDesdeNacimiento) y los segundos que hay
	 * en una hora (SEGPORHORA). El valor ronda los 850'000.000 y se puede usar tipo unsigned int.
	 */

	//Creación y cálculo de la variable que contiene los segundos desde el nacimeinto
	segDesdeNacimiento = horasDesdeNacimiento * SEGPORHORA;

	/**
	 * 5. Para obtener los valores solicitados se realizo desde la perspectiva debugger observando
	 * "Live Expressions" y "Number Format".
	 *
	 * diasDesdeNacimiento (BIN) = 10011011001101
	 * horasDesdeNacimiento (BIN) = 111010001101000100
	 * segDesdeNacimiento (HEX) = 0x3327ec40
	 */

	/**
	 * 6. Primero se debe aclarar que la variable diasDesdeNacimiento al ser tipo short contiene 16 bits
	 * de información, en el binario presentado en el punto anterior se entiende que los dos valores que
	 * faltan a la derecha corresponden a ceros, al aplicar el shift-izq las posiciones se corren y ocupan
	 * uno de los ceros mencionados y al final del binario también se asigna un cero, en el segundo shift-izq
	 * el número que se obtiene es 1001101100110100, los ceros que antes estaban a la derecha pasaron a la izq
	 *
	 * si se aplicara el shift-izq 4 veces al número original se perdería información ya que al correr mas de
	 * dos veces el primer bit 1 se perdería y en su lugar entra un valor 0 a la derecha del binario, no se podría
	 * recuperar el mismo valor regresando con un shift-derecha.
	 */
	//Aplicación de shift-izq a diasDesdeNacimiento
	diasDesdeNacimiento = diasDesdeNacimiento << 1;

	//Segunda aplicación de shift-izq
	diasDesdeNacimiento = diasDesdeNacimiento << 1;
}

