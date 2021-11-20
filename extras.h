#ifndef _EXTRAS_H_

//Funciones extras
#define _EXTRAS_H_

typedef struct 	{
		int turno;		//Turno del jugador
		int simbolo;		//Simbolo que usara
		int matriz[3][3];	//Matriz de la partida
		char jugador [20];	//Nombre del jugador
		char oponente [20];	//Nombre del rival
		int seJuega;		//Flag que indica si la partida se juega
		int termino;		//Flag que indica que la partida conluyo
		int ganador;		//Flag que indica el ganador de la partida
		int ronda;		//Numero de ronda en la que se encuentra el jugador
		int partidasGanadas;	//Cantidad de partidas ganadas
		}datos;

//Inicializa la matriz
void inicializarBuffer(datos * paquete);
//Realiza la conexion entre el cliente y el servidor
int conectar(char * ip, int puerto);
//Desactiva interrupciones para que no pueda ser finalizado por señales
void DesactivarInterrupciones();
//Lee el archivo y obtiene datos necesarios para establecer la conexion
int LeerArchivoConfiguracionCliente(char *, int *);
#endif
