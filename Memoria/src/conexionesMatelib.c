#include "Memoria.h"


void iniciarServidorKernel(void){
	pthread_t hiloServidorKernel;
	if(!pthread_create(&hiloServidorKernel, NULL, (void*) iniciarConexionPorCarpinchoConKernel, NULL)){
		pthread_detach(hiloServidorKernel);
	}
	else {
		log_error(LOGGER, "No se pudo crear el hilo servidor kernel");
	}
}

void iniciarServidorMateLib(void){
	pthread_t hiloServidorMateLib;
	if(!pthread_create(&hiloServidorMateLib, NULL, (void*) iniciarConexionSoloConMateLib, NULL)){
		pthread_detach(hiloServidorMateLib);
	}
	else {
		log_error(LOGGER, "No se pudo crear el hilo servidor matelib");
	}
}

int iniciarConexionPorCarpinchoConKernel(void){
	int socketServidor = 0;
	int valor = 0;

    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, string_itoa(CONFIG_MEMORIA->PUERTO_ESCUCHA), &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next){
    	socketServidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		 if(socketServidor == -1){
			 return EXIT_FAILURE;
		 }
		 valor = bind(socketServidor, p->ai_addr, p->ai_addrlen);
		 if(valor == -1){
			 return EXIT_FAILURE;
		 }
		 break;
    }

    valor = listen(socketServidor, SOMAXCONN);
	if(valor == -1){
		return EXIT_FAILURE;
	}

	freeaddrinfo(servinfo);
	log_info(LOGGER, "el socket servidor vale: %d", socketServidor);
	pthread_t conexionPorCarpincho;
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);
	int socketKernel;
	while(1){
		socketKernel = 0;
		socketKernel = accept(socketServidor, (void*)&dir_cliente, &tam_direccion);
		if(socketKernel == -1) {
			log_info(LOGGER, "aun no recibi cliente");
			continue;
		}
		else {
			pthread_create(&conexionPorCarpincho, NULL, (void*)conexionConCarpincho, (void*)socketKernel);
			pthread_detach(conexionPorCarpincho);
		}
	}
}


void conexionConCarpincho(void* voidSocketKernel){
	int socketKernel = (int)voidSocketKernel;
	t_paquete paquete;
	int idCarpincho;
	int tamanioRespuesta;
	char* respuesta;
	mate_pointer direccionLogica;
	bool terminoCarpincho;
	int returnDeMemoria;
	char** mensajeArray = string_array_new();

		while(1){
			recibirMensaje(socketKernel, CARPINCHO, &paquete);
			switch(paquete.header.tipoMensaje){
				case MATE_INIT:
					idCarpincho = (int)paquete.mensaje;
					returnDeMemoria = start_process(idCarpincho);
					enviarMensaje(socketKernel, MEMORIA, NULL, 0, MATE_INIT);
					break;

				case MATE_CLOSE:
					idCarpincho = (int)paquete.mensaje;
					close_process(idCarpincho);
					enviarMensaje(socketKernel, MEMORIA, NULL, 0, MATE_CLOSE);
					terminoCarpincho = true;
					break;

				case MATE_MEMALLOC:
					mensajeArray = string_split(paquete.mensaje, ";");
					direccionLogica = reserve_memory(atoi(mensajeArray[0]), atoi(mensajeArray[1]));
					tamanioRespuesta = strlen(string_itoa(direccionLogica)) + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, string_itoa(direccionLogica));
					enviarMensaje(socketKernel, MEMORIA, respuesta, tamanioRespuesta, MATE_MEMALLOC);
					break;

				case MATE_MEMFREE:
					mensajeArray = string_split(paquete.mensaje, ";");
					returnDeMemoria = free_memory(atoi(mensajeArray[0]), atoi(mensajeArray[1]));
					tamanioRespuesta = strlen(string_itoa(returnDeMemoria)) + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, string_itoa(returnDeMemoria));
					enviarMensaje(socketKernel, MEMORIA, respuesta, tamanioRespuesta, MATE_MEMFREE);
					break;

				case MATE_MEMREAD:
					mensajeArray = string_split(paquete.mensaje, ";");
					returnDeMemoria = read_memory(atoi(mensajeArray[0]), atoi(mensajeArray[1]), (void*)mensajeArray[2], atoi(mensajeArray[3]));
					tamanioRespuesta = strlen(string_itoa(returnDeMemoria)) + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, string_itoa(returnDeMemoria));
					enviarMensaje(socketKernel, MEMORIA, respuesta, tamanioRespuesta, MATE_MEMREAD);
					break;

				case MATE_MEMWRITE:
					mensajeArray = string_split(paquete.mensaje, ";");
					returnDeMemoria = write_memory(atoi(mensajeArray[0]), (void*)mensajeArray[1], atoi(mensajeArray[2]), atoi(mensajeArray[3]));
					tamanioRespuesta = strlen(string_itoa(returnDeMemoria)) + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, string_itoa(returnDeMemoria));
					enviarMensaje(socketKernel, MEMORIA, respuesta, tamanioRespuesta, MATE_MEMWRITE);
					break;
			}


			if(terminoCarpincho == true){
				close(socketKernel);
				break;
			}
			else{
				continue;
			}
		}
}


