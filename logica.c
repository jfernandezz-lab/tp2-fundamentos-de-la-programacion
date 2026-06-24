#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oponente.h"

#define DIMENSION_TABLERO 10
#define TAMANIO_MAX_LINEA 100
#define CODIGO_EXITO 0
#define CODIGO_ERROR 1
#define ESCANEO_CORRECTO 2
#define CANT_CAMPOS_CSV 4

#define LARGO_MIN_BARCO 2
#define LARGO_MAX_BARCO 5
#define TOTAL_CONFIGURACIONES_LARGO 6

#define CANT_REQUERIDA_LARGO_5 1
#define CANT_REQUERIDA_LARGO_4 1
#define CANT_REQUERIDA_LARGO_3 2
#define CANT_REQUERIDA_LARGO_2 1

#define COORDENADA_MIN_ENTRADA 1
#define COORDENADA_MAX_ENTRADA 10
#define INDICE_MIN_TDA 0
#define INDICE_MAX_TDA 9
#define DESPLAZAMIENTO_INDICE 1
#define BARCO_NO_ENCONTRADO -1

#define VALOR_VACIO 0
#define VALOR_CELDA_VACIA 0
#define BARCO_VIVO 0
#define BARCO_HUNDIDO 1

#define CARACTER_AGUA 'A'
#define CARACTER_TOCADO 'T'
#define CARACTER_HUNDIDO 'H'
#define CARACTER_BARCO 'B'

int validar_argumentos(int argc, char *argv[]);
int cargar_y_validar_flota(const char *archivo, barco_t flota[CANT_BARCOS]);
void liberar_flota_parcial(barco_t flota[CANT_BARCOS], int cant_cargados);
void ejecutar_partida(oponente_t *oponente, barco_t flota[CANT_BARCOS], const char *archivo_reporte);
void generar_archivo_reporte(const char *archivo, int j_aciertos, int j_erradas, int o_aciertos, int o_erradas, int o_hundidos, int j_sobreviven);
void destruir_flota(barco_t flota[CANT_BARCOS]);
void imprimir_tableros(char tablero_jugador[DIMENSION_TABLERO][DIMENSION_TABLERO], char tablero_enemigo[DIMENSION_TABLERO][DIMENSION_TABLERO]);
void procesar_ataque_jugador(char resultado_tiro, coordenada_t disparo, char tablero_enemigo[DIMENSION_TABLERO][DIMENSION_TABLERO], int *jugador_aciertos, int *jugador_erradas, int *oponente_hundidos);
void procesar_ataque_oponente(coordenada_t disparo_oponente, char tablero_jugador[DIMENSION_TABLERO][DIMENSION_TABLERO], barco_t flota[CANT_BARCOS], int *oponente_aciertos, int *oponente_erradas);
int contar_barcos_aliados_vivos(barco_t flota[CANT_BARCOS], char tablero_jugador[DIMENSION_TABLERO][DIMENSION_TABLERO]);
int verificar_barco_hundido(barco_t barco, char tablero_jugador[DIMENSION_TABLERO][DIMENSION_TABLERO]);

int validar_composicion_flota(int conteo_por_largo[TOTAL_CONFIGURACIONES_LARGO], int cantidad_barcos);
int validar_y_ubicar_segmentos(barco_t *barco, int f_ini, int c_ini, char dir, int tablero[DIMENSION_TABLERO][DIMENSION_TABLERO]);
int procesar_linea_flota(char *linea, barco_t *barco, int tablero[DIMENSION_TABLERO][DIMENSION_TABLERO], int conteo_por_largo[TOTAL_CONFIGURACIONES_LARGO]);

void inicializar_tablero_aliado(char tablero_jugador[DIMENSION_TABLERO][DIMENSION_TABLERO], barco_t flota[CANT_BARCOS]);
void vaciar_buffer_entrada(void);
int obtener_disparo_jugador(coordenada_t *disparo, int *entrada_jugador_agotada, char tablero_enemigo[DIMENSION_TABLERO][DIMENSION_TABLERO]);

int buscar_indice_barco_por_coordenada(coordenada_t coordenada, barco_t flota[CANT_BARCOS]);

