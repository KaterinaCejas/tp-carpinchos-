#include "cliente_conexion.h"



void* serializar_paquete(t_paquete_swamp* paquete, int bytes) {
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

int crear_conexion(char *ip, char* puerto) {
	struct addrinfo hints;
	struct addrinfo *server_info;
	struct addrinfo *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(ip, puerto, &hints, &server_info) == -1){
		printf("GETADDRINFO Rompio.");
	}

	// Ahora vamos a crear el socket.
	int socket_cliente = 0;

	//prueba
	for(p=server_info; p!=NULL; p=p->ai_next){
		if((socket_cliente=socket(p->ai_family, p->ai_socktype, p->ai_protocol))==-1){
			continue;
		}
		if(connect(socket_cliente, p->ai_addr, p->ai_addrlen) == -1){
			printf("no se pudo conectar el cliente al servidor\n");
			continue;
		}
		break;
	}

	if(p == NULL){
		printf("no se pudo conectar p == NULL\n");
		freeaddrinfo(server_info);
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(server_info);
	return socket_cliente;
}



t_buffer* crear_buffer() {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = 0;
	buffer->stream = NULL;

	return buffer;
}

t_paquete_swamp* crear_paquete(int codigo) {
	t_paquete_swamp* paquete = malloc(sizeof(t_paquete_swamp));
	paquete->codigo_operacion = codigo;
	paquete->buffer = crear_buffer();
	return paquete;
}

void agregar_a_paquete(t_paquete_swamp* paquete, void* valor, int tamanio) {
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}
void agregar_cadena_a_paquete(t_paquete_swamp* paquete, char* palabra)
{
	agregar_a_paquete(paquete, (void*)palabra, strlen(palabra)+1);
}
void agregar_entero_a_paquete(t_paquete_swamp* paquete, int numero)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &numero, sizeof(int));

	paquete->buffer->size += sizeof(int);
}

void agregar_n_bytes_a_paquete(t_paquete_swamp* paquete, void* bytes, int tamanio){
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);
	memcpy(paquete->buffer->stream + paquete->buffer->size, bytes, tamanio);
	paquete->buffer->size += tamanio;
}

void enviar_paquete(t_paquete_swamp* paquete, int socket_cliente) {
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete_swamp* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}