/*
 *
 * 			EN CASO DE QUE EL KERNEL NO ESTE CONECTADO
 *
 */


int iniciarConexionSoloConMateLib(void){
	int socketServidor = 0;
	int valor = 0;

    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(CONFIG_MEMORIA->IP_MEMORIA, string_itoa(CONFIG_MEMORIA->PUERTO_ESCUCHA), &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next){
    	socketServidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		 if(socketServidor == -1){
			 return EXIT_FAILURE;
		 }
		 valor = bind(socketServidor, p->ai_addr, p->ai_addrlen);
		 if(valor == -1){
			 return EXIT_FAILURE;
		 }
		 break;
    }

    valor = listen(socketServidor, SOMAXCONN);
	if(valor == -1){
		return EXIT_FAILURE;
	}

	freeaddrinfo(servinfo);

	pthread_t conexionConMateLib;
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);
	int socketMateLib;
	while(1){
		socketMateLib = 0;
		socketMateLib = accept(socketServidor, (void*)&dir_cliente, &tam_direccion);
		if(socketMateLib == -1) {
			continue;
		}
		else {
			pthread_create(&conexionConMateLib, NULL, (void*)conexionConMateLib, (void*)socketMateLib);
			pthread_detach(conexionConMateLib);
		}
	}
}


