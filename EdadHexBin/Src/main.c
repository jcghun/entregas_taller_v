/*
 * main.c
 *
 *  Created on: Aug 18, 2022
 *      Author: Juan Camilo Gómez Hidalgo
 *      Email: jucgomezhi@unal.edu.co
 *      Fecha Nacimiento: 31/05/1995
 */

//Inicialización de variables
/**
 * 1. Se selecciona tipo char para la edad dado que no se conoce alguien con mas de 255 años.
 * Los valores de días por año y segundos por hora son constantes mayores a 255 (char) y
 * menores que 65535 (short). Se guardan variables con el prefijo orig para conservar el valor original,
 * si hace falta se genera otra variable para manipular este resultado
 */
unsigned char numeroAnios = 0;
unsigned short DIASPORANIO = 0;
unsigned short SEGPORHORA = 0;

unsigned short diasDesdeNacimiento = 0; //Variable creada para manipular el valor original Dias desde nacimiento
unsigned short origDiasNacimiento = 0;
unsigned int horasDesdeNacimiento = 0; //Variable creada para manipular el valor original Horas desde nacimiento
unsigned int origHorasNacimiento = 0;
unsigned int segDesdeNacimiento = 0; //Variable creada para manipular el valor original Segundos desde nacimiento
unsigned int origSegNacimiento = 0;

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
	origDiasNacimiento = numeroAnios * DIASPORANIO + 78;

	/**
	 * 3. Para calcular el número de horas desde el nacimiento se puede multiplicar el numero de
	 * días desde el nacimiento con las horas en un día (24h) y sumar las 12h para llegar al
	 * medio día del 17 de agosto. El valor supera las 200.000h por lo que se debe inicializar con
	 * tipo de variable unsigned int.
	 */

	//Creación y cálculo de la variable que contiene las horas desde el nacimiento
	origHorasNacimiento = origDiasNacimiento * 24 + 12;

	/**
	 * 4. Para el calculo del número de segundos transcurridos desde el nacimiento se puede realizar
	 * la multiplicación entre las horas transcurridas (origHorasNacimiento) y los segundos que hay
	 * en una hora (SEGPORHORA). El valor ronda los 850'000.000 y se puede usar tipo unsigned int.
	 */

	//Creación y cálculo de la variable que contiene los segundos desde el nacimeinto
	origSegNacimiento = origHorasNacimiento * SEGPORHORA;

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
	 * si se aplica el shift-izq 4 veces al número original se perdería información ya que al correr mas de
	 * dos veces el primer bit 1 (Posición 16 MSB) se perdería y en su lugar entra un valor 0 a la derecha del
	 * binario (LSB), no se podría recuperar el mismo valor regresando con un shift-derecha.
	 */
	//Aplicación de shift-izq a diasDesdeNacimiento
	diasDesdeNacimiento = origDiasNacimiento;
	diasDesdeNacimiento = diasDesdeNacimiento << 1;

	//Segunda aplicación de shift-izq
	diasDesdeNacimiento = diasDesdeNacimiento << 1;

	/**
	 * 7. Se sabe que la variable horasDesdeNacimiento es de tipo int (32 bits), si realizamos un shift-der las
	 * posiciones de los binarios se corren hacia la derecha, si se aplica una segunda vez el bit de valor 1 que
	 * estaba en la tercera casilla de derecha a izq quedará ubicado en la primera (LSB), si se aplica mas de dos
	 * veces el shift-der ocurre lo mismo que el anterior punto, se empezara a perder la información del binario
	 */
	//Aplicación de shift-der a horasDesdeNacimiento
	horasDesdeNacimiento = origHorasNacimiento;
	horasDesdeNacimiento = horasDesdeNacimiento >> 1;

	//Segunda aplicación de shift-der
	horasDesdeNacimiento = horasDesdeNacimiento >> 1;

	/**
	 * 8. Primero se guarda en una nueva variable el valor original de diasDesdeNacimiento, luego se utiliza la
	 * operación bitwise not (~) invirtiendo cada bit, a este valor se le sumó 1, al hacer esto se observa que el
	 * resultado es un complemento a dos (C2) del valor original de la variable, esto quiere decir que se está
	 * representando el mismo número pero es su valor negativo, por lo que al sumarse se entiende que el resultado da 0,
	 * esto va a ocurrir con cualquier valor dado que el procedimiento es el mismo, se realiza con horas desde nacimiento
	 */
	//Invirtiendo (NOT) el valor de origDiasNacimiento
	diasDesdeNacimiento = ~origDiasNacimiento;

	//Sumando 1 a este valor
	diasDesdeNacimiento ++;

	//Sumando valor de la variable original
	diasDesdeNacimiento = diasDesdeNacimiento + origDiasNacimiento;


	//Invirtiendo (NOT) el valor de origHorasNacimiento
	horasDesdeNacimiento = ~origHorasNacimiento;

	//Sumando 1 a este valor
	horasDesdeNacimiento ++;

	//Sumando valor de la variable original
	horasDesdeNacimiento = horasDesdeNacimiento + origHorasNacimiento;

	/**
	 * 9.El numero en la posición #4 es un 7, por lo tanto se hace una mascara para obtener las posiciones 0,2 y 6, para
	 * realizar esto se recuerda que cada posicion en hexadecimales representa 4 bits binarios, por lo que si se desea obtener
	 * esas posiciones basta con realizar una oparacion bitwise AND con un valor F (4 bites con valor 1) en las posiciones mencionadas
	 */
	//Se realiza la operacion bitwise AND (Máscara)
	segDesdeNacimiento = origSegNacimiento & 0x0F000F0F;

	/**
	 * 10. En el punto se aplica la máscara 0x040 y dado que el resultado es 0x40 se realiza el item (b)
	 */
	//Se realiza la operacion bitwise AND (Máscara)
	segDesdeNacimiento = origSegNacimiento & 0x040;

	//Ya que el resultado anterior es distinto de 0 se utiliza una máscara que solo elimine la posición 6.
	segDesdeNacimiento = origSegNacimiento & 0xF0FFFFFF;

	/**
	 * 11. Al aplicar el operador ! sobre la variable origSegNacimiento, se observa que su valor se convierte en 0, a este operador se
	 * le conoce como logical NOT por lo que al entregarle un valor distinto a un booleano, el operador regresa el valor 0, al aplicar
	 * de nuevo sobre el cero obtenido el operador niega el 0 binario y lo convierte en 1.
	 */
	segDesdeNacimiento = !origSegNacimiento;
	segDesdeNacimiento = !segDesdeNacimiento;
}
