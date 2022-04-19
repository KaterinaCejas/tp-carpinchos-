#include "Kernel.h"

//funciones de la lib sobre el inicio y cierre de carpinchos
int mate_init_kernel(t_paquete* paquete, int socketMemoria){
	pthread_mutex_lock(&mutexLoggerKernel);
	log_debug(loggerKernel, "Inicio mate_init de un nuevo carpincho");
	pthread_mutex_unlock(&mutexLoggerKernel);
	t_pcbCarpincho* newCarpincho = malloc(sizeof(t_pcbCarpincho));
	newCarpincho->pedidosIO = list_create();
	newCarpincho->pedidosSEM = list_create();
	newCarpincho->estado = NEW;
	newCarpincho->pid = generarPID();
	newCarpincho-> estimacionAnterior = configKernel.estimacionInicial;
	newCarpincho-> rafagaRealAnterior = 0;
	newCarpincho-> estimacionActual = 0;
	newCarpincho-> tiempoInicioExec = 0;
	newCarpincho-> tiempoSalidaExec = 0;
	newCarpincho-> tiempoDeEspera = 0;
	newCarpincho-> tiempoInicioReady = 0;
	newCarpincho-> tiempoSalidaReady = 0;
	newCarpincho->socket = paquete->header.cliente;
	newCarpincho->socketMemoria = socketMemoria;
	log_debug(loggerKernel, "cargue un carpincho con algunos datos. PID = %d", newCarpincho->pid);
	pthread_mutex_lock(&mutexNew);
	list_add(listaNew, newCarpincho);
	sem_post(&ordenLargoPlazo);
	pthread_mutex_unlock(&mutexNew);

	pthread_mutex_lock(&mutexTodosLosCarpinchos);
	list_add(listaGeneralTodosCarpinchos, newCarpincho);
	pthread_mutex_unlock(&mutexTodosLosCarpinchos);

	int tamanioMensaje = strlen(string_itoa(newCarpincho->pid)) + 1;
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(newCarpincho->pid));

	enviarMensaje(socketMemoria, KERNEL, mensaje, tamanioMensaje, MATE_INIT);
	log_debug(loggerKernel, "le envie un mensaje a la memoria de init carpincho PID= %d", newCarpincho->pid);
	t_paquete paqueteNew;
	while (recibirMensaje(socketMemoria, MEMORIA, &paqueteNew) <= 0) {
		log_debug(loggerKernel, "estoy dentro del while de recibir mensaje de la memoria");
		continue;
		//no hace nada. Tiene que esperar a que la memoria le diga que termino el mate_init
	}

	while(newCarpincho->estado != EXEC){            //revisar
		continue;
	}

	pthread_mutex_lock(&mutexLoggerKernel);
	log_debug(loggerKernel, "El carpincho %d esta listo para ejecutar", newCarpincho->pid);
	pthread_mutex_unlock(&mutexLoggerKernel);
	return newCarpincho->pid;
}

int generarPID(void){
	char* stringTemporal = string_new();
	stringTemporal = temporal_get_string_time("%M:%S:%MS");
	char** arrayString =  string_array_new();
	arrayString = string_split(stringTemporal, ":");
	//string_split devuelve NULL en la ultima posicion del array, hay que eliminarlo
	char* stringNull = string_new();
	stringNull =  string_array_pop(arrayString); //elimina y retorna el ultimo elemento del array
	char* stringTodoJunto = string_new();
	for(int i=0; i< string_array_size(arrayString); i++){
		string_append(&stringTodoJunto, arrayString[i]);
	}
	int numeroFinal = atoi(stringTodoJunto);
	string_array_destroy(arrayString);
	free(stringTodoJunto);
	free(stringNull); //lo borro
	free(stringTemporal);
	return numeroFinal;
}