void conexionConMateLib(void* voidSocketMateLib){
	int socketMateLib = (int)voidSocketMateLib;
	t_paquete paquete;
	int idCarpincho;
	int tamanioRespuesta;
	char* respuesta;
	mate_pointer direccionLogica;
	bool terminoCarpincho;
	int returnDeMemoria;
	char** mensajeArray = string_array_new();

		while(1){
			recibirMensaje(socketMateLib, CARPINCHO, &paquete);
			switch(paquete.header.tipoMensaje){
				case MATE_INIT:
					idCarpincho = (int)paquete.mensaje;
					returnDeMemoria = start_process(idCarpincho);
					enviarMensaje(socketMateLib, MEMORIA, NULL, 0, MATE_INIT);
					break;

				case MATE_CLOSE:
					idCarpincho = (int)paquete.mensaje;
					delete_carpincho(idCarpincho);
					enviarMensaje(socketMateLib, MEMORIA, NULL, 0, MATE_CLOSE);
					terminoCarpincho = true;
					break;

				case MATE_MEMALLOC:
					mensajeArray = string_split(paquete.mensaje, ";");
					direccionLogica = reserve_memory(atoi(mensajeArray[0]), atoi(mensajeArray[1]));
					tamanioRespuesta = strlen(string_itoa(direccionLogica)) + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, string_itoa(direccionLogica));
					enviarMensaje(socketMateLib, MEMORIA, respuesta, tamanioRespuesta, MATE_MEMALLOC);
					break;

				case MATE_MEMFREE:
					mensajeArray = string_split(paquete.mensaje, ";");
					returnDeMemoria = free_memory(atoi(mensajeArray[0]), atoi(mensajeArray[1]));
					tamanioRespuesta = strlen(string_itoa(returnDeMemoria)) + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, string_itoa(returnDeMemoria));
					enviarMensaje(socketMateLib, MEMORIA, respuesta, tamanioRespuesta, MATE_MEMFREE);
					break;

				case MATE_MEMREAD:
					mensajeArray = string_split(paquete.mensaje, ";");
					returnDeMemoria = read_memory(atoi(mensajeArray[0]), atoi(mensajeArray[1]), (void*)mensajeArray[2], atoi(mensajeArray[3]));
					tamanioRespuesta = strlen(string_itoa(returnDeMemoria)) + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, string_itoa(returnDeMemoria));
					enviarMensaje(socketMateLib, MEMORIA, respuesta, tamanioRespuesta, MATE_MEMREAD);
					break;

				case MATE_MEMWRITE:
					mensajeArray = string_split(paquete.mensaje, ";");
					returnDeMemoria = write_memory(atoi(mensajeArray[0]), (void*)mensajeArray[1], atoi(mensajeArray[2]), atoi(mensajeArray[3]));
					tamanioRespuesta = strlen(string_itoa(returnDeMemoria)) + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, string_itoa(returnDeMemoria));
					enviarMensaje(socketMateLib, MEMORIA, respuesta, tamanioRespuesta, MATE_MEMWRITE);
					break;

				case MATE_SEM_INIT:
					tamanioRespuesta = strlen("KERNEL DESCONECTADO. IMPOSIBLE HACER MATE_SEM_INIT") + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, "KERNEL DESCONECTADO. IMPOSIBLE HACER MATE_SEM_INIT");
					enviarMensaje(socketMateLib, MEMORIA, respuesta, tamanioRespuesta, MATE_SEM_INIT);
					break;

				case MATE_SEM_WAIT:
					tamanioRespuesta = strlen("KERNEL DESCONECTADO. IMPOSIBLE HACER MATE_SEM_WAIT") + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, "KERNEL DESCONECTADO. IMPOSIBLE HACER MATE_SEM_WAIT");
					enviarMensaje(socketMateLib, MEMORIA, respuesta, tamanioRespuesta, MATE_SEM_WAIT);
					break;

				case MATE_SEM_POST:
					tamanioRespuesta = strlen("KERNEL DESCONECTADO. IMPOSIBLE HACER MATE_SEM_POST") + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, "KERNEL DESCONECTADO. IMPOSIBLE HACER MATE_SEM_POST");
					enviarMensaje(socketMateLib, MEMORIA, respuesta, tamanioRespuesta, MATE_SEM_POST);
					break;

				case MATE_SEM_DESTROY:
					tamanioRespuesta = strlen("KERNEL DESCONECTADO. IMPOSIBLE HACER MATE_SEM_DESTROY") + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, "KERNEL DESCONECTADO. IMPOSIBLE HACER MATE_SEM_DESTROY");
					enviarMensaje(socketMateLib, MEMORIA, respuesta, tamanioRespuesta, MATE_SEM_DESTROY);
					break;
				case MATE_CALL_IO:
					tamanioRespuesta = strlen("KERNEL DESCONECTADO. IMPOSIBLE HACER MATE_CALL_IO") + 1;
					respuesta = malloc(tamanioRespuesta);
					string_append(&respuesta, "KERNEL DESCONECTADO. IMPOSIBLE HACER MATE_CALL_IO");
					enviarMensaje(socketMateLib, MEMORIA, respuesta, tamanioRespuesta, MATE_CALL_IO);
					break;
			}

			free(respuesta);

			if(terminoCarpincho == true){
				close(socketMateLib);
				break;
			}
			else{
				continue;
			}
		}
}












