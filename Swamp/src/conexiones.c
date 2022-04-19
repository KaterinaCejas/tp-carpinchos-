/*
 * conexiones.c
 *
 *  Created on: 18 nov. 2021
 *      Author: utnso
 */


#include "conexiones.h"


void iniciar_servidor(void)
{
	int socket_servidor = 0;
	int valor = 0;
	char* ip = configSwamp.IP;
	char* puerto = configSwamp.PUERTO;
	printf("ip: %s  puerto %s  \n", ip, puerto);

    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    getaddrinfo(ip, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        // Creamos el socket de escucha del servidor
     socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
     if(socket_servidor == -1){ log_info(logger, "No se pudo crear el socket"); exit(EXIT_FAILURE); }

    	// Asociamos el socket a un puerto
     valor = bind(socket_servidor, p->ai_addr, p->ai_addrlen);
     if(valor == -1){ log_info(logger, "No se pudo asociar el socket a un puerto"); exit(EXIT_FAILURE); }

     break;
    }

    // Escuchamos las conexiones entrantes
    valor = listen(socket_servidor, SOMAXCONN);
	if(valor == -1){ //SOMAXCONN es el maximo de conexiones en backlog que puede haber
		log_info(logger, "El servidor no pudo escuchar");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(servinfo);
	log_info(logger, "Servidor listo pa recibir al cliente");

    // Esperamos al cliente
	while(1){
		struct sockaddr_in dir_cliente;
		socklen_t tam_direccion = sizeof(struct sockaddr_in);
		int socket_cliente = 0;

		// Aceptamos un nuevo cliente

		socket_cliente = accept(socket_servidor, (void*)&dir_cliente, &tam_direccion);

		if(socket_cliente == -1) { log_info(logger, "Error al aceptar a un nuevo cliente"); continue;}

		log_info(logger, "Se conecto un cliente!");

		trabajemos_con_el_cliente(socket_cliente);

	}
}

void trabajemos_con_el_cliente(int socket_cliente)
{

		//char* asignacion;
		void* nbytes = NULL;
		int numero_pagina;
		int pid;
		log_info(logger, "entro a trabajemos con el cliente\n");
		int cod_op;
		int estado;
		void* respuesta;
		int numero_recibido = -1;
		int num1 = -1;
		int num2 = -1;

		while (1){
			cod_op = recibir_operacion(socket_cliente);
			switch (cod_op)
			{
				case ASIGNACION:
					asignacion_final = recibir_paquete_asignacion(socket_cliente);
					//set_asignacion(asignacion_final);
					log_info(logger, "tipo asignacion: %s", asignacion_final);
					//free(asignacion_final);
					break;
				case ASIGNACION2:
					recibir_dos_enteros(socket_cliente, &num1, &num2);
					//set_asignacion(asignacion_final);
					log_info(logger, "num1: %d, num2: %d", num1, num2);
					//free(asignacion_final);
					break;
				case READ:
					log_info(logger, "recibi codigo de operacion mensaje \n");
					recibir_paquete_read(socket_cliente,&pid, &numero_pagina);
					log_info(logger, "Llego recibir paquete read. pid: %d y nro_pagina: %d \n", pid, numero_pagina);
					respuesta = read_in_swap(numero_pagina,pid);
					log_info(logger, "Leyo en swap. Respuesta: %s \n", (char*)respuesta);
					enviar_respuesta_read(socket_cliente, respuesta);
					log_info(logger, "Envio respuesta. \n");
					usleep(configSwamp.RETARDO_SWAP * 1000);
					free(respuesta);
					break;
				case WRITE:
					log_info(logger, "Me llegaron los siguientes valores:\n");
					recibir_paquete_write(socket_cliente,&nbytes,&pid, &numero_pagina);
					log_info(logger, "pid: %d... nro_pagina: %d", pid, numero_pagina);
					estado = write_in_swap(numero_pagina,nbytes,pid);
					printf("Respuesta write: %d \n", estado);
					enviar_respuesta_write(socket_cliente, estado); //ARREGLAR
					printf("envie la respuesta\n");
					usleep(configSwamp.RETARDO_SWAP * 1000);
					free(nbytes);
					break;
				case DELETE:
					log_info(logger, "recibi codigo de operacion mensaje");
					pid = recibir_paquete_delete(socket_cliente);
					printf("Niggas in paris pid: %d", pid);
					delete_in_swap(pid);
					usleep(configSwamp.RETARDO_SWAP * 1000);
					break;
			}
	}
}


void* recibir_buffer(int* size, int socket_cliente){
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}


int recibir_entero(void* buffer){

	int entero;
	memcpy(&entero, buffer, sizeof(int));

	return entero;
}


void* recibir_cadena_de_bytes(void* buffer, int* tamanio_cadena){

	*tamanio_cadena = recibir_entero(buffer);

	void* cadena = malloc(*tamanio_cadena);

	memcpy(cadena, buffer+sizeof(int), *tamanio_cadena);

	return cadena;
}

char* recibir_paquete_asignacion(int socket_cliente){
	int size = -1;
	int tamanio_cadena = -1;

	void* buffer = recibir_buffer(&size, socket_cliente);

	char* palabra = recibir_cadena_de_bytes(buffer, &tamanio_cadena);

	return palabra;
}

void recibir_paquete_write(int socket_cliente,void** nbytes, int* pid, int* numero_pagina){
	int size = -1;
	int aux = 0;
	int tamanio_cadena = -1;

	void* buffer = recibir_buffer(&size, socket_cliente);

	*pid = recibir_entero(buffer);
	aux = aux + sizeof(int);
	*numero_pagina = recibir_entero(buffer + aux);
	aux = aux + sizeof(int);
	printf("VALOR DE AUX: %d \n\n\n", aux);
	printf("VALOR DE size: %d \n\n\n", size);
	*nbytes = recibir_nbytes(buffer + aux);
	printf("VALOR DE NBYTES: %s \n\n\n", (char*)*nbytes);
	printf("Longitud cadena: %d \n\n\n", strlen((char*)*nbytes));

	free(buffer);
}

void recibir_paquete_read(int socket_cliente,int* pid, int* numero_pagina){
	int size = 0;

	void* buffer = recibir_buffer(&size, socket_cliente);

	*numero_pagina = recibir_entero(buffer);
	*pid = recibir_entero(buffer + sizeof(int));

	free(buffer);
}


int recibir_paquete_delete(int socket_cliente){
	int size = 0;
	int pid;

	void* buffer = recibir_buffer(&size, socket_cliente);

	pid = recibir_entero(buffer);

	free(buffer);
	return pid;
}





void agregar_n_bytes_a_paquete(t_paquete* paquete, void* bytes, int tamanio){
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);
	memcpy(paquete->buffer->stream + paquete->buffer->size, bytes, tamanio);
	paquete->buffer->size += tamanio;
}


