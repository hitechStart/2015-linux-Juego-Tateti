#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "extras.h"

#ifndef _GRAFICA_H_

//Funciones de la grafica del juego
#define _EXTRAS_H_

//Variables Globales
#define TABLERO "Imagenes/tablero.bmp"
#define CRUZ "Imagenes/Cruz.bmp"
#define CIRCULO "Imagenes/Circ.bmp"
#define FUENTE "Imagenes/font_arial.bmp"
#define SIN_MARCA "Imagenes/None.bmp"
#define CRUZ_SELECTED "Imagenes/CruzSelected.bmp"
#define CIRCULO_SELECTED "Imagenes/CircSelected.bmp"
#define POSIBLE_MOVIMIENTO "Imagenes/PosibleMov.bmp"

#define FALSO  0
#define VERDADERO 1

#define NO_INTERCAMBIAR 0
#define INTERCAMBIAR 1

#define VACIO 0
#define VALORCRUZ 1
#define VALORCIRCULO 2
#define VALORCRUZSELECCIONADO 3
#define VALORCIRCULOSELECCIONADO 4
#define VALOR_POSIBLE_MOVIMIENTO 9
#define DISPONIBLE_MOVER 3

#define DIF_ENCUADRE 13

/* Esta va a ser nuestra "fuente" */
struct tipoFuente{
	SDL_Surface *imagen;
	int ancho, alto;
	int anchoLetra, altoLetra;
	int primeraLetra;
} fuente;


//Funciones para eventos e imagenes
//Grafica la pantalla inicial
SDL_Surface * pantallaInicial(datos *);
//Obtiene la jugada ingresada por el usuario en la pantalla
int obtenerJugada(datos *, int, int);
//Setea el sector donde sera clickeado
int marcarSectorClickeado(int, int, datos *, int, int, int, int);
//Pinta el sector
void pintarSector(SDL_Surface * pantalla, datos *);
//Muestra el resultado de la partida en la pantalla
void pintarResultado(SDL_Surface * pantalla, datos *);
  
/* ----- Funciones para fuentes ----*/
  /* Leer una fuente desde fichero */
int leerFuente(char nombre[]);
  /* Mostrar toda la fuente en pantalla, sólo como auxiliar */
int mostrarFuente(int x, int y, SDL_Surface* pantalla);
  /* Escribir una letra */
int escribirLetra(int x, int y, char letra, SDL_Surface * pantalla);
  /* Escribir una frase */
int escribirFrase(int x, int y, char * frase, SDL_Surface * pantalla); 

char* convertirEnCadena(int numero);			// agregado por coronado

//Funcion arbitro que setea al ganador en el caso de que asi lo fuera
int arbitro();

#endif
