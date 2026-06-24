# Proyecto Final: Batalla Naval en C

Este repositorio contiene el desarrollo de mi proyecto final individual para la materia **Fundamentos de la Programación** en la Facultad de Ingeniería de la Universidad de Buenos Aires (FIUBA). El software emula de forma completa una partida estratégica del clásico juego de la Batalla Naval contra un oponente virtual.

## Tecnologías y Conceptos Clave

El desarrollo fue realizado íntegramente en **C (estándar C99)** bajo entornos Linux, aplicando de forma estricta un paradigma de diseño robusto y buenas prácticas de ingeniería de software. 

Los pilares técnicos más importantes del proyecto incluyen:

* **Gestión Dinámica de Memoria:** Implementación rigurosa de asignación dinámica utilizando `malloc` para el manejo de las estructuras internas de la flota, garantizando un ciclo de vida limpio mediante la liberación secuencial de bloques (`free`) y el control estricto de fugas de memoria (*memory leaks*).
* **Contratos de Diseño:** Uso estricto de pre y postcondiciones lógicas en cada módulo para blindar el comportamiento del sistema.
* **Control de Flujo Limpio:** Erradicación total de salidas anómalas (`break` o `continue`) y retornos múltiples intrusivos, estructurando los bucles y algoritmos de búsqueda mediante banderas lógicas y un único punto de salida (`return único`).
* **Modularización de Responsabilidad Única:** Fragmentación del sistema en submódulos encapsulados independientes para procesamiento de archivos (parsing de CSV), validaciones posicionales de vectores y lógica de turnos.

## Aprendizaje y Conclusiones

Más allá de la resolución del juego en sí, el verdadero desafío y valor de este proyecto radicó en trabajar directamente a "bajo nivel". Desarrollar en C me brindó un entendimiento profundo sobre **el funcionamiento real de la memoria**, cómo interactúan los punteros con las estructuras de datos y la importancia crítica de la prolijidad y la optimización en la arquitectura de un software.

```bash
$ gcc *.c oponente.o -o batalla_naval -std=c99 -Wall -Wconversion -Werror -lm

Compilacion exitosa
~~~~~~~~ Corriendo pruebas internas ~~~~~~~~~

~~~~~~~~ Corriendo test 1 (test01_jugador_gana) ~~~~~~~~~
Descripcion: El jugador hunde toda la flota enemiga sin recibir ningun impacto
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 2 (test02_oponente_gana) ~~~~~~~~~
Descripcion: El oponente hunde toda la flota aliada antes de que el jugador acierte un solo disparo
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 3 (test03_archivo_vacio) ~~~~~~~~~
Descripcion: El archivo de barcos esta vacio: el programa debe terminar con error
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 4 (test04_barco_fuera_de_tablero) ~~~~~~~~~
Descripcion: Un barco se extiende mas alla del tablero: el programa debe terminar con error
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 5 (test05_direccion_invalida) ~~~~~~~~~
Descripcion: Un barco usa una direccion invalida (X): el programa debe terminar con error
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 6 (test06_largo_invalido) ~~~~~~~~~
Descripcion: Un barco tiene un largo no permitido (6): el programa debe terminar con error
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 7 (test07_composicion_invalida) ~~~~~~~~~
Descripcion: Composicion incorrecta de flota (dos barcos de largo 5, falta el de 4): el programa debe terminar con error
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 8 (test08_barcos_superpuestos) ~~~~~~~~~
Descripcion: Dos barcos se superponen en al menos una posicion: el programa debe terminar con error
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 9 (test09_partida_mixta_jugador_gana) ~~~~~~~~~
Descripcion: Partida realista con aciertos y fallos de ambos lados. El jugador gana.
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 10 (test10_partida_mixta_oponente_gana) ~~~~~~~~~
Descripcion: Partida realista con aciertos y fallos de ambos lados. El oponente gana.
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 11 (test11_superposicion_direcciones_distintas) ~~~~~~~~~
Descripcion: Dos barcos con direcciones distintas se cruzan en una posicion: el programa debe terminar con error.
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 12 (test12_posicion_inicial_fuera) ~~~~~~~~~
Descripcion: La posicion inicial de un barco esta fuera del tablero: el programa debe terminar con error
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 13 (test13_composicion_falta_barco) ~~~~~~~~~
Descripcion: La flota del jugador tiene menos barcos de lo esperados: el programa debe terminar con error
✓ Prueba Exitosa

~~~~~~~~ Corriendo test 14 (test14_disparo_fuera_tablero) ~~~~~~~~~
Descripcion: El jugador ingresa disparos fuera del tablero: el programa debe solicitar un nuevo disparo hasta que se ingrese uno válido
✓ Prueba Exitosa

~~~~~~~~ Pasaste 14 pruebas de 14 ~~~~~~~~~

Entrega aceptada!!!!!
