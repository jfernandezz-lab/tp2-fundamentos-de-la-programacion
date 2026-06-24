#include <stdio.h>
#include <stdlib.h>
#include "oponente.h"

#define ARGUMENTOS_REQUERIDOS 3
#define EXITO 0
#define ERROR 1

int validar_argumentos(int argc, char *argv[]);
int cargar_y_validar_flota(const char *archivo, barco_t flota[CANT_BARCOS]);
void destruir_flota(barco_t flota[CANT_BARCOS]);
void ejecutar_partida(oponente_t *oponente, barco_t flota[CANT_BARCOS], const char *archivo_reporte);

/*
 * Pre: 'argc' representa la cantidad de argumentos pasados por la línea de comandos de forma entera.
 * 'argv' es el vector de punteros a cadenas de caracteres con dichos argumentos.
 * Post: Coordina la validación, la carga de la flota, la creación del oponente del TDA, 
 * la ejecución de la partida y la posterior liberación ordenada de recursos.
 * Retorna EXIT_SUCCESS si finalizó correctamente o EXIT_FAILURE ante cualquier fallo intermedio.
 */
int main(int argc, char *argv[]) {
    int estado_final = EXIT_SUCCESS;
    int continuacion_valida = ERROR;
    barco_t flota[CANT_BARCOS];
    oponente_t *oponente = NULL;

    if (validar_argumentos(argc, argv) == EXITO) {
        if (cargar_y_validar_flota(argv[1], flota) == EXITO) {
            continuacion_valida = EXITO;
        } else {
            fprintf(stderr, "Error al cargar el archivo de barcos\n");
            estado_final = EXIT_FAILURE;
        }
    } else {
        estado_final = EXIT_FAILURE;
    }

    if (estado_final == EXIT_SUCCESS && continuacion_valida == EXITO) {
        oponente = oponente_crear(flota);
        if (oponente != NULL) {
            ejecutar_partida(oponente, flota, argv[2]);
            oponente_destruir(oponente);
            destruir_flota(flota);
        } else {
            fprintf(stderr, "Error al crear el oponente\n");
            destruir_flota(flota);
            estado_final = EXIT_FAILURE;
        }
    }

    return estado_final;
}

/*
 * Pre: 'argc' es la cantidad de argumentos recibidos por consola. 'argv' es un vector válido de strings.
 * Post: Valida que la cantidad de argumentos sea estrictamente la cantidad requerida por el programa. 
 * Retorna 0 (EXITO) si es correcto, o 1 (ERROR) imprimiendo el uso por stderr en caso contrario.
 */
int validar_argumentos(int argc, char *argv[]) {
    int codigo_retorno = EXITO;

    if (argc != ARGUMENTOS_REQUERIDOS) {
        fprintf(stderr, "Uso correcto: %s <archivo_barcos.csv> <archivo_reporte.txt>\n", argv[0]);
        codigo_retorno = ERROR;
    }

    return codigo_retorno;
}