/*
 * conexiones.h
 *
 *  Created on: 18 nov. 2021
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include <pthread.h>
#include "configSwamp.h"
#include "logger.h"
#include "Swamp.h"
#include <unistd.h>


typedef enum {
		ASIGNACION,
		ASIGNACION2,
		READ,
		WRITE,
		DELETE,
		RESPUESTA
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

void recibir_dos_enteros(int socket_cliente, int* num1, int* num2);
op_code recibir_operacion(int socket_cliente);
void trabajemos_con_el_cliente(int socket_cliente);
char* recibirMensaje(int socketCliente);
void recibirMensajeMemoria(int socketCliente);
int recibirOperacion(int socketCliente);
void iniciar_servidor(void);
void* recibir_buffer(int* size, int socket_cliente);
void recibir_mensaje(int socket_cliente);
void recibir_paquete_write(int socket_cliente,void** nbytes, int* pid, int* numero_pagina);
void recibir_paquete_read(int socket_cliente,int* pid, int* numero_pagina);
int recibir_paquete_delete(int socket_cliente);
int recibir_entero(void* buffer);
void* recibir_buffer(int* size, int socket_cliente);
void recibir_mensaje(int socket_cliente);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void crear_buffer(t_paquete* paquete);
void* serializar_paquete(t_paquete* paquete, int bytes);
void enviar_respuesta_read(int socket_cliente, void* respuesta);
void enviar_respuesta_write(int socket_cliente,int estado);
char* recibir_paquete_asignacion(int socket_cliente);
char* recibir_cadena(void** buffer);
void* recibir_cadena_de_bytes(void* buffer, int* tamanio_cadena);
void* recibir_nbytes(void* buffer);
void agregar_entero_a_paquete(t_paquete* paquete, int numero);
int asignacion;

#endif /* CONEXIONES_H_ */




