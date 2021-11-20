#include "grafica.h"

SDL_Surface * pantallaInicial(datos * paquete)
{
SDL_Surface * pantalla;
SDL_Rect dest;
SDL_Event event;
SDL_Surface * fondo;

SDL_Rect src;

// Inicializo video 
if (SDL_Init(SDL_INIT_VIDEO) == -1){
	printf("Error: %s\n", SDL_GetError());
}
// Seteo propiedades de pantalla y obtengo el puntero a la superficie
pantalla = SDL_SetVideoMode(620, 450, 24, SDL_DOUBLEBUF);//SDL_SWSURFACE);
if (pantalla == NULL) {
	printf("Error: %s\n", SDL_GetError());
}
// Titulo de la ventana
SDL_WM_SetCaption("TA-TE-TI Tournament", NULL);

// Cargo imagen del tablero.
fondo = SDL_DisplayFormat(SDL_LoadBMP(TABLERO));
if ( fondo == NULL ) {
	printf("No pude cargar gráfico: %s\n", SDL_GetError());
}

// Definimos donde dibujaremos el gráfico
// y lo copiamos a la pantalla.
dest.x = 0;
dest.y = 0;
dest.w = fondo->w;
dest.h = fondo->h;

SDL_BlitSurface(fondo, NULL, pantalla, &dest);
// Mostramos la pantalla
SDL_Flip(pantalla);
// liberar superficie
SDL_FreeSurface(fondo);

/******** Inicio carga de nombres de jugadores, nros ronda *******/

leerFuente(FUENTE);
escribirFrase(440, 80, "RONDA", pantalla);
escribirFrase(440, 120, convertirEnCadena(paquete->ronda), pantalla);				// modificado por coronado		
escribirFrase(440, 160, paquete->jugador, pantalla);	
escribirFrase(440, 200, convertirEnCadena(paquete->partidasGanadas), pantalla);		// modificado por coronado		
escribirFrase(440, 240, paquete->oponente, pantalla);	
escribirFrase(440, 280, convertirEnCadena(paquete->partidasGanadas), pantalla);		// modificado por coronado		

/* Actualizamos la pantalla */


SDL_Flip(pantalla); 

return pantalla;
}

int obtenerJugada(datos *paquete, int cantidadJugadas, int intercambiar)
{
int sector;
SDL_Event event;
int done = 0;

//La variable "valido" almacena el valor que se tomara como valido para reemplazar
//La variable "asignar" almacena el valor que reemplazara a la celda valida
int valido;
int asignar;

//Vacio la cola de eventos
while(SDL_PollEvent(&event));

//En caso de haber hasta 3 jugadas se debe validar que la posision marcada sea vacia
//y se le asigne el simbolo del jugador
if(cantidadJugadas < 3)
{
	valido = VACIO;
	asignar = paquete->simbolo;
}
else
{
	//En caso de haber mas de 3 jugadas se debe validar que la posision marcada sea el simbolo
	//que le toco al jugador para luego poder marcar las posisiones posibles de intercambio
	if(intercambiar == 0)
	{
		valido = paquete->simbolo;
		asignar = paquete->simbolo + 2;
	}
	else
	{
		//En caso de haber mas de 3 y tener que intercambiar se debe validar que la posision marcada 
		//sea una posision disponible para luego poder marcarla con su simbolo 
		valido = VALOR_POSIBLE_MOVIMIENTO;
		asignar = paquete->simbolo;
	}
}

printf("Entre a obtener jugada!!\n");

while(done == 0)
{
		SDL_WaitEvent(&event);

		switch (event.type) 
		{   	
		 // PRESION DEL BOTON ESCAPE  		
		case SDL_KEYDOWN:   				
		   	if (event.key.keysym.sym == SDLK_ESCAPE)  
				done = -1;
		break;
		// CLICK EN LA GRILLA
		case SDL_MOUSEBUTTONDOWN:
		if(event.button.button == SDL_BUTTON_LEFT)
		{
			sector = marcarSectorClickeado(event.button.x,event.button.y,
				paquete,valido,asignar,intercambiar, cantidadJugadas);
			//Si se marco un sector correcto de la tabla sale de la funcion
			if(sector != -1)
				done = 1;
		}
		break;
		}
}

//Verifico si hubo un ganador
if(cantidadJugadas < 3 || (cantidadJugadas >= 3 && intercambiar == INTERCAMBIAR))
{
	if(arbitro(paquete)!=0)
	{
		paquete->ganador = 1;
		paquete->termino = 1;
		paquete->partidasGanadas++;
	}
}

printf("Obtuve la jugada!\n");
return done;

}

