#include "Kernel.h"

void iniciarConexionCarpinchoMemoria(void){
	int conexionCarpincho = iniciarServidor();
	if(conexionCarpincho != -1){
		pthread_t conexionPorCarpincho;
		int socketCarpincho;
		struct sockaddr_in dir_cliente;
		socklen_t tam_direccion = sizeof(struct sockaddr_in);
		int socketMemoria;
		t_conexion* conexion;
		conexion = malloc(sizeof(t_conexion));

		while(1){
			socketCarpincho = 0;
			socketCarpincho = accept(conexionCarpincho, (void*)&dir_cliente, &tam_direccion);
			if(socketCarpincho == -1) {
				continue;
			}
			else {
				socketMemoria = iniciarCliente();
				if(socketMemoria != -1){
					conexion->socketCarpincho = socketCarpincho;
					conexion->socketMemoria = socketMemoria;
					pthread_create(&conexionPorCarpincho, NULL, (void*) conexionConCarpincho, (void*)conexion);
				}
			}
		}
	}
}

int iniciarServidor(void) {
	int socketServidor = 0;
	int valor = 0;

    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, string_itoa(configKernel.puertoEscucha), &hints, &servinfo);

    p=servinfo;
    while(p != NULL){
    	socketServidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(socketServidor == -1){
			p = p->ai_next;
			continue;
		}
		valor = bind(socketServidor, p->ai_addr, p->ai_addrlen);
		if(valor == -1){
			close(socketServidor);
			p = p->ai_next;
			continue;
		}
		if(valor != -1){
			break;
		}
    }

    valor = listen(socketServidor, SOMAXCONN);
	if(valor == -1){
		pthread_mutex_lock(&mutexLoggerKernel);
		log_error(loggerKernel, "El servidor no pudo escuchar");
		pthread_mutex_unlock(&mutexLoggerKernel);
		return EXIT_FAILURE;
	}

	freeaddrinfo(servinfo);
    log_debug(loggerKernel, "Conexion exitosa. El valor del socket es %d", valor);
	return socketServidor;
}

int iniciarCliente(void) {
    struct addrinfo hints;
    struct addrinfo *server_info;
    int clientSocket;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if(getaddrinfo(configKernel.ipMemoria, string_itoa(configKernel.puertoMemoria), &hints, &server_info)){
            perror("ERROR GET ADDRINFO: ");
        exit(-1);
    }
    clientSocket = socket(server_info->ai_family, server_info->ai_socktype, 0);
    if(connect(clientSocket, server_info->ai_addr, server_info->ai_addrlen) == -1){
            perror("ERROR CONNECT: ");
        exit(-1);
    }
    freeaddrinfo(server_info);
    return clientSocket;
}


