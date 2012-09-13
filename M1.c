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

void cliente (const char *IP, int puertoCliente){//función encargada de crear el proceso cliente, recibe la IP y el puerto cliente

int estadoConexion; //variable de identificacion del estado de conexion

struct sockaddr_in direccionServidor;
struct hostent *servidor;
char buffer[256];


int socketServidor = socket(AF_INET, SOCK_STREAM, 0);

if (socketServidor < 0){
fprintf(stderr,"No se puede crear el socket. \n\a");
exit(1);
}



servidor = gethostbyname(IP);


if (servidor == NULL) {
fprintf(stderr,"No existe o no se encuentra disponible el servidor solicitado. \a\n");
exit(1);
}

direccionServidor.sin_family = AF_INET;


bcopy((char *)servidor->h_addr,(char *)&direccionServidor.sin_addr.s_addr, servidor->h_length); //copia características del socket servidor

direccionServidor.sin_port = htons(puertoCliente);


int contador = 0;

while(1){ //while para espera de conexion con el cliente

if (estadoConexion >= 0){
	printf("Se ha establecido conexion\n");
	break;
}
else{
	estadoConexion = connect(socketServidor,(struct sockaddr *) &direccionServidor,sizeof(direccionServidor));
}
}


while(1){

bzero(buffer,256);//Se limpia el buffer con la funcion bzero

fgets(buffer,255,stdin);//Se obtiene del bash la informacion

/* En la linea anterior se utiliza:
--> \033[A  -> para inprimir la linea arriba del prompt
--> \033[2K -> para inicio de la linea del prompt
--> \033[01;36m -> para establecer el color cyan (para mensaje enviado)
--> \033[01;37m -> para establecer el colorcolor blanco (para cuerpo del mensaje)
*/
				
printf("\033[A\033[2K\033[01;36m""Mensaje Enviado:\033[00;37m %s",buffer);//Impresion de datos en pantalla con formato
				


if(strcmp(buffer,escape)==0){//Búsqueda del proceso hijo-cliente y terminarlo en caso de digitar la palabra de escape

write(socketServidor, buffer, strlen(buffer));//Escribe Adios en el buffer para posteriormente cerrar el proceso

int hijoID = getpid();//Se obtiene el ID del proceso cliente
hijoID++;
printf("\nCierre de sesion\n");
kill(hijoID, SIGKILL);//Funcion que termina el proceso de acuerdo al numero establecido
break;
}

else{
write(socketServidor, buffer, strlen(buffer));//sino se escribe normalmente en el socket la cadena a enviar
}
}

close(socketServidor);
}

//Metodo main donde se realizan las validaciones de los datos ingresados, ademas de la bifurcación
void main(int argc, char *argv[]){

printf("\nBienvenido al cliente de mensajeria instantanea\n\n");

int proceso = fork();

//La funcion atoi cambia de tipo string a int, necesario para el manejo interno de los puertos
int puertoServidor = atoi(argv[3]);
int puertoCliente = atoi(argv[2]);
const char *IP = argv[1]; // Se defne como puntero constante a un char ya que éste no va a ser modificado


if(proceso == 0){ //si aun no hay un proceso padre creado
	servidor(puertoServidor);
}
else{ //crea el proceso hijo
	cliente(IP, puertoCliente);
}
}
