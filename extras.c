#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <netinet/in.h>
#include "extras.h"

//Inicializa la matriz
void inicializarPartida(datos * paquete)
{
int i,j;

// Nombre del jugador
printf("Nombre del jugador: ");
scanf("%s",paquete->jugador);

//Matriz en 0
for(i=0; i<3; i++)
	for(j=0; j<3;j++)
		paquete->matriz[i][j]=0;


paquete->turno = 0;
paquete->simbolo = 0;
paquete->seJuega = 0;
paquete->termino = 0;
paquete->ganador = 0;
paquete->ronda = 0;
paquete->partidasGanadas = 0;
}

//Realiza la conexion entre el cliente y el servidor
int conectar(char * ip, int puerto)
{
//Se configura la direccion del servidor al que debo conectarme
struct sockaddr_in listen_address;

//Puerto
int caller_socket = 0;

//Creo el socket
if((caller_socket = socket(PF_INET,SOCK_STREAM,0)) <0)
	printf("Error al crear el socket.\n");

//Protocolo de la conexion
listen_address.sin_family = AF_INET;

//Puerto de la conexion
listen_address.sin_port = htons(puerto);

//IP del servidor
listen_address.sin_addr.s_addr = inet_addr(ip);

//Seteo en 0 a sin_zero
bzero(&(listen_address.sin_zero),8);

//Me conecto al servidor y espero
connect(caller_socket,(struct sockaddr *)&listen_address,sizeof(struct sockaddr));

return caller_socket;
}

//FUNCIÓN QUE BLOQUEA LAS INTERRUPCIONES DEL USUARIO.
void DesactivarInterrupciones()
{

 signal(SIGINT,SIG_IGN);  //BLOQUEAR ctrl-C //
 signal(SIGQUIT,SIG_IGN); //BLOQUEAR ctrl-\ //
 signal(SIGTSTP,SIG_IGN); //BLOQUEAR ctrl-Z //
 signal(SIGTERM,SIG_IGN); //BLOQUEAR kill -15// 
  
}

int LeerArchivoConfiguracionCliente(char *ip, int *puerto)
{
int CANTCLIENT;

 FILE * entrada=fopen("Configuracion.txt","r");

 if(!entrada)
	return -1;
  
 char cadena[50];
 int cant=0;

 while (fgets(cadena, 50, entrada) != NULL){
	if(strcmp("\n",cadena)!=0){
	      if(cant==1)
                 *puerto=atoi(cadena);	
	      if(cant==5)
                 CANTCLIENT=atoi(cadena);	
              if(cant==7)
                  strcpy(ip,cadena);
            cant++;    
         }
  }
 close((int)entrada);

 return 1;
} 

