/*________________________________________________________________________
	Instituto Tecnológico de Costa Rica
	Lenguajes de Programación	
	Primera Tarea Programada 
	Messenger en C
	Realizado por: 
			-> Yader Morales
		        -> Frank Brenes
			-> Mercedes Escalante

	Septiembre del 2012
__________________________________________________________________________*/



// Importacion de las bibliotecas de C a utlizar durante la progra:

// Biblioteca de entrada y salida de datos llamada Standar input output
	#include <stdio.h>
// Biblioteca estándar para uso de sockets
	#include <sys/socket.h> 
// Salida del programa en caso de error, uso de exit()
	#include <stdlib.h>     
	#include <sys/types.h>
// Biblioteca para el uso de base de datos en red
	#include <netdb.h>	
	#include <signal.h>
// Biblioteca para uso de fork()
	#include <unistd.h>
// Biblioteca que posee los headers para resolución de DNS
	#include <netinet/in.h> 
// Biblioteca para el uso de bzero y bcopy
	#include <string.h>     
// Define la palabra Adios como palabra de espace o cierre de la aplicacion
	#define escape "Adios\n"  



void servidor(int puertoServidor){//funcion que crea el servidor en el sistema, recibe el puerto desde el main

char buffer[256];//buffer o mensaje a enviar, se establece a 256 el tamano


struct sockaddr_in direccionServidor;//hace referencia a los elementos del socket nativo
struct sockaddr_in direccionCliente; //hace referencia a los elementos del socket del cliente


int SocketServidor = socket(AF_INET, SOCK_STREAM, 0);

if (SocketServidor < 0) {//Manejo de error en caso de que no se pueda crear el socket
fprintf(stderr,"No se puede crear el socket\n\a");
exit(1);
}

//Protocolos a seguir para la conexion entrante (cliente)

direccionServidor.sin_family = AF_INET; //Protocolo ARPA
direccionServidor.sin_addr.s_addr = INADDR_ANY; //Protocolo para usar la IP que se establezca


//se asigna el puerto servidor al socket nativo con htons (convierte a variable corta de red el int pasado como parametro)
direccionServidor.sin_port = htons(puertoServidor); 

// union del socket servidor con el entrante
bind(SocketServidor, (struct sockaddr *) &direccionServidor,sizeof(direccionServidor));

//el servidor escucha el cliente
listen(SocketServidor,2);

socklen_t largoDireccionCliente; //variable de dirección de tipo socket
largoDireccionCliente = sizeof(direccionCliente); //asigna a la variable el largo de la direccion del socket


int SocketCliente = accept(SocketServidor, (struct sockaddr *) &direccionCliente, &largoDireccionCliente);




while(1){ // ciclo de lectura del buffer del socket para su muestreo en pantalla
bzero (buffer,256);


read(SocketCliente, buffer, 255);

		
printf(	"\033[2K\r\033[01;32m""Mensaje Recibido:\033[00;37m %s",buffer);

/* En la linea anterior se utiliza:
--> \033[2K\r  -> para limpiar el prompt
--> \033[01;31m  -> poner color verde (para mensaje recibido)
--> \033[01;37m  -> poner color blanco (para cuerpo del mensaje)
*/				

if(strcmp(buffer, escape)==0){//Búsqueda del proceso padre-servidor y terminarlo en caso de digitar la palabra de escape

int PadreID = getpid();

PadreID--;

printf("\nCierre de sesion\n");
kill(PadreID, SIGKILL);
break;
}

}
close(SocketServidor);
close(SocketCliente);
}