t_pcbCarpincho* encontrarCarpincho(int idCarpincho){
  int i = 0;
  t_pcbCarpincho* carpinchoAux = malloc(sizeof(t_pcbCarpincho));
  carpinchoAux->pedidosIO = list_create();
  carpinchoAux->pedidosSEM = list_create();

  while(i < list_size(listaGeneralTodosCarpinchos)){
    carpinchoAux = list_get(listaGeneralTodosCarpinchos,i);
    if(idCarpincho == carpinchoAux->pid){
    	return carpinchoAux;
    }
    else{i++;}
  }
  return 0;
}

void mate_close_kernel(t_paquete* paquete, int socketMemoria) {
	char** datosDelPaquete = string_array_new();
	datosDelPaquete = string_split((char*)paquete->mensaje, " ");

	int id = atoi(datosDelPaquete[0]);
	pthread_mutex_lock(&mutexLoggerKernel);
	log_debug(loggerKernel, "Iniciando el mate_close del carpincho %d", id);
	pthread_mutex_unlock(&mutexLoggerKernel);

	int i = 0;
	t_pcbCarpincho* carpinchoAux = malloc(sizeof(t_pcbCarpincho));
	carpinchoAux->pedidosIO = list_create();
	carpinchoAux->pedidosSEM = list_create();

	while(i<list_size(listaExec)){
		carpinchoAux = list_get(listaExec,i);
		if(carpinchoAux->pid == id){
			break;
		}
		else{i++;}
	}

	mate_sem_name semAux = string_new();
	int j = 0;
	while(j<list_size(carpinchoAux->pedidosSEM)){
		semAux = list_get(carpinchoAux->pedidosSEM,j);    //esto es para darle un "post" a todos los semaforos que tiene el carpincho pedido
		kernel_sem_post(semAux);
		j++;
	}

	int tamanioMensaje = strlen(string_itoa(id)) + 1;
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(id));
	enviarMensaje(socketMemoria, KERNEL, mensaje, tamanioMensaje, MATE_CLOSE);

	t_paquete paqueteNew;
	while (recibirMensaje(socketMemoria, MEMORIA, &paqueteNew) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que la memoria le diga que termino el mate_close
	}

	close(socketMemoria);

	if(carpinchoAux->estado == EXEC){
		pthread_mutex_lock(&mutexExec);
		pthread_mutex_lock(&mutexExit);
		moverDeLista(carpinchoAux, listaExec, listaExit);
		pthread_mutex_unlock(&mutexExec);
		pthread_mutex_unlock(&mutexExit);
	}
	if(carpinchoAux->estado == BLOCKED){
		pthread_mutex_lock(&mutexBloq);
		pthread_mutex_lock(&mutexExit);
		moverDeLista(carpinchoAux,listaBloq,listaExit);
		pthread_mutex_unlock(&mutexExit);
		pthread_mutex_unlock(&mutexBloq);
	}

	sem_post(&contadorMultiprocesamiento);
	sem_post(&contadorMultiprogramacion);
	pthread_mutex_lock(&mutexLoggerKernel);
	log_debug(loggerKernel, "mate_close del carpincho %d realizado con exito", id);
	pthread_mutex_unlock(&mutexLoggerKernel);
}



//funciones de la lib sobre SEMAFOROS
int mate_sem_init_kernel (t_paquete* paquete, int socketMemoria) {
	//datosDePaquete[] = [int idCarpincho, mate_sem_name sem, int recursos]

	char** datosDelPaquete = string_array_new();
	datosDelPaquete = string_split((char*)paquete->mensaje, ";");

	if(existeSemaforo((mate_sem_name)datosDelPaquete[1]) == true){
		pthread_mutex_lock(&mutexLoggerKernel);
		log_error(loggerKernel, "El semaforo %s ya existe no puede hacerse sem_init", datosDelPaquete[1]);
		pthread_mutex_unlock(&mutexLoggerKernel);
		return -1;
	}
	else {
		pthread_mutex_lock(&mutexLoggerKernel);
		log_debug(loggerKernel, "Realizando sem_init del semaforo %s", datosDelPaquete[1]);
		pthread_mutex_unlock(&mutexLoggerKernel);
		t_SEMAFORO* semaforo = malloc(sizeof(t_SEMAFORO));
		semaforo->nombre = string_new();
		semaforo->nombre = datosDelPaquete[1];
		semaforo->recursosDisponibles = atoi(datosDelPaquete[2]);
		semaforo->carpinchos = list_create();
		list_add(semaforo->carpinchos, (int*)atoi(datosDelPaquete[0]));
		pthread_mutex_lock(&mutexListaSemaforos);
		list_add(list_semaforos, semaforo);
		pthread_mutex_unlock(&mutexListaSemaforos);
		pthread_mutex_lock(&mutexLoggerKernel);
		log_debug(loggerKernel, "sem_init del semaforo %s exitoso", semaforo->nombre);
		pthread_mutex_unlock(&mutexLoggerKernel);
		return 0;
	}
}

