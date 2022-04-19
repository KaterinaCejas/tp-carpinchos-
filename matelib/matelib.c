#include "matelib.h"





//--------------Funciones de conexiones con el backend-------------------/
int crearConexion(char *ip, int puerto);
void enviarMensaje(int socket, t_enviadoPor cliente, void* mensaje, int tamanioMensaje, t_tipoMensaje tipoMensaje);
void enviarPaquete(int socket, t_paquete* paquete);
int recibirMensaje(int socket, t_enviadoPor servidor, t_paquete * paquete);
int recibirDatos(void* paquete, int socket, uint32_t cantARecibir);

int mate_init(mate_instance *lib_ref, char* configPath){
	t_config* archivoConfig = config_create(configPath);
	t_configCarpincho configCarpincho;
	configCarpincho.ipBackend = string_new();
	configCarpincho.ipBackend = config_get_string_value(archivoConfig, "IP_BACKEND");
	configCarpincho.puertoBackend = config_get_int_value(archivoConfig, "PUERTO_BACKEND");
	config_destroy(archivoConfig);
    char*puertoString = string_new();
	string_append(&puertoString ,string_itoa(configCarpincho.puertoBackend));
	printf("puerto con string es: %s", puertoString);

	int conexion = crearConexion(configCarpincho.ipBackend, configCarpincho.puertoBackend);
	printf("pude contarme al kernel");
	printf("El socket de conexion de carpincho con kernel vale %d", conexion);

	enviarMensaje(conexion, CARPINCHO, NULL, 0, MATE_INIT);
	t_paquete paquete;
	recibirMensaje(conexion, SERVIDOR_POLIMORFICO, &paquete);

	/*while (recibirMensaje(conexion, SERVIDOR_POLIMORFICO, &paquete) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que el servidor le diga que continue.
	}*/

	lib_ref->group_info = malloc(sizeof(t_groupInfo));
	t_groupInfo* info = malloc(sizeof(t_groupInfo));

	memcpy(&(info->idCarpincho), paquete.mensaje, sizeof(int));

	//info->idCarpincho = atoi(paquete.mensaje);
	info->socket = conexion;

	lib_ref->group_info = (void*)info;

	return 0;
}

int mate_close(mate_instance *lib_ref){
	t_groupInfo* info = malloc(sizeof(t_groupInfo));
	info = (t_groupInfo*)lib_ref->group_info;

	int tamanioMensaje = strlen(string_itoa(info->idCarpincho)) + 1;
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(info->idCarpincho));

	enviarMensaje(info->socket, CARPINCHO, mensaje, tamanioMensaje, MATE_CLOSE);

	t_paquete paquete;
	while (recibirMensaje(info->socket, SERVIDOR_POLIMORFICO, &paquete) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que el servidor le diga que continue.
	}
	close(info->socket);
	return 0;
}

int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value) {
	t_groupInfo* info = malloc(sizeof(t_groupInfo));
	info = (t_groupInfo*)lib_ref->group_info;

	int tamanioMensaje = strlen(string_itoa(info->idCarpincho)) + strlen(sem) + 1 + 1 + strlen(string_itoa(value));
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(info->idCarpincho));
	string_append(&mensaje, ";");
	string_append(&mensaje, sem);
	string_append(&mensaje, ";");
	string_append(&mensaje, string_itoa(value));

	enviarMensaje(info->socket, CARPINCHO, mensaje, tamanioMensaje, MATE_SEM_INIT);

	t_paquete paquete;
	while (recibirMensaje(info->socket, SERVIDOR_POLIMORFICO, &paquete) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que el servidor le diga que continue.
	}

	return atoi(paquete.mensaje);
}

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem) {
	t_groupInfo* info = malloc(sizeof(t_groupInfo));
	info = (t_groupInfo*)lib_ref->group_info;

	int tamanioMensaje = strlen(string_itoa(info->idCarpincho)) + strlen(sem) + 1 + 1;
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(info->idCarpincho));
	string_append(&mensaje, ";");
	string_append(&mensaje, sem);

	enviarMensaje(info->socket, CARPINCHO, mensaje, tamanioMensaje, MATE_SEM_WAIT);

	t_paquete paquete;
	while (recibirMensaje(info->socket, SERVIDOR_POLIMORFICO, &paquete) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que el servidor le diga que continue.
	}

	return atoi(paquete.mensaje);
}

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem) {
	t_groupInfo* info = malloc(sizeof(t_groupInfo));
	info = (t_groupInfo*)lib_ref->group_info;

	int tamanioMensaje = strlen(string_itoa(info->idCarpincho)) + strlen(sem) + 1 + 1;
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(info->idCarpincho));
	string_append(&mensaje, ";");
	string_append(&mensaje, sem);

	enviarMensaje(info->socket, CARPINCHO, mensaje, tamanioMensaje, MATE_SEM_POST);

	t_paquete paquete;
	while (recibirMensaje(info->socket, SERVIDOR_POLIMORFICO, &paquete) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que el servidor le diga que continue.
	}

	return atoi(paquete.mensaje);
}

