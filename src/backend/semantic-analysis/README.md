# Analizador Semántico

¿Qué se debería implementar en esta parte del proyecto?

El analizador semántico debe ser el encargado de analizar el árbol de sintaxis abstracta construido desde el analizador sintáctico (_i.e._, Bison), aplicando todas las reglas del dominio específico que el lenguaje resuelve, garantizando que el significado (la semántica) del programa, es correcto. Esto implica verificar que las operaciones estén bien formadas, que los parámetros y tipos sean correctos, que la definición de las variables (en caso de existir dicho concepto), sean consistentes, entre muchos otros detalles asociados a la especificación del lenguaje.

Si el analizador semántico logra validar el programa completo, entonces se procede a la última fase: la __generación de código__. Nótese que la generación de código no necesariamente implica emitir un nuevo programa (por ejemplo, un ejecutable, o código en _assembler_ listo para compilar). Para un programa que permita procesar datos, la generación de código puede implicar emitir un set de datos, o un simple resultado, como es el caso de este proyecto base (que computa el resultado de un cálculo aritmético, y lo muestra por consola).