/*
 * Pre: 'conteo_por_largo' es un vector válido de tamaño TOTAL_CONFIGURACIONES_LARGO. 
 * 'cantidad_barcos' es la cantidad total de barcos procesados con éxito.
 * Post: Compara el conteo contra los requisitos de la cátedra (1x5, 1x4, 2x3, 1x2).
 * Retorna CODIGO_EXITO (0) si la composición es perfecta, o CODIGO_ERROR (1) si falla.
 */
int validar_composicion_flota(int conteo_por_largo[TOTAL_CONFIGURACIONES_LARGO], int cantidad_barcos) {
    int resultado = CODIGO_EXITO;

    if (cantidad_barcos != CANT_BARCOS || 
        conteo_por_largo[5] != CANT_REQUERIDA_LARGO_5 || 
        conteo_por_largo[4] != CANT_REQUERIDA_LARGO_4 || 
        conteo_por_largo[3] != CANT_REQUERIDA_LARGO_3 || 
        conteo_por_largo[2] != CANT_REQUERIDA_LARGO_2) {
        
        resultado = CODIGO_ERROR;
    }

    return resultado;
}

/*
 * Pre: 'barco' apunta a una estructura barco_t válida con memoria ya reservada en 'posiciones'.
 * 'tablero' es la matriz de ocupación actual de 10x10.
 * Post: Valida límites y superposiciones. Si es válido, llena las posiciones en Base 0,
 * marca el tablero con 1 y retorna CODIGO_EXITO. Si falla, retorna CODIGO_ERROR.
 */
int validar_y_ubicar_segmentos(barco_t *barco, int f_ini, int c_ini, char dir, int tablero[DIMENSION_TABLERO][DIMENSION_TABLERO]) {
    int resultado = CODIGO_EXITO;
    int indice_segmento = 0;
    int es_valido = 1;

    while (es_valido && indice_segmento < barco->largo) {
        int fila_actual = f_ini + (dir == 'S' ? indice_segmento : (dir == 'N' ? -indice_segmento : 0));
        int columna_actual = c_ini + (dir == 'E' ? indice_segmento : (dir == 'O' ? -indice_segmento : 0));

        if (fila_actual < 1 || fila_actual > DIMENSION_TABLERO || 
            columna_actual < 1 || columna_actual > DIMENSION_TABLERO || 
            tablero[fila_actual - 1][columna_actual - 1] == 1) {
            
            es_valido = 0;
            resultado = CODIGO_ERROR;
        } else {
            tablero[fila_actual - 1][columna_actual - 1] = 1;
            barco->posiciones[indice_segmento].fila = fila_actual - 1;
            barco->posiciones[indice_segmento].columna = columna_actual - 1;
            indice_segmento++;
        }
    }

    return resultado;
}

/*
 * Pre: 'linea' es la cadena leída del archivo. 'barco' apunta a una posición de la flota.
 * 'tablero' es la matriz de ocupación. 'conteo_por_largo' es el vector acumulador.
 * Post: Descompone la línea, valida tamaño del barco, reserva memoria y delega la ubicación.
 * Retorna CODIGO_EXITO si la línea se procesó y ubicó bien, o CODIGO_ERROR en caso de fallo.
 */
int procesar_linea_flota(char *linea, barco_t *barco, int tablero[DIMENSION_TABLERO][DIMENSION_TABLERO], int conteo_por_largo[TOTAL_CONFIGURACIONES_LARGO]) {
    int resultado = CODIGO_EXITO;
    int f_ini, c_ini, largo;
    char dir;

    if (sscanf(linea, "%d;%d;%c;%d", &f_ini, &c_ini, &dir, &largo) != CANT_CAMPOS_CSV) {
        resultado = CODIGO_ERROR;
    }

    if (resultado == CODIGO_EXITO && (largo < LARGO_MIN_BARCO || largo > LARGO_MAX_BARCO)) {
        resultado = CODIGO_ERROR;
    }

    if (resultado == CODIGO_EXITO) {
        conteo_por_largo[largo]++;
        barco->largo = largo;
        barco->posiciones = malloc((size_t)largo * sizeof(coordenada_t));

        if (barco->posiciones == NULL) {
            resultado = CODIGO_ERROR;
        } else {
            resultado = validar_y_ubicar_segmentos(barco, f_ini, c_ini, dir, tablero);
        }
    }

    return resultado;
}

