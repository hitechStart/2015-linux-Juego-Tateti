#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <pthread.h>
#include <SDL/SDL.h>
#include <netinet/in.h>
#include "extras.h"

typedef struct 	{
		int matriz[3][3];
		char * jugador1;
		int puntajeJugador1;
		char * jugador2;
		int puntajeJugador2;
                int empieza;
		}resultado;

typedef struct {
	     int j1;
	     int j2;
             int usado;	
}torn;						//Estructura usada para las rondas del torneo

//Variables globales
//#define PUERTO 8080
#define TAM_BUF 1024
#define NROMAX 10
int sen;	
int listen_socket;
int PORT;
int ESP;
int MEJORDE;
int CantJug;			 		 //Cantidad de clientes conectados
char nombres[25][100];                	 	 //Guardo los nombres de los jugadores 
torn t[100];
torn vec[100];
int VPartRond[100];			 	 //Guardo la cantidad de partidas por ronda
int partronda;			         	 //Guardo la cantidad de partidas de cada ronda
int caux;				 	 //Variable de contador de partidas

int comm_socket[NROMAX];			//Guardo file descriptor de los clientes

void DesactivarInterrupciones();		//Funcion que bloquea las interrupciones del usuario
void LeerArchivoConfiguracion();	 	//Obtiene los datos del archivo de configuracion  
void * comunicacion(void *);
int Rondas(int CantJug);
void FuncSig(int s);

//Main
int main(int argc, char *argv[])
{
 //DesactivarInterrupciones();
 LeerArchivoConfiguracion();

 //Variables utilizadas
 int i;
 void * ret;

  printf("---------------D-TaTeTi Tournament--------------\n");
  printf("*************************************************\n"); 
  printf("-------------------Servidor----------------------\n");
  int conexiones = 2;							//ESTE SE REEMPLAZA DPS POR EL DE ABAJO			
  int contador = 0;

  //Puntero de descriptores
  fd_set descriptoresLectura;

  printf("Funcion comunicacion. \n");

  //Se configura la direccion del servidor al que debo conectarme
  struct sockaddr_in listen_address;

  //Buffer de comunicacion
  datos paquete[2];
  resultado datosPartida;
  signal(14,FuncSig);			  				 //Activo la captura de la señal alarma
  //Creo el socket
  if((listen_socket = socket(PF_INET,SOCK_STREAM,0)) <0)
	printf("Error al crear el socket.\n");
  //Protocolo de la conexion
  listen_address.sin_family = AF_INET;
  listen_address.sin_port = htons(PORT);		  //Puerto de la conexion	
  listen_address.sin_addr.s_addr = htonl(INADDR_ANY);     //IP del servidor

  //Seteo en 0 a sin_zero
  bzero(&(listen_address.sin_zero),8);
  //Se asocia el puerto con la direccion del cliente jugador del que voy a escuchar
  bind(listen_socket, (struct sockaddr *) &listen_address, sizeof(struct sockaddr));

  printf("Esperando conexiones...\n");

  listen(listen_socket, conexiones);			     //Se comienzan a recibir las conexiones

  FD_ZERO (&descriptoresLectura);			    //Inicializo el puntero de descriptores	
  FD_SET (listen_socket, &descriptoresLectura);             //Le asocio al puntero el descriptor del servidor


   CantJug=0;
   sen=1;
   alarm(ESP);               			 			 //Tiempo para conectarse los clientes 
     while(sen){ 
         printf("%d\n",sen);
     	comm_socket[CantJug] = accept(listen_socket, 0, 0);
     	if (comm_socket[CantJug]< 0) {	
		printf("ERROR en accept");
                exit(1);
        }  
	recv(comm_socket[CantJug], &paquete[CantJug], sizeof(datos), 0);          //Guardo la informacion recibida del cliente jugador en el buffer
        strcpy(nombres[CantJug],paquete[CantJug].jugador);
	printf("Datos jugador %i recibidos.\n", CantJug+1);

	
        FD_SET(comm_socket[CantJug], &descriptoresLectura);            //Seteo el descriptor para cada jugador	
        CantJug++;							// "Cantjug" almacena la cantidad de jugadores del torneo(clientes)
       
     }	
 
 printf("Partida entre: %s y %s\n",paquete[0].jugador, paquete[1].jugador);

 //Seteamos los parametros de la partida de los jugadores
 paquete[0].simbolo = 1;
 paquete[0].turno = 1;
 paquete[0].seJuega = 1;
 strcpy(paquete[0].oponente, paquete[1].jugador);

 paquete[1].simbolo = 2;
 paquete[1].turno = 0;
 paquete[1].seJuega = 1;
 strcpy(paquete[1].oponente, paquete[0].jugador);

 //Envio los datos de la partida a cada jugador
 printf("Enviando datos a los jugadores..\n"); 
 send(comm_socket[0], &paquete[0], sizeof(datos), 0);
 printf("Envie inicio de partida al jugador 1\n");

 send(comm_socket[1], &paquete[1], sizeof(datos), 0);
 printf("Envie inicio de partida al jugador 2\n");
 int CantRondas;
 CantRondas=Rondas(CantJug);                                       //Guardo las rondas del torneo y armo el fixture

 printf("Numero maximo de socket %i %i\n", comm_socket[1]+1, FD_SETSIZE);

 //Comienza el juego
 while(paquete[0].termino == 0 && paquete[1].termino == 0){
//1er parametro es el maximo valor del descriptor que vamos a tratar (socket del jugador 2 en este caso) mas 1.
select (comm_socket[1]+1, &descriptoresLectura, NULL, NULL, NULL);
	
	//Por si termino el jugador 1
	if(paquete[1].termino == 0)
	{
		FD_ISSET (comm_socket[0], &descriptoresLectura);
		
		printf("Recibo del jugador 1\n");
		//Espero datos del jugador 1
		read(comm_socket[0], &paquete[0].matriz, sizeof(paquete[0].matriz));

		FD_ISSET (comm_socket[1], &descriptoresLectura);
		printf("Envio al jugador 2\n");
		//Envio datos del jugador 1 al jugador 2
		send(comm_socket[1], &paquete[0].matriz, sizeof(paquete[0].matriz), 0);
	}


	//Por si termino el jugador 2
	if(paquete[0].termino == 0)
	{
		FD_ISSET (comm_socket[1], &descriptoresLectura);
		printf("Recibo del jugador 2\n");
		//Espero datos del jugador 2
		read(comm_socket[1], &paquete[1].matriz, sizeof(paquete[1].matriz));

		FD_ISSET (comm_socket[0], &descriptoresLectura);
		printf("Envio al jugador 1\n");
		//Envio datos del jugador 2 al jugador 1
		send(comm_socket[0], &paquete[1].matriz, sizeof(paquete[1].matriz), 0);
	}
}

printf("Partida disputada entre %s y %s\n",paquete[0].jugador,paquete[0].oponente);

if(paquete[0].ganador == 0 && paquete[1].ganador == 0)
{
	printf("Partida empatada.\n");
}
else
{
	if(paquete[0].ganador == 1)
		printf("Ganador: %s! \n", paquete[0].jugador);
	else
		printf("Ganador: %s! \n", paquete[1].jugador);
}

//Cierro el socket
close(comm_socket);
close(listen_socket);
}