int marcarSectorClickeado(int x, int y, datos *paquete, int validar, int asignar, 
			int intercambiar, int cantidadJugadas)
{
int i,j,fila,columna;

//Valido que se marque la tabla
if(x<=31 || x>=375 || y<=85 || y>=421)
	return -1;

	// Columna 1
	if(x >= 31 && x<= 139){
		// Sector 1
		if(y >= 85 && y <= 191) {
			if(paquete->matriz[0][0] != validar)
				return -1;
			paquete->matriz[0][0] = asignar;
		}
		// Sector 4
		if(y >= 199 && y <= 307) {
			if(paquete->matriz[1][0] != validar)
				return -1;
			paquete->matriz[1][0] = asignar;
		}
		// Sector 7
		if(y >= 316 && y <= 421) {
			if(paquete->matriz[2][0] != validar)
				return -1;
			paquete->matriz[2][0] = asignar;
		}	
	}
	// Columna 2
	if(x >= 145 && x<= 256){
		// Sector 2
		if(y >= 85 && y <= 191) {
			if(paquete->matriz[0][1] != validar)
				return -1;
			paquete->matriz[0][1] = asignar;
		}
		// Sector 5
		if(y >= 199 && y <= 307) {
			if(paquete->matriz[1][1] != validar)
				return -1;
			paquete->matriz[1][1] = asignar;
		}
		// Sector 8
		if(y >= 316 && y <= 421) {
			if(paquete->matriz[2][1] != validar)
				return -1;
			paquete->matriz[2][1] = asignar;
		}	
	}
	// Columna 3
	if(x >= 262 && x<= 375){
		// Sector 3
		if(y >= 85 && y <= 191) {
			if(paquete->matriz[0][2] != validar)
				return -1;
			paquete->matriz[0][2] = asignar;
		}
		// Sector 6
		if(y >= 199 && y <= 307) {
			if(paquete->matriz[1][2] != validar)
				return -1;
			paquete->matriz[1][2] = asignar;
		}
		// Sector 9
		if(y >= 316 && y <= 421) {
			if(paquete->matriz[2][2] != validar)
				return -1;
			paquete->matriz[2][2] = asignar;
		}		
	}

	//SI HUBIERON 3 JUGADAS POR JUGADOR VERIFICO SI DEBE MARCAR O SE DEBE INTERCAMBIAR
	if(cantidadJugadas >= 3)
	{
		// VERIFICO SI DEBE MARCAR
		if(intercambiar == NO_INTERCAMBIAR)
		{	
		//Obtengo el sector seleccionado
		for(i = 0; i<3; i++)
			for(j = 0; j<3; j++)
				if(paquete->matriz[i][j]==asignar)
				{
					fila=i;
					columna=j;
				}

		//A partir del sector seleccionado marco los posibles movimientos
		for(i=0; i<3; i++)
			for(j=0; j<3; j++)
			{
				if(i!=fila || j!=columna)
					if(abs(i-fila) <=1 && abs(j-columna) <=1)
						if(paquete->matriz[i][j] == 0)
							paquete->matriz[i][j] = VALOR_POSIBLE_MOVIMIENTO;
			}

		}

		//VERIFICO SI HAY QUE INTERCAMBIAR
		if(intercambiar == INTERCAMBIAR)
		{
		//Obtengo el sector seleccionado
		for(i = 0; i<3; i++)
			for(j = 0; j<3; j++)
				if(paquete->matriz[i][j] == (paquete->simbolo+2) 
					|| paquete->matriz[i][j] == VALOR_POSIBLE_MOVIMIENTO)
					paquete->matriz[i][j]=0;
		}
	}

return 1;
}


void pintarSector(SDL_Surface * pantalla, datos *paquete)
{
SDL_Rect d;
SDL_Surface * imTemp;
int i,j;
int distanciaX = 0;
int distanciaY = 0;
char *vacio,
	*cruz, 
	*circulo,
	*circuloSeleccionado,
	*cruzSeleccionada,
	*posibleMovimiento,
	*imagen;

vacio = SIN_MARCA;
cruz = CRUZ;
circulo = CIRCULO;
circuloSeleccionado = CIRCULO_SELECTED;
cruzSeleccionada = CRUZ_SELECTED;
posibleMovimiento = POSIBLE_MOVIMIENTO;

for(i=0; i<3; i++)
{
	for(j=0; j<3; j++)
	{
		switch (paquete->matriz[i][j]) 
		{   	
		case VACIO:
			imagen = vacio;
		break;
		case VALORCIRCULO:   				
		   	imagen = circulo;
		break;
		case VALORCRUZ:
			imagen = cruz;
		break;
		case VALORCIRCULOSELECCIONADO:   				
		   	imagen = circuloSeleccionado;
		break;
		case VALORCRUZSELECCIONADO:
			imagen = cruzSeleccionada;
		break;
		case VALOR_POSIBLE_MOVIMIENTO:
			imagen = posibleMovimiento;
		break;
		}

		printf("Encontre lugar para pintar! Posicion en matriz: X=%i Y=%i\n", i, j);
		// Obtengo sector y sumo diferencia de encuadre para centrar bien
		d.x = 31 + distanciaX + DIF_ENCUADRE;
		d.y = 85 + distanciaY + DIF_ENCUADRE;

		imTemp = SDL_DisplayFormat(SDL_LoadBMP(imagen));
		if (imTemp == NULL)
		{
			printf("No pude cargar gráfico: %s\n", SDL_GetError());
			exit(1);
		}
		printf("Cargue la imagen!\n", imTemp->w);

		d.w = imTemp->w;
		d.h = imTemp->h;
		// Plasmo el dibujo en la pantalla
		SDL_BlitSurface(imTemp, NULL, pantalla, &d);
		printf("Plasmo el dibujo en la pantalla\n");
		// Refresco
		SDL_Flip(pantalla);
		printf("Refresco!\n");
		// Libero memoria
		printf("Libero memoria\n");
		SDL_FreeSurface(imTemp);

		distanciaX += 115;
	}
	distanciaX = 0;
	distanciaY += 115;
}

}

  
/* ----- Funciones para fuentes ----*/