/*
 * Pre: 'archivo' es un path válido a un archivo CSV. 'flota' es un vector de 5 barcos.
 * Post: Coordina la lectura del archivo, delegando el procesamiento y la validación final.
 * Retorna 0 (CODIGO_EXITO) si la flota es totalmente válida, o 1 (CODIGO_ERROR) si algo falla.
 */
int cargar_y_validar_flota(const char *archivo, barco_t flota[CANT_BARCOS]) {
    int resultado_operacion = CODIGO_EXITO;
    FILE *archivo_flota = fopen(archivo, "r");

    if (archivo_flota == NULL) {
        resultado_operacion = CODIGO_ERROR;
    }

    if (resultado_operacion == CODIGO_EXITO) {
        int tablero_validacion[DIMENSION_TABLERO][DIMENSION_TABLERO] = {{0}};
        int conteo_por_largo[TOTAL_CONFIGURACIONES_LARGO] = {0}; 
        char linea_leida[TAMANIO_MAX_LINEA];
        int indice_barco = 0;
        int lectura_correcta = 1;

        while (lectura_correcta && indice_barco < CANT_BARCOS && fgets(linea_leida, TAMANIO_MAX_LINEA, archivo_flota) != NULL) {
            if (procesar_linea_flota(linea_leida, &flota[indice_barco], tablero_validacion, conteo_por_largo) == CODIGO_EXITO) {
                indice_barco++;
            } else {
                lectura_correcta = 0;
                resultado_operacion = CODIGO_ERROR;
                liberar_flota_parcial(flota, indice_barco);
            }
        }

        if (resultado_operacion == CODIGO_EXITO) {
            resultado_operacion = validar_composicion_flota(conteo_por_largo, indice_barco);
            if (resultado_operacion == CODIGO_ERROR) {
                liberar_flota_parcial(flota, indice_barco);
            }
        }

        fclose(archivo_flota);
    }

    return resultado_operacion;
}

/*
 * Pre: 'flota' es un vector de estructuras barco_t válido. 'cant_cargados' representa 
 * la cantidad de barcos (mayor o igual a cero y menor o igual a CANT_BARCOS) que llegaron 
 * a reservar memoria dinámica para sus posiciones antes de que ocurriera un fallo.
 * Post: Libera la memoria dinámica asignada al campo 'posiciones' de cada uno de los 
 * barcos indexados desde el inicio hasta 'cant_cargados' - 1, asignando NULL a los punteros 
 * liberados para evitar accesos inválidos.
 */
void liberar_flota_parcial(barco_t flota[CANT_BARCOS], int cant_cargados) {
    int indice_barco = 0;

    while (indice_barco < cant_cargados) {
        if (flota[indice_barco].posiciones != NULL) {
            free(flota[indice_barco].posiciones);
            flota[indice_barco].posiciones = NULL;
        }
        indice_barco++;
    }

    return;
}

/*
 * Pre: 'tablero_jugador' y 'tablero_enemigo' son matrices válidas de caracteres 
 * de tamaño DIMENSION_TABLERO x DIMENSION_TABLERO que representan el estado actual 
 * de la flota aliada y los disparos efectuados al oponente, respectivamente.
 * Post: Imprime de forma simétrica y formateada ambos tableros en la salida estándar (stdout), 
 * reemplazando las celdas vacías (0) por puntos ('.') para mejorar la legibilidad visual.
 */
