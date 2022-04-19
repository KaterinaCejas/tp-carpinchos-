#include "Kernel.h"

int main(/*int argc, char** argv*/) {
	// argc = 2
	// argv = ["./kernel", "path del archivo config"

	iniciarConfigYLog();
	iniciarSemaforosGenerales();
	iniciarListas();
	iniciarDispositivosIO();

	int socketMemoriaSuspension = crearConexion(configKernel.ipMemoria, configKernel.puertoMemoria);
	if(socketMemoriaSuspension != -1){
		log_debug(loggerKernel, "socketMemoriaSuspension creado");
	}
	log_debug(loggerKernel, "kernel iniciado");


	pthread_t hiloPlaniLargoPlazo;
    pthread_create(&hiloPlaniLargoPlazo, NULL, (void*) FIFO, (void*)socketMemoriaSuspension);


	pthread_t hiloPlaniMedianoPlazo;
    pthread_create(&hiloPlaniMedianoPlazo, NULL, (void*) planiMedianoPlazoBloqASuspendedBloq, (void*)socketMemoriaSuspension);

    pthread_t hiloPlaniCortoPlazo;
	if(string_equals_ignore_case(configKernel.algoritmo, "HRRN")){
		pthread_create(&hiloPlaniCortoPlazo, NULL, (void*)HRRN, NULL);
	}
	else{
		pthread_create(&hiloPlaniCortoPlazo, NULL, (void*)SJF, NULL);
	}


    pthread_t hiloDeadlock;
    pthread_create(&hiloDeadlock, NULL, (void*) deteccionYrecuperacion, NULL);



	/*for(int i=0; i<configKernel.gradoMultiprocesamiento; i++){
		pthread_t hiloCPU;
	    pthread_create(&hiloCPU, NULL, (void*) iniciarConexionCarpinchoMemoria, NULL);
	}*/
    iniciarConexionCarpinchoMemoria();

	log_debug(loggerKernel, "hilos creados");

	pthread_join(hiloPlaniLargoPlazo, NULL);
	pthread_join(hiloPlaniMedianoPlazo, NULL);
	pthread_join(hiloPlaniCortoPlazo, NULL);
	pthread_join(hiloDeadlock, NULL);

	return EXIT_SUCCESS;

}

void iniciarDispositivosIO(void){
	for(int i=0; i<list_size(configKernel.dispositivosIO); i++){
		t_dispositivoIO* dispositivoIO = malloc(sizeof(t_dispositivoIO));
		dispositivoIO->nombre = string_new();
		dispositivoIO->idCarpinchosEnEspera = list_create();
		dispositivoIO->nombre = list_get(configKernel.dispositivosIO, i);
		dispositivoIO->duracion = (int)list_get(configKernel.duracionesIO, i);
		dispositivoIO->enUso = false;
		pthread_mutex_init(&(dispositivoIO->mutexEditarDispositivoIO), NULL);
		sem_init(&(dispositivoIO->carpinchoEnEspera), 0, 0);
		list_add(listaDispositivosIO, dispositivoIO);
	}
}


int iniciarHiloDeadlock(void){
	pthread_t hiloDeadlock;
    if(!pthread_create(&hiloDeadlock, NULL, (void*) deteccionYrecuperacion, NULL)){
    	pthread_detach(hiloDeadlock);
    	return 0;
    }
    else {
        return -1;
    }
}

void cerrarKernel(void){
	list_destroy(listaNew);
	list_destroy(listaReady);
	list_destroy(listaExec);
	list_destroy(listaBloq);
	list_destroy(listaSuspendedBlocked);
	list_destroy(listaSuspendedReady);
	list_destroy_and_destroy_elements(listaExit, (void*)eliminarCarpincho);
	list_destroy_and_destroy_elements(list_semaforos, (void*)eliminarSemaforo);
	list_destroy(lista_carpinchos_deadlock);
	pthread_mutex_destroy(&mutexLoggerKernel);
	sem_destroy(&contadorMultiprogramacion);
	sem_destroy(&contadorMultiprocesamiento);
	pthread_mutex_destroy(&mutexNew);
	pthread_mutex_destroy(&mutexReady);
	pthread_mutex_destroy(&mutexExec);
	pthread_mutex_destroy(&mutexBloq);
	pthread_mutex_destroy(&mutexSuspendedBlocked);
	pthread_mutex_destroy(&mutexSuspendedReady);
	pthread_mutex_destroy(&mutexListaSemaforos);
	pthread_mutex_destroy(&mutexListaCarpinchosDeadlock);
	pthread_mutex_destroy(&mutexListaIO);
	eliminarConfig();
	log_destroy(loggerKernel);
}

void eliminarConfig(void){
	free(configKernel.algoritmo);
	free(configKernel.ipKernel);
	free(configKernel.ipMemoria);
	list_destroy_and_destroy_elements(configKernel.dispositivosIO, free);
	list_destroy_and_destroy_elements(configKernel.duracionesIO, free);
}