/* Leer una fuente desde fichero */
int leerFuente(char nombre[]) {
    fuente.imagen = SDL_LoadBMP(nombre); 
    /* Los siguientes valores son prefijados para la fuente
    de ejemplo: 16x16 caracteres, cada uno de 16x16 pixels;
    habria que modificarlo si se usan fuentes de otro tamño */
    fuente.ancho=256;
    fuente.alto=256;
    fuente.anchoLetra=16;
    fuente.altoLetra=16;
    fuente.primeraLetra=0;
  }
  
  /* Mostrar toda la fuente en pantalla,
     sólo como auxiliar */
  int mostrarFuente(int x, int y, SDL_Surface* pantalla) {
    SDL_Rect destino; 
    destino.x = x;
    destino.y = y;
    //Plasmo el dibujo en la pantalla
    SDL_BlitSurface(fuente.imagen, NULL, pantalla, &destino); 
    
  }
  
  /* Escribir una letra */
  int escribirLetra(int x, int y, char letra, SDL_Surface* pantalla) {
    SDL_Rect destino, origen; 
    int fila, columna;
    int letrasPorFila, letrasPorColumna;
    
    letrasPorFila = fuente.ancho/fuente.anchoLetra;
    letrasPorColumna = fuente.alto/fuente.altoLetra;
    fila = letra / letrasPorColumna;
    columna = letra % letrasPorColumna;
    origen.x = columna * fuente.anchoLetra;
    origen.y = fila * fuente.altoLetra;
    origen.w = 16;
    origen.h = 16;
    destino.x = x;
    destino.y = y; 
    SDL_BlitSurface(fuente.imagen, &origen, pantalla, &destino); 
  }
  
  /* Escribir una frase */
  int escribirFrase(int x, int y, char *frase, SDL_Surface* pantalla) {
    int i;
    for (i=0; i<strlen(frase); i++)
      escribirLetra(x+i*fuente.anchoLetra, y, frase[i], pantalla);
  }
  
  /* ------ Fin de funciones para fuentes ------ */
  
/* Funcion para convertir un numero en una cadena de caracteres */  
char* convertirEnCadena(int numero){								// agregado por coronado
	char*	cadena = (char*)malloc(10);
	sprintf(cadena,"%d", numero);
	
	return cadena;	
}

/********** Funcion calcula ganador **********/
int arbitro(datos * paquete)
{
int f = 0, c = 2, contador = 0;

//Diagonal principal
for(f=0;f<3;f++)
	if(paquete->matriz[f][f] == paquete->simbolo)
		contador++;

if(contador == 3)
	return 1;

//Diagonal secundaria
contador = 0;
for(c=2;c>=0;c--)
	if(paquete->matriz[2-c][c] == paquete->simbolo)
		contador++;

if(contador == 3)
	return 1;

//Filas
for(f=0;f<3;f++)
{
	contador = 0;
	for(c=0;c<3;c++)
	{
		if(paquete->matriz[f][c] == paquete->simbolo)
			contador++;
	}
	
	if(contador == 3)
		return 1;
}

//Columnas
for(f=0;f<3;f++)
{
	contador = 0;
	for(c=0;c<3;c++)
	{
		if(paquete->matriz[c][f] == paquete->simbolo)
			contador++;
	}
	
	if(contador == 3)
		return 1;
}

return 0;
}


void pintarResultado(SDL_Surface *pantalla, datos *paquete)
{
	if(paquete->ganador == 1)
		escribirFrase(440, 330, "GANASTE!!!", pantalla);
	else
		escribirFrase(440, 330, "PERDISTE!!!", pantalla);

	/* Actualizamos la pantalla */
	SDL_Flip(pantalla); 
}