void imprimir_tableros(char tablero_jugador[DIMENSION_TABLERO][DIMENSION_TABLERO], char tablero_enemigo[DIMENSION_TABLERO][DIMENSION_TABLERO]) {
    int fila = 0;
    int columna = 0;

    printf("\n   TU TABLERO (Barcos)         TABLERO ENEMIGO (Disparos)\n");
    printf("   1 2 3 4 5 6 7 8 9 10         1 2 3 4 5 6 7 8 9 10\n");

    while (fila < DIMENSION_TABLERO) {
        printf("%2d ", fila + 1);
        
        columna = 0;
        while (columna < DIMENSION_TABLERO) {
            printf("%c ", tablero_jugador[fila][columna] ? tablero_jugador[fila][columna] : '.');
            columna++;
        }
        
        printf("      %2d ", fila + 1);
        
        columna = 0;
        while (columna < DIMENSION_TABLERO) {
            printf("%c ", tablero_enemigo[fila][columna] ? tablero_enemigo[fila][columna] : '.');
            columna++;
        }
        
        printf("\n");
        fila++;
    }
    printf("\n");

    return;
}

/*
 * Pre: 'tablero_jugador' es una matriz de caracteres inicializada en 0 de 10x10.
 * 'flota' es un vector válido cargado con CANT_BARCOS.
 * Post: Registra con el carácter 'B' las posiciones geográficas de cada barco en la matriz.
 */
void inicializar_tablero_aliado(char tablero_jugador[DIMENSION_TABLERO][DIMENSION_TABLERO], barco_t flota[CANT_BARCOS]) {
    int indice_barco = 0;
    int indice_posicion = 0;

    while (indice_barco < CANT_BARCOS) {
        indice_posicion = 0;
        while (indice_posicion < flota[indice_barco].largo) {
            int fila_b = flota[indice_barco].posiciones[indice_posicion].fila;
            int columna_b = flota[indice_barco].posiciones[indice_posicion].columna;
            tablero_jugador[fila_b][columna_b] = 'B';
            indice_posicion++;
        }
        indice_barco++;
    }
    return;
}

/*
 * Pre: Ninguna.
 * Post: Descarta de forma iterativa todos los caracteres residuales del búfer de entrada estándar (stdin) 
 * hasta encontrar un salto de línea o el fin del archivo (EOF).
 */
void vaciar_buffer_entrada(void) {
    int caracter_leido = getchar();
    while (caracter_leido != '\n' && caracter_leido != EOF) {
        caracter_leido = getchar();
    }
    return;
}

/*
 * Pre: 'disparo' apunta a una estructura coordenada_t válida. 'entrada_jugador_agotada' apunta a un flag.
 * 'tablero_enemigo' es la matriz de disparos realizados.
 * Post: Lee de la consola el tiro. Si hay EOF o error de formato, o si es un rango/tiro repetido, 
 * avisa imprimiendo el mensaje correspondiente. Devuelve 1 si el tiro es legal y se puede procesar, u 0 si no.
 */
int obtener_disparo_jugador(coordenada_t *disparo, int *entrada_jugador_agotada, char tablero_enemigo[DIMENSION_TABLERO][DIMENSION_TABLERO]) {
    int turno_valido = 0;
    int resultado_scanf = scanf("%d;%d", &(disparo->fila), &(disparo->columna));

    if (resultado_scanf == EOF) {
        *entrada_jugador_agotada = 1;
    } else if (resultado_scanf != ESCANEO_CORRECTO) {
        vaciar_buffer_entrada();
        printf(">> Error: Formato invalido. Intente de nuevo.\n");
    } else {
        if (disparo->fila < COORDENADA_MIN_ENTRADA || disparo->fila > COORDENADA_MAX_ENTRADA || 
            disparo->columna < COORDENADA_MIN_ENTRADA || disparo->columna > COORDENADA_MAX_ENTRADA) {
            printf(">> Error: Coordenada fuera de rango.\n");
            turno_valido = 0;
        } else if (tablero_enemigo[disparo->fila - 1][disparo->columna - 1] != VALOR_VACIO) {
            printf(">> Error: Ya disparaste ahi.\n");
            turno_valido = 0;
        } else {
            turno_valido = 1;
        }
    }

    return turno_valido;
}

/*
 * Pre: 'oponente' es un puntero válido a la estructura del TDA oponente. 'flota' es la colección 
 * de barcos aliados cargada de forma correcta. 'archivo_reporte' es la ruta al archivo de texto de salida.
 * Post: Ejecuta y controla el ciclo principal del juego por turnos. Al finalizar, genera el archivo 
 * de reporte y actualiza visualmente los tableros impresos.
 */