void eliminarCarpincho(t_pcbCarpincho* carpincho){
	list_destroy_and_destroy_elements(carpincho->pedidosIO, free);
	list_destroy_and_destroy_elements(carpincho->pedidosSEM, free);
	free(carpincho);
}

void eliminarSemaforo(t_SEMAFORO* semaforo) {
	list_destroy(semaforo->carpinchos);
	free(semaforo->nombre);
	free(semaforo);
}

void iniciarListas(void){
	listaNew = list_create();
	listaReady = list_create();
	listaExec = list_create();
	listaBloq = list_create();
	listaExit = list_create();
	listaSuspendedBlocked = list_create();
	listaSuspendedReady = list_create();
	list_semaforos = list_create();
	lista_carpinchos_deadlock = list_create();
	listaGeneralTodosCarpinchos =list_create();
	listaDispositivosIO = list_create();

}

void iniciarSemaforosGenerales(void){
	sem_init(&contadorMultiprogramacion, 0, configKernel.gradoMultiprogramacion);
	sem_init(&contadorMultiprocesamiento, 0, configKernel.gradoMultiprocesamiento);
	sem_init(&ordenCortoPlazo, 0, 0);
	sem_init(&ordenLargoPlazo, 0, 0);
	pthread_mutex_init(&mutexNew, NULL);
	pthread_mutex_init(&mutexReady, NULL);
	pthread_mutex_init(&mutexExec, NULL);
	pthread_mutex_init(&mutexBloq, NULL);
	pthread_mutex_init(&mutexSuspendedBlocked, NULL);
	pthread_mutex_init(&mutexSuspendedReady, NULL);
	pthread_mutex_init(&mutexExit, NULL);

	pthread_mutex_init(&mutexListaSemaforos, NULL);
	pthread_mutex_init(&mutexListaCarpinchosDeadlock, NULL);
	pthread_mutex_init(&mutexListaIO, NULL);
	pthread_mutex_init(&mutexTodosLosCarpinchos, NULL);
}



void iniciarConfigYLog(void) {
	loggerKernel = log_create(PATH_LOG_KERNEL, "kernel", 1, LOG_LEVEL_DEBUG);
	t_config* archivoConfig = config_create(PATH_CONFIG_KERNEL);
	if (archivoConfig == NULL){
		printf("archivoConfig es NULL");
		exit(100);
	}
	extraerDatosConfig(archivoConfig);
	config_destroy(archivoConfig);
}

void extraerDatosConfig(t_config* archivoConfig) {
	configKernel.algoritmo = string_new();
	configKernel.ipKernel = string_new();
	configKernel.ipMemoria = string_new();
	configKernel.dispositivosIO = list_create();
	configKernel.duracionesIO = list_create();

	string_append(&configKernel.ipKernel,config_get_string_value(archivoConfig, "IP_KERNEL"));
	string_append(&configKernel.ipMemoria, config_get_string_value(archivoConfig, "IP_MEMORIA"));
	configKernel.puertoMemoria     = config_get_int_value(archivoConfig, "PUERTO_MEMORIA");
	configKernel.puertoEscucha     = config_get_int_value(archivoConfig, "PUERTO_ESCUCHA");
	string_append(&configKernel.algoritmo, config_get_string_value(archivoConfig, "ALGORITMO_PLANIFICACION"));
	configKernel.estimacionInicial = config_get_int_value(archivoConfig, "ESTIMACION_INICIAL");
	configKernel.alfa              = config_get_int_value(archivoConfig, "ALFA");
	configKernel.gradoMultiprogramacion   = config_get_int_value(archivoConfig, "GRADO_MULTIPROGRAMACION");
	configKernel.gradoMultiprocesamiento  = config_get_int_value(archivoConfig, "GRADO_MULTIPROCESAMIENTO");
	configKernel.tiempoDeadlock           = config_get_int_value(archivoConfig, "TIEMPO_DEADLOCK");

	char** arrayDispositivosIO = string_array_new();
	arrayDispositivosIO = config_get_array_value(archivoConfig, "DISPOSITIVOS_IO"); 
	int sizeArrayDispositivosIO = string_array_size(arrayDispositivosIO);
	mate_sem_name* dispositivosIO = string_array_new();

	dispositivosIO = arrayDispositivosIO;

	for(int i = 0; i<sizeArrayDispositivosIO; i++){
		list_add(configKernel.dispositivosIO, dispositivosIO[i]); 
	}

	char** arrayDuracionesIO = string_array_new();
	arrayDuracionesIO = config_get_array_value(archivoConfig, "DURACIONES_IO"); 
	int sizeArrayDuracionesIO = string_array_size(arrayDuracionesIO);
	for(int j = 0; j<sizeArrayDuracionesIO; j++){
		list_add(configKernel.duracionesIO, (int*)atoi(arrayDuracionesIO[j]));
	}

}

