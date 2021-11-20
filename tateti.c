#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include "extras.h"
#include "grafica.h"
#include <SDL/SDL.h>

//Estructura del paquete
datos paquete;

int cantidadJugadas = 0;

pthread_mutex_t mComunicar;
pthread_mutex_t mGraficar;
pthread_mutex_t mAccion;

//Funciones de los hilos
void * graficar(void *);
void * comunicacion(void *);
void * accion(void *);

//Main
void main(int argc,char * argv[])
{
int i;
void * ret;
pthread_t tareas[3];
pthread_attr_t attr;

//Buffer
//Inicializo el nombre del jugador
inicializarPartida(&paquete);

//DesactivarInterrupciones();
 

//MUTEX
//Inicializa los semaforos
pthread_mutex_init(&mGraficar, NULL);
pthread_mutex_init(&mComunicar, NULL);
pthread_mutex_init(&mAccion, NULL);

//Simulo una primer entrada para que la proxima vez se bloqueen
pthread_mutex_lock(&mGraficar);
pthread_mutex_lock(&mComunicar);
pthread_mutex_lock(&mAccion);

//HILOS
//Inicializacion de atributos
pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

//Creacion de los threads
pthread_create(&(tareas[0]), &attr, graficar, NULL);
pthread_create(&(tareas[1]), &attr, comunicacion, NULL);
pthread_create(&(tareas[2]), &attr, accion, NULL);

//Esperamos la finalizacion de los threads
for(i = 0; i<3; i++)
	pthread_join(tareas[i], &ret);

//Eliminamos los atributos
pthread_attr_destroy(&attr);

//Eliminamos los threads
pthread_exit(NULL);
}

void * graficar(void * var)
{
SDL_Surface *pantalla;

// Para testing!
int pantallaInicio = 1;
	
while(!paquete.termino)	
{
	pthread_mutex_lock(&mGraficar);
	printf("Entre a graficar..\n");

	if(pantallaInicio == 1)
	{
		pantalla = pantallaInicial(&paquete);
		pantallaInicio = 0;

	}
	else
		pintarSector(pantalla, &paquete);

}

pintarResultado(pantalla, &paquete);

//Hago un sleep para que pueda ver el resultado
sleep(5);

}

void * comunicacion(void * var)
{

//IP
char ip[16];
int puerto;

int caller_socket;

if(LeerArchivoConfiguracionCliente(&ip, &puerto)==-1)
	printf("Error al leer de Archivo de configuración.");

//Me conecto al servidor
printf("Esperando conexion al servidor...\n");
caller_socket = conectar(ip, puerto);

printf("Conexion establecida.\n", ip);

if(caller_socket == -1)
{
	printf("No se pudo conectar al servidor.\n");
	exit(1);
}

//Envio informacion
printf("Enviando datos al servidor (jugador).\n");
send(caller_socket, &paquete, sizeof(paquete), 0);

//Esperando jugadores
printf("Esperando oponente..\n");	
recv(caller_socket, &paquete, sizeof(paquete), 0);

if(paquete.seJuega == 0)
	printf("Se lo declara ganador, no hay oponente!\n");
else
	printf("Comienzo de la partida..\n");

//Pantalla inicial
pthread_mutex_unlock(&mGraficar);
sleep(2);

while(!paquete.termino)
{
	if(paquete.turno == 1)
	{
		pthread_mutex_unlock(&mAccion);
	
		//Se mantiene bloqueado hasta que se genera la accion		
		pthread_mutex_lock(&mComunicar);
		printf("Envio mi informacion al otro jugador!\n");

		//Envio datos
		send(caller_socket, &paquete.matriz, sizeof(paquete.matriz),0);					
	}
	else
	{
		//Le modifico el flag para que la proxima vez ejecute una jugada
		paquete.turno = 1;
	}

	if(!paquete.termino)
	{
		//Recibo el mensaje
		printf("Esperando datos del servidor.\n");
		recv(caller_socket, &paquete.matriz, sizeof(paquete.matriz), 0);

		printf("Recibi datos del servidor.\n");

		//Libero los semaforos	
		pthread_mutex_unlock(&mGraficar);
	}
}

//Cierro el socket
close(caller_socket);
}

void * accion(void * var)
{

while(!paquete.termino)
{
	pthread_mutex_lock(&mAccion);

	//El flag de no intercambiar va a ser util cuando la cantidad de jugadas sea mayor a 3.
	//En dicho caso primero se debe marcar las posibilidades y en la proxima accion se debe intercambiar.
	if (obtenerJugada(&paquete, cantidadJugadas, NO_INTERCAMBIAR) == -1)
		exit(1);
	
	// En caso que se hayan hecho las 3 jugadas correspondientes al jugador grafico las habilitadas
	if(cantidadJugadas < 3)
	{
		cantidadJugadas++;
	}
	else
	{
		pthread_mutex_unlock(&mGraficar);

		if (obtenerJugada(&paquete, cantidadJugadas, INTERCAMBIAR) == -1)
			exit(1);

	}

	pthread_mutex_unlock(&mGraficar);

	pthread_mutex_unlock(&mComunicar);
}

}
