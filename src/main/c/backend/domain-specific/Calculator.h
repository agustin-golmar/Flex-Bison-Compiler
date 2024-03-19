#ifndef CALCULATOR_HEADER
#define CALCULATOR_HEADER

/**
* La calculadora pertenece al dominio de esta aplicación, y por ello se
* codifican en este lugar sus modelos y operaciones. Para que el dominio
* específico se encuentre completamente desacoplado del compilador, debería
* ser posible calcular algo utilizando estas funciones directamente.
*/

int Add(const int leftAddend, const int rightAddend);

int Subtract(const int minuend, const int subtract);

int Multiply(const int multiplicand, const int multiplier);

int Divide(const int dividend, const int divisor);

#endif