bool existeSemaforo(mate_sem_name nombreSem){
	t_SEMAFORO* semaforoAux = malloc(sizeof(t_SEMAFORO));
	semaforoAux->nombre = string_new();
	semaforoAux->carpinchos = list_create();
	for(int i =0; i<list_size(list_semaforos); i++){
		semaforoAux = list_get(list_semaforos, i);
		if(semaforoAux->nombre == nombreSem){
			eliminarSemaforo(semaforoAux);
			return true;
		}
	}
	eliminarSemaforo(semaforoAux);
	return false;
}


int mate_sem_wait_kernel(t_paquete* paquete){
	//datosDelPaquete[] = [int idCarpincho, mate_sem_name SEM]

	char** datosDelPaquete = string_array_new();
	datosDelPaquete = string_split((char*)paquete->mensaje, ";");

	t_pcbCarpincho* carpincho = malloc(sizeof(t_pcbCarpincho));
	carpincho->pedidosIO = list_create();
	carpincho->pedidosSEM = list_create();

	carpincho = encontrarCarpincho(atoi(datosDelPaquete[0]));

	registrarRecursosEnListas(carpincho,(mate_sem_name)datosDelPaquete[0]);

	int valorDeSEM = kernel_sem_getvalue((mate_sem_name)datosDelPaquete[0]);

	if(valorDeSEM > 0){
		kernel_sem_wait((mate_sem_name)datosDelPaquete[0]);                               //KERNEL_SEM_WAIT
		return 0;
	}
	else{
		carpincho->estado = BLOCKED;

		pthread_mutex_lock(&mutexExec);
		pthread_mutex_lock(&mutexBloq);
		moverDeLista(carpincho,listaExec,listaBloq);
		pthread_mutex_unlock(&mutexBloq);
		pthread_mutex_unlock(&mutexExec);

		carpincho->tiempoSalidaExec = temporalConversion();
		sem_post(&contadorMultiprocesamiento);
		sem_post(&contadorMultiprogramacion);

		kernel_sem_wait((mate_sem_name)datosDelPaquete[0]);

		int valorDeSemPostWait = kernel_sem_getvalue((mate_sem_name)datosDelPaquete[0]);
		while(valorDeSemPostWait <= 0){
			continue;
		}

		while(carpincho->estado != EXEC){
			continue;
		}

		return 0;
	}
}

int mate_sem_post_kernel (t_paquete* paquete) {
	//datosDelPaquete = [int idCarpincho ; mate_sem_name sem]

	char** datosDelPaquete = string_array_new();
	datosDelPaquete = string_split((char*)paquete->mensaje, ";");

	kernel_sem_post((mate_sem_name)datosDelPaquete[1]);
	liberarRecursosRegistrados((mate_sem_name)datosDelPaquete[1], atoi(datosDelPaquete[0]));
	desBloquear((mate_sem_name)datosDelPaquete[1]);



	return 0;
}

int mate_sem_destroy_kernel(t_paquete* paquete){
	//datosDelPaquete = [int idCarpincho ; mate_sem_name sem]

	char** datosDelPaquete = string_array_new();
	datosDelPaquete = string_split((char*)paquete->mensaje, ";");

	removerSemDeListaGeneral((mate_sem_name)datosDelPaquete[1]);
	return 0;
}