void ejecutar_partida(oponente_t *oponente, barco_t flota[CANT_BARCOS], const char *archivo_reporte) {
    int j_aciertos = 0, j_erradas = 0, o_aciertos = 0, o_erradas = 0, o_hundidos = 0;
    int j_sobreviven = CANT_BARCOS;
    int entrada_jugador_agotada = 0;
    
    char tablero_jugador[DIMENSION_TABLERO][DIMENSION_TABLERO] = {{0}};
    char tablero_enemigo[DIMENSION_TABLERO][DIMENSION_TABLERO] = {{0}};

    inicializar_tablero_aliado(tablero_jugador, flota);

    int partida_activa = 1;

    while (partida_activa) {
        int turno_jugador_exitoso = 0;

        if (!entrada_jugador_agotada) {
            imprimir_tableros(tablero_jugador, tablero_enemigo);
            printf("Ingrese disparo (Fila;Columna): ");
            fflush(stdout);

            coordenada_t disparo;
            if (obtener_disparo_jugador(&disparo, &entrada_jugador_agotada, tablero_enemigo) == 1) {
                coordenada_t disparo_tda;
                disparo_tda.fila = disparo.fila - 1;
                disparo_tda.columna = disparo.columna - 1;

                char respuesta_tiro = oponente_recibe_disparo(oponente, disparo_tda);
                procesar_ataque_jugador(respuesta_tiro, disparo, tablero_enemigo, &j_aciertos, &j_erradas, &o_hundidos);
                turno_jugador_exitoso = 1;
            }
        }

        if (o_hundidos == CANT_BARCOS) {
            partida_activa = 0;
        }
        if (entrada_jugador_agotada && j_sobreviven == 0) {
            partida_activa = 0;
        }

        if (partida_activa && (turno_jugador_exitoso || entrada_jugador_agotada)) {
            coordenada_t disp_o = oponente_realiza_disparo(oponente);

            if (disp_o.fila >= INDICE_MIN_TDA && disp_o.fila <= INDICE_MAX_TDA && 
                disp_o.columna >= INDICE_MIN_TDA && disp_o.columna <= INDICE_MAX_TDA) {
                
                procesar_ataque_oponente(disp_o, tablero_jugador, flota, &o_aciertos, &o_erradas);
                j_sobreviven = contar_barcos_aliados_vivos(flota, tablero_jugador);
            } else {
                partida_activa = 0;
            }

            if (j_sobreviven == 0) {
                partida_activa = 0;
            }
            if (entrada_jugador_agotada) {
                partida_activa = 0;
            }
        }
    }

    imprimir_tableros(tablero_jugador, tablero_enemigo);
    generar_archivo_reporte(archivo_reporte, j_aciertos, j_erradas, o_aciertos, o_erradas, o_hundidos, j_sobreviven);

    return;
}

/*
 * Pre: 'resultado_tiro' contiene un carácter válido devuelto por el TDA oponente ('A', 'T' o 'H').
 * 'disparo' es una estructura con las coordenadas en Base 1 ingresadas por el usuario.
 * 'tablero_enemigo' es la matriz de tamaño DIMENSION_TABLERO x DIMENSION_TABLERO que registra el mapa de tiros.
 * 'jugador_aciertos', 'jugador_erradas' y 'oponente_hundidos' son punteros válidos a enteros de control del juego.
 * Post: Traduce la coordenada a Base 0, actualiza la celda correspondiente del tablero enemigo con el resultado,
 * e incrementa de forma exacta el contador pertinente por referencia según el impacto sea agua, impacto o hundimiento.
 */
void procesar_ataque_jugador(char resultado_tiro, coordenada_t disparo, char tablero_enemigo[DIMENSION_TABLERO][DIMENSION_TABLERO], int *jugador_aciertos, int *jugador_erradas, int *oponente_hundidos) {
    int fila_base_cero = disparo.fila - DESPLAZAMIENTO_INDICE;
    int columna_base_cero = disparo.columna - DESPLAZAMIENTO_INDICE;

    tablero_enemigo[fila_base_cero][columna_base_cero] = resultado_tiro;

    if (resultado_tiro == CARACTER_AGUA) {
        (*jugador_erradas)++;
    } else {
        (*jugador_aciertos)++;
        if (resultado_tiro == CARACTER_HUNDIDO) {
            (*oponente_hundidos)++;
        }
    }

    return;
}

