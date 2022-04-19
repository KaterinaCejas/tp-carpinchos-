/*
 * conexiones.c
 *
 *  Created on: 30 oct. 2021
 *      Author: utnso
 */

#include "conexiones.h"

/*
void iniciarServidor(void) {
	int socketServidor = 0;
	int valor = 0;

    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    //getaddrinfo(configMemoria.ipMemoria, configMemoria.PUERTO_ESCUCHA, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next){
    	socketServidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		 if(socketServidor == -1){
			 //sem_wait();
			 log_error(logger, "No se pudo crear el socket");
			 //sem_post();
			 exit(EXIT_FAILURE);
		 }
		 valor = bind(socketServidor, p->ai_addr, p->ai_addrlen);
		 if(valor == -1){
			 //sem_wait();
			 log_error(logger, "No se pudo asociar el socket a un puerto");
			 //sem_post();
			 exit(EXIT_FAILURE);
		 }
		 break;
    }

    valor = listen(socketServidor, SOMAXCONN);
	if(valor == -1){
		//sem_wait();
		log_error(logger, "El servidor no pudo escuchar");
		//sem_post();
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(servinfo);
	//sem_wait();
	log_info(logger, "Memoria lista para recibir mensajes de la matelib");
	//sem_post();

	while(1){
		struct sockaddr_in dir_cliente;
		socklen_t tam_direccion = sizeof(struct sockaddr_in);
		int socketCliente = 0;
		socketCliente = accept(socketServidor, (void*)&dir_cliente, &tam_direccion);
		if(socketCliente == -1) {
			//sem_wait();
			log_error(logger, "Error al aceptar a un nuevo cliente");
			//sem_post();
			continue;
		}
		//recibirMensajeMateLib(socketCliente);
	}
}



int recibirOperacion(int socketCliente){
	int codigoOperacion = -1;
	if(recv(socketCliente, &codigoOperacion, sizeof(int), MSG_WAITALL) == -1){
		codigoOperacion = -1;
		//sem_wait();
		log_error(logger,	"Codigo de operación recibido es incorrecto/no existe");
		//sem_post();
	}
	return codigoOperacion;
}

void recibirMensajeMateLib(int socketCliente){
	int codigoOperacion = recibirOperacion(socketCliente);
	char* mensaje = string_new();
	switch (codigoOperacion){
		case MEMALLOC:
			mensaje = recibirMensaje(socketCliente);
			break;
		case MEMFREE:
			mensaje = recibirMensaje(socketCliente);
			break;
		case MEMREAD:
			mensaje = recibirMensaje(socketCliente);
			break;
		case MEMWRITE:
			mensaje = recibirMensaje(socketCliente);
			break;
		case -1:
			log_error(logger, "no se pudo leer el mensaje del cliente");
			break;
		default:
			log_error(logger,	"Codigo de operacion recibido no existe/incorrecto");
			break;
	}
	free(mensaje);
}
char* recibirMensaje(int socketCliente){
	int size;
	void * buffer;

	recv(socketCliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socketCliente, buffer, *size, MSG_WAITALL);

	return (char*)buffer;
}
*/