//funciones de la lib pero con la memoria.
mate_pointer mate_memalloc_kernel(t_paquete* paquete, int socketMemoria){
	enviarPaquete(socketMemoria, paquete);

	t_paquete paqueteNew;
	while (recibirMensaje(socketMemoria, MEMORIA, &paqueteNew) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que la memoria le diga que termino el mate_memalloc
	}

	char** id = string_array_new();
	id = string_split((char*)paquete->mensaje, ";");
	pthread_mutex_lock(&mutexLoggerKernel);
	log_debug(loggerKernel, "Mate memalloc de %s exitoso", id[0]);
	pthread_mutex_unlock(&mutexLoggerKernel);
	string_array_destroy(id);

	return (mate_pointer)paqueteNew.mensaje;
}

int mate_memfree_kernel(t_paquete* paquete, int socketMemoria){
	enviarPaquete(socketMemoria, paquete);

	t_paquete paqueteNew;
	while (recibirMensaje(socketMemoria, MEMORIA, &paqueteNew) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que la memoria le diga que termino el mate_memalloc
	}

	return (int)paqueteNew.mensaje;
}

int mate_memread_kernel(t_paquete* paquete, int socketMemoria){
	enviarPaquete(socketMemoria, paquete);

	t_paquete paqueteNew;
	while (recibirMensaje(socketMemoria, MEMORIA, &paqueteNew) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que la memoria le diga que termino el mate_memalloc
	}

	return (int)paqueteNew.mensaje;
}

int mate_memwrite_kernel(t_paquete* paquete, int socketMemoria){
	enviarPaquete(socketMemoria, paquete);

	t_paquete paqueteNew;
	while (recibirMensaje(socketMemoria, MEMORIA, &paqueteNew) <= 0) {
		continue;
		//no hace nada. Tiene que esperar a que la memoria le diga que termino el mate_memalloc
	}

	return (int)paqueteNew.mensaje;

}

t_dispositivoIO* encontrarDispositivoIO(mate_io_resource nombreIO){
	t_dispositivoIO* dispo = malloc(sizeof(t_dispositivoIO));
	dispo->nombre = string_new();
	for(int i=0; i<list_size(listaDispositivosIO); i++){
		dispo = list_get(listaDispositivosIO, i);
		if(nombreIO == dispo->nombre){
			return dispo;
		}
	}
	return 0;
}

int buscarPosicionIOEnLista(mate_io_resource nombreIO){
	int indice = 0;
	t_dispositivoIO* dispo = malloc(sizeof(t_dispositivoIO));
	dispo->nombre = string_new();

	while(indice < list_size(listaDispositivosIO)){
		dispo = list_get(listaDispositivosIO, indice);
		if(nombreIO == dispo->nombre){
			return indice;
		}
		indice++;
	}
	return 0;
}

void eliminarDispositivoIO(t_dispositivoIO* dispo){
	free(dispo->nombre);
	free(dispo);
}