void DesactivarInterrupciones()					
{
 signal(SIGINT,SIG_IGN);  					//BLOQUEAR ctrl-C 
 signal(SIGQUIT,SIG_IGN); 					//BLOQUEAR ctrl-\ 
 signal(SIGTSTP,SIG_IGN); 					//BLOQUEAR ctrl-Z 
 signal(SIGTERM,SIG_IGN); 					//BLOQUEAR kill -15 
  
}

void LeerArchivoConfiguracion()
{char IP[16];
 FILE * entrada=fopen("Configuracion.txt","r");

 if(!entrada)
   {printf("Error al leer de Archivo de configuración.");
    exit(0);
   }
  
 char cadena[50];
 int cant=0;

 while (fgets(cadena, 50, entrada) != NULL){
	if(strcmp("\n",cadena)!=0){
	      if(cant==1)				//Obtengo el puerto del archivo
                 PORT=atoi(cadena);
              if(cant==3)				//Obtengo el tiempo de espera de conexion del archivo
                 ESP=atoi(cadena);
              if(cant==7)				//Obtengo la IP del archivo
                  strcpy(IP,cadena);		
              if(cant==9)				//Obtengo el num de partidas al "mejor de" del archivo
                  MEJORDE=atoi(cadena); 
            cant++;    
         }
  }
 close((int)entrada);
} 


