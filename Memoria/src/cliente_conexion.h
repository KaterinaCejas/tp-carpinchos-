#ifndef CLIENTE_CONEXION_H_
#define CLIENTE_CONEXION_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
//#include<commons/log.h>

typedef enum
{
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
} t_paquete_swamp;


int crear_conexion(char* ip, char* puerto);
t_paquete_swamp* crear_paquete(int codigo);
void agregar_a_paquete(t_paquete_swamp* paquete, void* valor, int tamanio);
void agregar_cadena_a_paquete(t_paquete_swamp* paquete, char* palabra);
void agregar_entero_a_paquete(t_paquete_swamp* paquete, int numero);
void enviar_paquete(t_paquete_swamp* paquete, int socket_cliente);
void eliminar_paquete(t_paquete_swamp* paquete);
void liberar_conexion(int socket_cliente);
void agregar_n_bytes_a_paquete(t_paquete_swamp* paquete, void* bytes, int tamanio);


#endif /* CLIENTE_CONEXION_H_ */