void enviar_respuesta_read(int socket_cliente, void* respuesta){
	t_paquete* paquete = crear_paquete();
	agregar_n_bytes_a_paquete(paquete,respuesta,configSwamp.TAMANIO_PAGINA);
	enviar_paquete(paquete,socket_cliente);
}

void agregar_entero_a_paquete(t_paquete* paquete, int numero){
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &numero, sizeof(int));

	paquete->buffer->size += sizeof(int);
}


void enviar_respuesta_write(int socket_cliente,int estado){
	t_paquete* paquete = crear_paquete();
	agregar_entero_a_paquete(paquete, estado);
	enviar_paquete(paquete,socket_cliente);


}

op_code recibir_operacion(int socket_cliente){
	int cod_op = -1;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) == -1)
	{
		cod_op = -1;
		log_warning(logger,	"cod_op = -1. No quieras meter la pata");
	}

	return cod_op;
}

// de aqui para abajo esta_todo bien

void* recibir_nbytes(void* buffer){

	void* mensaje = malloc(configSwamp.TAMANIO_PAGINA);
	memcpy(mensaje, buffer, configSwamp.TAMANIO_PAGINA);

	return mensaje;
}


void crear_buffer(t_paquete* paquete) {
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(void){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = RESPUESTA;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio) {
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente) {
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);


	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void* serializar_paquete(t_paquete* paquete, int bytes) {
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}


void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}



void recibir_dos_enteros(int socket_cliente, int* num1, int* num2)
{
	int size = 0;

	void* buffer = recibir_buffer(&size, socket_cliente);

	printf("\n\nel valor del size %d en doble puntero \n\n", size);
	*num1 = recibir_entero(buffer);

	*num2 = recibir_entero(buffer+sizeof(int));

	free(buffer);
}