//funcion de la lib de IO
int mate_call_io_kernel(t_paquete* paquete){
	//datosDelPaquete = [int idCarpincho, mate_io_resource io, void *msg]

	char** datosDelPaquete = string_array_new();
	datosDelPaquete = string_split((char*)paquete->mensaje, ";");

	t_pcbCarpincho* carpincho = malloc(sizeof(t_pcbCarpincho));
	carpincho->pedidosIO = list_create();
	carpincho->pedidosSEM = list_create();
	carpincho = encontrarCarpincho(atoi(datosDelPaquete[0]));

	t_dispositivoIO* dispositivoIO = malloc(sizeof(t_dispositivoIO));
	dispositivoIO->nombre = string_new();
	dispositivoIO = encontrarDispositivoIO((mate_io_resource)datosDelPaquete[1]);


	carpincho->estado = BLOCKED;
	pthread_mutex_lock(&mutexExec);
	pthread_mutex_lock(&mutexBloq);
	moverDeLista(carpincho,listaExec,listaBloq);
	pthread_mutex_unlock(&mutexBloq);
	pthread_mutex_unlock(&mutexExec);
	carpincho->tiempoSalidaExec = temporalConversion();
	registrarRecursoIOenLista(carpincho,dispositivoIO->nombre);

	sem_post(&contadorMultiprocesamiento);

	pthread_mutex_lock(&(dispositivoIO->mutexEditarDispositivoIO));
	if((list_size(dispositivoIO->idCarpinchosEnEspera) == 0) && (dispositivoIO->enUso ==false)){
		//significa que no hay carpinchos en espera, y ademas el dispositivo no se esta usando en este momento
		dispositivoIO->enUso = true;

		pthread_mutex_unlock(&(dispositivoIO->mutexEditarDispositivoIO));

		ejecutarIO(carpincho, dispositivoIO, (char*)datosDelPaquete[2]);
	}

	else { //significa que el dispositivo esta en uso o que hay carpinchos en espera
		//lo agregamos a la lista de espera.
		pthread_mutex_lock(&(dispositivoIO->mutexEditarDispositivoIO));
		list_add(dispositivoIO->idCarpinchosEnEspera, carpincho);
		pthread_mutex_unlock(&(dispositivoIO->mutexEditarDispositivoIO));

		sem_wait(&(dispositivoIO->carpinchoEnEspera));  //aca se acumulan todos

		pthread_mutex_lock(&(dispositivoIO->mutexEditarDispositivoIO));
		dispositivoIO->enUso = true;
		pthread_mutex_unlock(&(dispositivoIO->mutexEditarDispositivoIO));

		list_remove(dispositivoIO->idCarpinchosEnEspera, 0);

		ejecutarIO(carpincho, dispositivoIO, (char*)datosDelPaquete[2]);
	}

	while(carpincho->estado != EXEC){
		continue;
		//tiene que esperar a que los planis lo pongan de nuevo en exec.
	}

	return 0;
}

void ejecutarIO(t_pcbCarpincho* carpincho, t_dispositivoIO* dispositivoIO, char* msg){
	pthread_mutex_lock(&mutexLoggerKernel);
	log_debug(loggerKernel, "El carpincho %d puede usar el io %s", carpincho->pid, dispositivoIO->nombre);
	pthread_mutex_unlock(&mutexLoggerKernel);

	usleep(dispositivoIO->duracion *1000);
	log_debug(loggerKernel, "Carpincho: %d \t\t imprime: %s \t\tdispositivoIO: %s", carpincho->pid, msg, dispositivoIO->nombre);

	pthread_mutex_lock(&mutexLoggerKernel);
	log_debug(loggerKernel, "El carpincho %d termino io %s", carpincho->pid, dispositivoIO->nombre);
	pthread_mutex_unlock(&mutexLoggerKernel);

	pthread_mutex_lock(&(dispositivoIO->mutexEditarDispositivoIO));
	dispositivoIO->enUso = false;
	pthread_mutex_unlock(&(dispositivoIO->mutexEditarDispositivoIO));

	liberarRecursoIO(carpincho, dispositivoIO->nombre);
	sem_post(&(dispositivoIO->carpinchoEnEspera));
}