/*
 * Pre: 'coordenada' contiene índices válidos en Base 0 (0 a 9). 'flota' es un vector cargado correctamente.
 * Post: Explora la colección de posiciones de la flota. Si localiza correspondencia exacta con la coordenada, 
 * retorna el índice del barco (0 a CANT_BARCOS - 1) y frena la búsqueda limpiamente. 
 * Si no pertenece a ninguna entidad, retorna BARCO_NO_ENCONTRADO (-1).
 */
int buscar_indice_barco_por_coordenada(coordenada_t coordenada, barco_t flota[CANT_BARCOS]) {
    int indice_resultado = BARCO_NO_ENCONTRADO;
    int indice_barco = 0;
    int barco_encontrado = 0;

    while (barco_encontrado == 0 && indice_barco < CANT_BARCOS) {
        int indice_posicion = 0;
        while (barco_encontrado == 0 && indice_posicion < flota[indice_barco].largo) {
            if (flota[indice_barco].posiciones[indice_posicion].fila == coordenada.fila && 
                flota[indice_barco].posiciones[indice_posicion].columna == coordenada.columna) {
                
                indice_resultado = indice_barco;
                barco_encontrado = 1;
            }
            indice_posicion++;
        }
        indice_barco++;
    }

    return indice_resultado;
}

/*
 * Pre: 'disparo_oponente' contiene una coordenada válida en Base 0 generada por el TDA.
 * 'tablero_jugador' es una matriz válida de tamaño DIMENSION_TABLERO x DIMENSION_TABLERO.
 * 'flota' es el vector con los barcos del jugador. 'oponente_aciertos' y 'oponente_erradas' apuntan a enteros válidos.
 * Post: Procesa el impacto modificando el estado de la celda afectada. Si se impacta un barco ('B'), se evalúa si 
 * quedó completamente hundido para actualizar la totalidad de sus celdas con 'H'; en caso contrario se marca 'T'.
 */
void procesar_ataque_oponente(
    coordenada_t disparo_oponente, 
    char tablero_jugador[DIMENSION_TABLERO][DIMENSION_TABLERO], 
    barco_t flota[CANT_BARCOS], 
    int *oponente_aciertos, 
    int *oponente_erradas
) {
    char *celda_atacada = &tablero_jugador[disparo_oponente.fila][disparo_oponente.columna];

    if (*celda_atacada == VALOR_CELDA_VACIA) {
        *celda_atacada = CARACTER_AGUA;
        (*oponente_erradas)++;
    } else if (*celda_atacada == CARACTER_AGUA) {
        (*oponente_erradas)++; 
    } else if (*celda_atacada == CARACTER_TOCADO || *celda_atacada == CARACTER_HUNDIDO) {
        (*oponente_aciertos)++; 
    } else if (*celda_atacada == CARACTER_BARCO) {
        (*oponente_aciertos)++;
        *celda_atacada = CARACTER_TOCADO;
        
        int indice_barco_afectado = buscar_indice_barco_por_coordenada(disparo_oponente, flota);

        if (indice_barco_afectado != BARCO_NO_ENCONTRADO && 
            verificar_barco_hundido(flota[indice_barco_afectado], tablero_jugador) == 1) {
            
            int indice_posicion = 0;
            while (indice_posicion < flota[indice_barco_afectado].largo) {
                int fila_barco = flota[indice_barco_afectado].posiciones[indice_posicion].fila;
                int columna_barco = flota[indice_barco_afectado].posiciones[indice_posicion].columna;
                
                tablero_jugador[fila_barco][columna_barco] = CARACTER_HUNDIDO;
                indice_posicion++;
            }
        }
    }

    return;
}

