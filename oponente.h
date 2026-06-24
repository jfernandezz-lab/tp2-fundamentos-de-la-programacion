#ifndef __OPONENTE_H__
#define __OPONENTE_H__

#define CANT_BARCOS 5

typedef struct coordenada {
	int fila;
	int columna;
} coordenada_t;

typedef struct barco {
	coordenada_t *posiciones;
	int largo;
} barco_t;

typedef struct oponente oponente_t;

/*
 * Pre condiciones: Recibe las posiciones de los barcos del jugador (que fueron
 * leidos anteriormente del archivo), todas las posiciones de barcos_jugador
 * deben ser validas.
 * Post condiciones: Crea al oponente inicializando todos los campos del mismo.
 */
oponente_t *oponente_crear(barco_t barcos_jugador[CANT_BARCOS]);

/*
 * Pre condiciones: Oponente debe estar previamente creado con `oponente_crear`.
 * La coordenada recibida debe ser válida.
 * Post condiciones: Devuelve:
 * --> A si la coordenada elegida es "Agua"
 * --> T si la coordenada elegida es un barco pero no todas sus posiciones
 *     fueron tocadas.
 * --> H si la coordenada elegida es un barco y todas sus posiciones fueron
 *     tocadas.
 */
char oponente_recibe_disparo(oponente_t *oponente, coordenada_t disparo);

/*
 * Pre condiciones: Oponente debe estar previamente creado con `oponente_crear`.
 * Post condiciones: Devuelve la coordenada del disparo del oponente.
 */
coordenada_t oponente_realiza_disparo(oponente_t *oponente);

/*
 * Pre condiciones: Oponente debe estar previamente creado con `oponente_crear`.
 * Post condiciones: Elimina al oponente de la memoria junto con todos sus
 * campos.
 */
void oponente_destruir(oponente_t *oponente);

#endif /* __OPONENTE_H__ */