int Rondas(int cjug){
 
  int f,z,c,w=1,s,tot=0,i;
  int ind=0;
   printf("%d\n\n\n",cjug);
  for(f=0;f<cjug;f++)
      for(c=f+1;c<cjug;c++)			 //Busco las partidas en la matriz tringular superior
           if(f!=c){				 //Guardo todas las partidas en un vector
		t[ind].j1=f;           		 //Guardo jugador 1   
		t[ind].j2=c;           	 	 //Guardo jugador 2 
                t[ind].usado=0;         	 //Auxiliar para saber si esa partida la use o no
	        ind++;
	   } 
  int rond=ind;					//Cantidad de partidas totales en el torneo
  int jug[cjug];
  int aux,aux2;				       
  for(f=0;f<cjug;f++)
      jug[f]=0;

   printf("\n");
  for(i=0;i<ind;i++)
      printf("La partida %d es %d vs %d\n",i,t[i].j1,t[i].j2); 
   printf("-----------------------------------RONDAS DEL TORNEO-----------------------\n\n");				
   while(rond){								  			//Crea el fixture
  	printf("                                   RONDA  %d                       \n",w);
        printf("------------------------------------------------------------------------\n");
 	 for(c=0;c<ind;c++){					 	   //Busco las partidas del primero al ultimo	
		aux=t[c].j1;
		aux2=t[c].j2;
	        if(jug[aux]==0 && jug[aux2]==0 && t[c].usado==0){  	   //Busco  las partidas no usadas y los jugadores no usados
	            printf("%s vs %s\n",nombres[aux],nombres[aux2]);       //Muestro quien juega contra quien 
		    vec[tot].j1=aux;
		    vec[tot].j2=aux2;	 
	            jug[aux]=1;
		    jug[aux2]=1; 
	            t[c].usado=1;					   //Marco la partida como que la use	
		    rond--;
                    tot++; 
		    partronda++;
	       }   
	  }
       VPartRond[w-1]=partronda;
       partronda=0;	
       w++; 
      for(f=0;f<cjug;f++)
         jug[f]=0;
        if(rond){
         	printf("                                   RONDA  %d                       \n",w);
       		printf("------------------------------------------------------------------------\n");
	 	for(s=ind-1;s>=0;s--){			               //Busco las partidas del ultimo al primero-La busqueda es alternada para disminuir la cantidad de rondas 	 
			aux=t[s].j1;
			aux2=t[s].j2;
	        	if(jug[aux]==0 && jug[aux2]==0 && t[s].usado==0){   
	            		printf("%s vs %s\n",nombres[aux],nombres[aux2]);
				vec[tot].j1=aux;
		    		vec[tot].j2=aux2;
	            		jug[aux]=1;
		    		jug[aux2]=1; 
	            		t[s].usado=1;
		    		rond--;	
                                tot++;	
				partronda++;
	      		 }  
	  	}
	      VPartRond[w-1]=partronda;
      	      partronda=0;		
       	      w++; 
	      for(f=0;f<cjug;f++)						//Inicializo el vector para la proxima pasada esten todas los jugadores disponibles
	         jug[f]=0;
        }
   }             
   printf("------------------------------------------------------------------------\n");  
  return w-1;                                                               		     	
}

void FuncSig(int s){
     int j,z,flag; 
    if(CantJug>=2){              		      				//Si entraron 2 o mas jugadores 
             printf("ENTRARON %d JUGADORES\n",CantJug);
             sen=0;
	     struct sockaddr_in serv_addr;
    	     struct hostent *server;
   										    
    	listen_socket = socket(AF_INET, SOCK_STREAM, 0);		       //Realiza la conexion con el localhost para que no se quede esperando 	
    	if (listen_socket < 0) 
           error("ERROR Al abrir socket");

    	in_addr_t data=inet_addr("127.0.0.1");           
    	if((server=gethostbyaddr(&data,4,AF_INET))==NULL){
              printf("gethostbyaddr()error\n");
              exit(-1);
        }
    	bzero((char *) &serv_addr, sizeof(serv_addr));
    	serv_addr.sin_family = AF_INET;
    	//bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    	serv_addr.sin_port = htons(PORT);
    	//serv_addr.sin_addr=*((struct in_addr*)server->h_addr);

    	if (connect(listen_socket,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
          error("ERROR conexion");

    	close(listen_socket);		   							
    }
    else{									//Si entraron menos de 2 jugadores finaliza			
        printf("********Termino el tiempo de conecciones***********\n");
        printf("**********Finaliza el juego***********************\n");
        exit(1);							  //<<<----------***********************CERRRAR LO QUE SE HAYA ABIERTO************************ 		
   }
}