/*
 * Pre: 'barco' es una estructura válida que cuenta con memoria asignada de forma correcta en su vector de posiciones.
 * 'tablero_jugador' es una matriz de caracteres válida de dimensiones DIMENSION_TABLERO x DIMENSION_TABLERO.
 * Post: Examina las coordenadas geográficas de la embarcación en el mapa. Si encuentra que al menos uno de sus 
 * segmentos todavía retiene el carácter CARACTER_BARCO ('B'), dictamina que sigue a flote devolviendo BARCO_VIVO (0).
 * Si todos los segmentos fueron impactados, devuelve BARCO_HUNDIDO (1).
 */
int verificar_barco_hundido(barco_t barco, char tablero_jugador[DIMENSION_TABLERO][DIMENSION_TABLERO]) {
    int estado_barco = BARCO_HUNDIDO;
    int indice_posicion = 0;

    while (estado_barco == BARCO_HUNDIDO && indice_posicion < barco.largo) {
        int fila = barco.posiciones[indice_posicion].fila;
        int columna = barco.posiciones[indice_posicion].columna;

        if (tablero_jugador[fila][columna] == CARACTER_BARCO) {
            estado_barco = BARCO_VIVO;
        }
        
        indice_posicion++;
    }

    return estado_barco;
}

/*
 * Pre: 'flota' es un vector válido correctamente inicializado de tamaño CANT_BARCOS.
 * 'tablero_jugador' es una matriz de caracteres válida de tamaño DIMENSION_TABLERO x DIMENSION_TABLERO.
 * Post: Explora la flota aliada y contabiliza cuántas embarcaciones no se encuentran hundidas 
 * basándose en el estado de sus posiciones en el tablero, retornando la cantidad total como un entero.
 */
int contar_barcos_aliados_vivos(barco_t flota[CANT_BARCOS], char tablero_jugador[DIMENSION_TABLERO][DIMENSION_TABLERO]) {
    int cantidad_vivos = 0;
    int indice_barco = 0;

    while (indice_barco < CANT_BARCOS) {
        if (verificar_barco_hundido(flota[indice_barco], tablero_jugador) == 0) {
            cantidad_vivos++;
        }
        indice_barco++;
    }

    return cantidad_vivos;
}

/*
 * Pre: 'ruta_archivo' es una cadena de caracteres válida que representa la ubicación del reporte. 
 * Los parámetros de conteo representan estadísticas consolidadas no negativas de la partida.
 * Post: Intenta abrir el archivo en modo escritura. Si la apertura es exitosa, escribe de forma 
 * formateada el reporte estadístico final del juego, cierra el flujo de datos y finaliza la ejecución.
 */
void generar_archivo_reporte(const char *ruta_archivo, int jugador_aciertos, int jugador_erradas, int oponente_aciertos, int oponente_erradas, int oponente_hundidos, int jugador_sobreviven) {
    FILE *archivo_reporte = fopen(ruta_archivo, "w");

    if (archivo_reporte != NULL) {
        fprintf(archivo_reporte, "Balas aliadas acertadas: %d\n", jugador_aciertos);
        fprintf(archivo_reporte, "Balas aliadas erradas: %d\n", jugador_erradas);
        fprintf(archivo_reporte, "Balas enemigas acertadas: %d\n", oponente_aciertos);
        fprintf(archivo_reporte, "Balas enemigas erradas: %d\n", oponente_erradas);
        fprintf(archivo_reporte, "Barcos enemigos hundidos: %d\n", oponente_hundidos);
        fprintf(archivo_reporte, "Barcos aliados sobrevivientes: %d\n", jugador_sobreviven);
        
        fclose(archivo_reporte);
    }

    return;
}

/*
 * Pre: 'flota' es un vector de barcos de tamaño CANT_BARCOS donde se reservó de forma correcta 
 * memoria dinámica en el campo 'posiciones' de cada elemento.
 * Post: Libera secuencialmente la memoria dinámica de todas las posiciones asignadas, 
 * asigna NULL a cada puntero liberado para evitar referencias residuales y finaliza la ejecución.
 */
void destruir_flota(barco_t flota[CANT_BARCOS]) {
    int indice_barco = 0;

    while (indice_barco < CANT_BARCOS) {
        if (flota[indice_barco].posiciones != NULL) {
            free(flota[indice_barco].posiciones);
            flota[indice_barco].posiciones = NULL;
        }
        indice_barco++;
    }

    return;
}