int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem) {
	t_groupInfo* info = malloc(sizeof(t_groupInfo));
	info = (t_groupInfo*)lib_ref->group_info;

	int tamanioMensaje = strlen(string_itoa(info->idCarpincho)) + strlen(sem) + 1 + 1;
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(info->idCarpincho));
	string_append(&mensaje, ";");
	string_append(&mensaje, sem);

	enviarMensaje(info->socket, CARPINCHO, mensaje, tamanioMensaje, MATE_SEM_DESTROY);

	t_paquete paquete;
	while (recibirMensaje(info->socket, SERVIDOR_POLIMORFICO, &paquete) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que el servidor le diga que continue.
	}

	return atoi(paquete.mensaje);
}


int mate_call_io(mate_instance *lib_ref, mate_io_resource io, void *msg){
	t_groupInfo* info = malloc(sizeof(t_groupInfo));
	info = (t_groupInfo*)lib_ref->group_info;

	int tamanioMensaje = strlen(string_itoa(info->idCarpincho)) + strlen(io) + 1 + 1 + strlen((char*)msg);
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(info->idCarpincho));
	string_append(&mensaje, ";");
	string_append(&mensaje, io);
	string_append(&mensaje, ";");
	string_append(&mensaje, (char*)msg);

	enviarMensaje(info->socket, CARPINCHO, mensaje, tamanioMensaje, MATE_CALL_IO);

	t_paquete paquete;
	while (recibirMensaje(info->socket, SERVIDOR_POLIMORFICO, &paquete) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que el servidor le diga que continue.
	}

	return atoi(paquete.mensaje);
}


mate_pointer mate_memalloc(mate_instance *lib_ref, int size){
	t_groupInfo* info = malloc(sizeof(t_groupInfo));
	info = (t_groupInfo*)lib_ref->group_info;

	//						tamaño del id del carpincho           tamaño del size a alojar  ";"  "/0"
	int tamanioMensaje = strlen(string_itoa(info->idCarpincho)) + strlen(string_itoa(size)) + 1 + 1;
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(info->idCarpincho));
	string_append(&mensaje, ";");
	string_append(&mensaje, string_itoa(size));

	enviarMensaje(info->socket, CARPINCHO, mensaje, tamanioMensaje, MATE_MEMALLOC);

	t_paquete paquete;
	while (recibirMensaje(info->socket, SERVIDOR_POLIMORFICO, &paquete) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que el servidor le diga que continue.
	}
	if((mate_pointer)paquete.mensaje == 0){
		return 0;
	}

	return (mate_pointer)paquete.mensaje;
}


int mate_memfree(mate_instance *lib_ref, mate_pointer addr){
	t_groupInfo* info = malloc(sizeof(t_groupInfo));
	info = (t_groupInfo*)lib_ref->group_info;

	//						tamaño del id del carpincho           tamaño de la direccion     ";" "/0"
	int tamanioMensaje = strlen(string_itoa(info->idCarpincho)) + strlen(string_itoa(addr)) + 1 + 1;
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(info->idCarpincho));
	string_append(&mensaje, ";");
	string_append(&mensaje, string_itoa(addr));

	enviarMensaje(info->socket, CARPINCHO, mensaje, tamanioMensaje, MATE_MEMFREE);

	t_paquete paquete;
	while (recibirMensaje(info->socket, SERVIDOR_POLIMORFICO, &paquete) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que el servidor le diga que continue.
	}

	if(((enum mate_errors)paquete.mensaje) == MATE_FREE_FAULT){
		return -5;
	}
	else {
		return (int)paquete.mensaje;
	}
}