void conexionConCarpincho(void* conexionVoid){
	t_conexion* conexion = malloc(sizeof(t_conexion));
	conexion = (t_conexion*)conexionVoid;

	t_paquete paquete;
	int idCarpincho;
	int tamanioRespuesta;
	char* respuesta;
	mate_pointer direccionLogica;
	bool terminoCarpincho;
	int returnDeMemoria;
	int returnDeSemaforos;

	while(1){
		recibirMensaje(conexion->socketCarpincho, CARPINCHO, &paquete);
		switch(paquete.header.tipoMensaje){
			case MATE_INIT:
				idCarpincho = mate_init_kernel(&paquete, conexion->socketMemoria);
				tamanioRespuesta = strlen(string_itoa(idCarpincho)) + 1;
				respuesta = malloc(tamanioRespuesta);
				string_append(&respuesta, string_itoa(idCarpincho));
				enviarMensaje(conexion->socketCarpincho, KERNEL, respuesta, tamanioRespuesta, MATE_INIT);
				break;

			case MATE_CLOSE:
				mate_close_kernel(&paquete, conexion->socketMemoria);
				enviarMensaje(conexion->socketCarpincho, KERNEL, NULL, 0, MATE_CLOSE);
				terminoCarpincho = true;
				break;

			case MATE_SEM_INIT:
				returnDeSemaforos = mate_sem_init_kernel(&paquete, conexion->socketMemoria);
				tamanioRespuesta = strlen(string_itoa(returnDeSemaforos)) + 1;
				respuesta = malloc(tamanioRespuesta);
				string_append(&respuesta, string_itoa(returnDeSemaforos));
				enviarMensaje(conexion->socketCarpincho, KERNEL, respuesta, tamanioRespuesta, MATE_SEM_INIT);
				break;

			case MATE_SEM_WAIT:
				returnDeSemaforos = mate_sem_wait_kernel(&paquete);
				tamanioRespuesta = strlen(string_itoa(returnDeSemaforos)) + 1;
				respuesta = malloc(tamanioRespuesta);
				string_append(&respuesta, string_itoa(returnDeSemaforos));
				enviarMensaje(conexion->socketCarpincho, KERNEL, respuesta, tamanioRespuesta, MATE_SEM_WAIT);
				break;

			case MATE_SEM_POST:
				returnDeSemaforos = mate_sem_post_kernel(&paquete);
				tamanioRespuesta = strlen(string_itoa(returnDeSemaforos)) + 1;
				respuesta = malloc(tamanioRespuesta);
				string_append(&respuesta, string_itoa(returnDeSemaforos));
				enviarMensaje(conexion->socketCarpincho, KERNEL, respuesta, tamanioRespuesta, MATE_SEM_POST);
				break;

			case MATE_SEM_DESTROY:
				returnDeSemaforos = mate_sem_destroy_kernel(&paquete);
				tamanioRespuesta = strlen(string_itoa(returnDeSemaforos)) + 1;
				respuesta = malloc(tamanioRespuesta);
				string_append(&respuesta, string_itoa(returnDeSemaforos));
				enviarMensaje(conexion->socketCarpincho, KERNEL, respuesta, tamanioRespuesta, MATE_SEM_DESTROY);
				break;

			case MATE_MEMALLOC:
				direccionLogica = mate_memalloc_kernel(&paquete, conexion->socketMemoria);
				tamanioRespuesta = strlen(string_itoa(direccionLogica)) + 1;
				respuesta = malloc(tamanioRespuesta);
				string_append(&respuesta, string_itoa(direccionLogica));
				enviarMensaje(conexion->socketCarpincho, KERNEL, respuesta, tamanioRespuesta, MATE_MEMALLOC);
				break;

			case MATE_MEMFREE:
				returnDeMemoria = mate_memfree_kernel(&paquete, conexion->socketMemoria);
				tamanioRespuesta = strlen(string_itoa(returnDeMemoria)) + 1;
				respuesta = malloc(tamanioRespuesta);
				string_append(&respuesta, string_itoa(returnDeMemoria));
				enviarMensaje(conexion->socketCarpincho, KERNEL, respuesta, tamanioRespuesta, MATE_MEMFREE);
				break;

			case MATE_MEMREAD:
				returnDeMemoria = mate_memread_kernel(&paquete, conexion->socketMemoria);
				tamanioRespuesta = strlen(string_itoa(returnDeMemoria)) + 1;
				respuesta = malloc(tamanioRespuesta);
				string_append(&respuesta, string_itoa(returnDeMemoria));
				enviarMensaje(conexion->socketCarpincho, KERNEL, respuesta, tamanioRespuesta, MATE_MEMREAD);
				break;

			case MATE_MEMWRITE:
				returnDeMemoria = mate_memwrite_kernel(&paquete, conexion->socketMemoria);
				tamanioRespuesta = strlen(string_itoa(returnDeMemoria)) + 1;
				respuesta = malloc(tamanioRespuesta);
				string_append(&respuesta, string_itoa(returnDeMemoria));
				enviarMensaje(conexion->socketCarpincho, KERNEL, respuesta, tamanioRespuesta, MATE_MEMWRITE);
				break;

			case MATE_CALL_IO:
				returnDeSemaforos = mate_call_io_kernel(&paquete);
				tamanioRespuesta = strlen(string_itoa(returnDeSemaforos)) + 1;
				respuesta = malloc(tamanioRespuesta);
				string_append(&respuesta, string_itoa(returnDeSemaforos));
				enviarMensaje(conexion->socketCarpincho, KERNEL, respuesta, tamanioRespuesta, MATE_CALL_IO);
				break;
			case SUSPENSION:
				break;
			case DES_SUSPENSION:
				break;
		}

		free(respuesta);

		if(terminoCarpincho == true){
			close(conexion->socketCarpincho);
			close(conexion->socketMemoria);
			break;
		}
		else{
			continue;
		}
	}
	free(conexion);
}