/*
 * VERSION ANTERIOR DE CALL IO
 *
	int valorDeIO = kernel_sem_getvalueIO((mate_sem_name)datosDelPaquete[1]);         //KERNEL_SEM_GETVALUE
	t_pcbCarpincho* carpincho = malloc(sizeof(t_pcbCarpincho));
	carpincho->pedidosIO = list_create();
	carpincho->pedidosSEM = list_create();
	carpincho = encontrarCarpincho(atoi(datosDelPaquete[0]));

	if(valorDeIO > 0){
		kernel_sem_waitIO((mate_sem_name)datosDelPaquete[1]);                             //KERNEL_SEM_WAIT
		registrarRecursoIOenLista(carpincho,(mate_sem_name)datosDelPaquete[1]);
		carpincho->estado = BLOCKED;

		pthread_mutex_lock(&mutexExec);
		pthread_mutex_lock(&mutexBloq);
		moverDeLista(carpincho,listaExec,listaBloq);
		pthread_mutex_unlock(&mutexBloq);
		pthread_mutex_unlock(&mutexExec);

		carpincho->tiempoSalidaExec = temporalConversion();
		sem_post(&contadorMultiprocesamiento);

		int indice = 0;
		mate_sem_name auxIO = string_new();
		
		int IOsize = list_size(configKernel.dispositivosIO);
		while(indice < IOsize){
			auxIO =(mate_sem_name) list_get(configKernel.dispositivosIO,indice);
			if(auxIO == (mate_sem_name)datosDelPaquete[1]){
				break;
			}
			else{
				indice++;
			}
		}

		free(auxIO);

		int duracionIO = (int) list_get(configKernel.duracionesIO,indice);         //REVISAR ESTO
		usleep(duracionIO);
		printf("Printing content: %s\n", datosDelPaquete[2]);
		printf("Done with IO %s\n", datosDelPaquete[1]);
		kernel_sem_postIO((mate_sem_name)datosDelPaquete[1]);                                 //KERNEL_SEM_POST
		liberarRecursoIO(carpincho,(mate_sem_name)datosDelPaquete[1]);
		desbloqueoIO((mate_sem_name)datosDelPaquete[1]);

		if(carpincho->estado == SUSPENDED_BLOCKED){sem_post(&contadorMultiprogramacion);return 0;}
		if(carpincho->estado == SUSPENDED_READY){sem_post(&contadorMultiprogramacion);return 0;}
		//sem_wait(&contadorMultiprogramacion);
		return 0;
	}
	else{ //IO TOMADA  RECURSIVIDAD?
		pthread_mutex_lock(&mutexExec);
		pthread_mutex_lock(&mutexBloq);
		moverDeLista(carpincho,listaExec,listaBloq);
		pthread_mutex_unlock(&mutexBloq);
		pthread_mutex_unlock(&mutexExec);

		carpincho->tiempoSalidaExec = temporalConversion();
		carpincho->estado = BLOCKED;
		sem_post(&contadorMultiprocesamiento);

		kernel_sem_waitIO((mate_sem_name)datosDelPaquete[1]);
		while(kernel_sem_getvalueIO((mate_sem_name)datosDelPaquete[1]) != 1){   //KERNEL_SEM_WAIT
			continue;
		}
		

		pthread_mutex_lock(&contadorMultiprogramacion);
		if(carpincho->estado == SUSPENDED_BLOCKED){return;}
		if(carpincho->estado == SUSPENDED_READY){return;}
		pthread_mutex_lock(&contadorMultiprocesamiento);
		if(carpincho->estado == SUSPENDED_BLOCKED){return;}
		if(carpincho->estado == SUSPENDED_READY){return;}
		carpincho->estado = READY;

		sem_wait(&contadorMultiprogramacion);

		pthread_mutex_lock(&mutexReady);
		pthread_mutex_lock(&mutexBloq);
		moverDeLista(carpincho,listaBloq,listaReady);
		pthread_mutex_unlock(&mutexBloq);
		pthread_mutex_unlock(&mutexReady);
		
		carpinchoAux->estado = READY;
		carpinchoAux->tiempoInicioReady = temporalConversion();

		registrarRecursoIOenLista(carpincho,(mate_sem_name)datosDelPaquete[1]);
		kernel_sem_post((mate_sem_name)datosDelPaquete[1]);
	}
}*/


void removerSemDeListaGeneral(mate_sem_name sem){
	int indice = list_size(list_semaforos);
	int i=0;
	t_SEMAFORO* semAux= malloc(sizeof(t_SEMAFORO));
	semAux->carpinchos = list_create();
	semAux->nombre = string_new();
	while(i<indice){
		semAux = list_get(list_semaforos,i);
		if(semAux->nombre == sem){ break; }
		else{ i++; }
	}
  list_destroy(semAux->carpinchos);
	list_remove(list_semaforos,i);
}