//					instancia LIB	   ptr return del memalloc  destino tamaño a leer
int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size){
	t_groupInfo* info = malloc(sizeof(t_groupInfo));
	info = (t_groupInfo*)lib_ref->group_info;

	//						tamaño del id del carpincho                 tamaño del ptr         ";"  "/0"     destino                 tamaño a leer
	int tamanioMensaje = strlen(string_itoa(info->idCarpincho)) + strlen(string_itoa(origin)) + 3 + 1 + strlen((char*)dest) + strlen(string_itoa(size));
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(info->idCarpincho));
	string_append(&mensaje, ";");
	string_append(&mensaje, string_itoa(origin));
	string_append(&mensaje, ";");
	string_append(&mensaje, (char*)dest);
	string_append(&mensaje, ";");
	string_append(&mensaje, string_itoa(size));

	enviarMensaje(info->socket, CARPINCHO, mensaje, tamanioMensaje, MATE_MEMREAD);

	t_paquete paquete;
	while (recibirMensaje(info->socket, SERVIDOR_POLIMORFICO, &paquete) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que el servidor le diga que continue.
	}

	if(((enum mate_errors)paquete.mensaje) == MATE_READ_FAULT){
		return -6;
	}
	else {
		return (int)paquete.mensaje;
	}
}


//					instancia LIB	        origen  ptr return del memalloc tamaño a escribir
int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size){
	t_groupInfo* info = malloc(sizeof(t_groupInfo));
	info = (t_groupInfo*)lib_ref->group_info;

	//						tamaño del id del carpincho                 origen           ";" "/0"       destino                   tamaño a leer
	int tamanioMensaje = strlen(string_itoa(info->idCarpincho)) + strlen((char*)origin) + 3 + 1 + strlen(string_itoa(dest)) + strlen(string_itoa(size));
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(info->idCarpincho));
	string_append(&mensaje, ";");
	string_append(&mensaje, (char*)origin);
	string_append(&mensaje, ";");
	string_append(&mensaje, string_itoa(dest));
	string_append(&mensaje, ";");
	string_append(&mensaje, string_itoa(size));

	enviarMensaje(info->socket, CARPINCHO, mensaje, tamanioMensaje, MATE_MEMWRITE);

	t_paquete paquete;
	while (recibirMensaje(info->socket, SERVIDOR_POLIMORFICO, &paquete) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que el servidor le diga que continue.
	}

	if(((enum mate_errors)paquete.mensaje) == MATE_WRITE_FAULT){
		return -7;
	}
	else {
		return (int)paquete.mensaje;
	}
}

int crearConexion(char* ip, int puerto) {
	struct addrinfo hints;
	struct addrinfo *server_info;
	struct addrinfo *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, string_itoa(puerto), &hints, &server_info);

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



void enviarMensaje(int socket, t_enviadoPor cliente, void* mensaje, int tamanioMensaje, t_tipoMensaje tipoMensaje){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->header.tipoMensaje = tipoMensaje;
	paquete->header.cliente = cliente;
	uint32_t r = 0;
	if(tamanioMensaje<=0 || mensaje==NULL){
		paquete->header.tamanioMensaje = sizeof(uint32_t);
		paquete->mensaje = &r;
	} else {
		paquete->header.tamanioMensaje = tamanioMensaje;
		paquete->mensaje = mensaje;
	}
	enviarPaquete(socket, paquete);
	free(paquete);
}

void enviarPaquete(int socket, t_paquete* paquete) {
	int cantAEnviar = sizeof(t_infoMensaje) + paquete->header.tamanioMensaje;
	void* datos = malloc(cantAEnviar);
	memcpy(datos, &(paquete->header), sizeof(t_infoMensaje));

	if (paquete->header.tamanioMensaje > 0){
		memcpy(datos + sizeof(t_infoMensaje), (paquete->mensaje), paquete->header.tamanioMensaje);
	}

	int enviado = 0; //bytes enviados
	int totalEnviado = 0;

	do {
		enviado = send(socket, datos + totalEnviado, cantAEnviar - totalEnviado, 0);
		totalEnviado += enviado;
		if(enviado==-1){
			break;
		}
	} while (totalEnviado != cantAEnviar);

	free(datos);
}

int recibirMensaje(int socket, t_enviadoPor servidor, t_paquete * paquete) {
	paquete->mensaje = NULL;
	int resul = recibirDatos(&(paquete->header), socket, sizeof(t_infoMensaje));

	if (resul > 0 && paquete->header.tamanioMensaje > 0) {
		paquete->mensaje = malloc(paquete->header.tamanioMensaje);
		resul = recibirDatos(paquete->mensaje, socket, paquete->header.tamanioMensaje);
	}
	return resul;
}

int recibirDatos(void* paquete, int socket, uint32_t cantARecibir) {
	void* datos = malloc(cantARecibir);
	int recibido = 0;
	int totalRecibido = 0;

	do {
		recibido = recv(socket, datos + totalRecibido, cantARecibir - totalRecibido, 0);
		totalRecibido += recibido;
	} while (totalRecibido != cantARecibir && recibido > 0);

	//if (recibido == -1)
	//	return recibido;

	memcpy(paquete, datos, cantARecibir);
	free(datos);
	return recibido;
}

