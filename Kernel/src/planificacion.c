#include "Kernel.h"


void FIFO(void* voidSocketMemoriaSuspension) {  //planificador largo plazo: de NEW a READY
	int socketMemoriaSuspension = (int)voidSocketMemoriaSuspension;
	t_pcbCarpincho* carpincho = malloc(sizeof(t_pcbCarpincho));             //malloc
	carpincho->pedidosIO = list_create();
	carpincho->pedidosSEM = list_create();
	while(1){
		while(list_size(listaSuspendedReady) != 0){
			sem_wait(&contadorMultiprogramacion);
			//sem_wait(&ordenLargoPlazo);
			carpincho = list_get(listaSuspendedReady, 0);
			carpincho->estado = READY;

			int tamanioMensaje = strlen(string_itoa(carpincho->pid)) + 1;
			char* mensaje = malloc(tamanioMensaje);
			string_append(&mensaje, string_itoa(carpincho->pid));
			enviarMensaje(socketMemoriaSuspension, KERNEL, mensaje, tamanioMensaje, DES_SUSPENSION);

			pthread_mutex_lock(&mutexSuspendedReady);
			pthread_mutex_lock(&mutexReady);
			moverDeLista(carpincho,listaSuspendedReady,listaReady);
			pthread_mutex_unlock(&mutexReady);
			pthread_mutex_unlock(&mutexSuspendedReady);

			carpincho->tiempoInicioReady = temporalConversion();

			//sem_post(&ordenCortoPlazo);
		}

		if(list_size(listaNew) != 0){
			sem_wait(&contadorMultiprogramacion);
			//sem_wait(&orden)
			carpincho = list_get(listaNew, 0);
			carpincho->estado = READY;

			pthread_mutex_lock(&mutexNew);
			pthread_mutex_lock(&mutexReady);
			moverDeLista(carpincho,listaNew,listaReady);
			pthread_mutex_unlock(&mutexReady);
			pthread_mutex_unlock(&mutexNew);

			carpincho->tiempoInicioReady = temporalConversion();
		}
	}
}

void SJF(void) {
	pthread_mutex_lock(&mutexReady);
	t_list* listaReadyAux = list_create();
	pthread_mutex_unlock(&mutexReady);
	int sizeListaReadyAux;
    int j;int i;
	int sizeListaReady;                                                                                     
	t_pcbCarpincho* carpinchoConMenorEst = malloc(sizeof(t_pcbCarpincho));     //malloc
	carpinchoConMenorEst->pedidosIO = list_create();
	carpinchoConMenorEst->pedidosSEM = list_create();

	t_pcbCarpincho* carpinchoAux= malloc(sizeof(t_pcbCarpincho));             //malloc;
	carpinchoAux->pedidosIO = list_create();
	carpinchoAux->pedidosSEM = list_create();
	while(1){
		listaReadyAux = list_duplicate(listaReady);
		if(list_size(listaReadyAux) !=0){
			log_debug(loggerKernel, "entre al if de sjf");
			sem_wait(&contadorMultiprocesamiento);
			carpinchoConMenorEst = list_get(listaReadyAux, 0);
			sizeListaReadyAux = list_size(listaReadyAux);
			mapeoManualSJF(listaReadyAux);
			pthread_mutex_lock(&mutexReady);
			for(i=1; i<sizeListaReadyAux; i++){
				carpinchoAux = list_get(listaReadyAux, i);
				if(carpinchoConMenorEst->estimacionActual < carpinchoAux->estimacionActual)
					continue;
				else
					carpinchoConMenorEst = carpinchoAux;
			}

			j = 0;
			pthread_mutex_lock(&mutexReady);
			sizeListaReady = list_size(listaReady);
			pthread_mutex_unlock(&mutexReady);
			while(j<sizeListaReady){
				carpinchoAux = list_get(listaReady,j);
				if(carpinchoAux->pid == carpinchoConMenorEst->pid){

					pthread_mutex_lock(&mutexExec);
					moverDeLista(carpinchoConMenorEst,listaReadyAux,listaExec);
					pthread_mutex_unlock(&mutexExec);

					carpinchoConMenorEst->estado = EXEC;
					carpinchoConMenorEst->estimacionAnterior = carpinchoConMenorEst->estimacionActual;
					carpinchoConMenorEst->tiempoSalidaReady = temporalConversion();
					carpinchoConMenorEst->tiempoInicioExec = carpinchoConMenorEst->tiempoSalidaReady;
					list_remove(listaReady,j);
					pthread_mutex_lock(&mutexReady);
					listaReady = listaReadyAux;
					pthread_mutex_unlock(&mutexReady);
					list_destroy(listaReadyAux);
					eliminarCarpincho(carpinchoAux);

					break;
				}
				else{j++;}
			}
		}
	}
}
	
	

void HRRN(void) {  //proxima estimacion = (timepo en cola  +  rafaga) / rafaga.
	t_pcbCarpincho* carpinchoConMenorEst = malloc(sizeof(t_pcbCarpincho));             //malloc
	carpinchoConMenorEst->pedidosIO = list_create();
	carpinchoConMenorEst->pedidosSEM = list_create();
	t_pcbCarpincho* carpinchoAux = malloc(sizeof(t_pcbCarpincho));     //malloc;
	carpinchoAux->pedidosIO = list_create();
	carpinchoAux->pedidosSEM = list_create();
	int sizeListaReadyAux;
	int j;int i;
	int sizeListaReady;
	while(1){

		if(list_size(listaReady) !=0){
			t_list* listaReadyAux = list_duplicate(listaReady);
			log_debug(loggerKernel, "cantidad de carpinchos en ready: %d", list_size(listaReady));
			sem_wait(&contadorMultiprocesamiento);

			carpinchoConMenorEst= (t_pcbCarpincho*)list_get(listaReady, 0);
			pthread_mutex_lock(&mutexReady);
			mapeoManualHRRN(listaReadyAux);
			sizeListaReadyAux = list_size(listaReadyAux);
			for(i=1; i<sizeListaReadyAux; i++){
				carpinchoAux = list_get(listaReadyAux, i);
				if(carpinchoConMenorEst->estimacionActual < carpinchoAux->estimacionActual)
					continue;
				else
					carpinchoConMenorEst = carpinchoAux;
			}
			j = 0;
			sizeListaReady = list_size(listaReady);
			while(j<sizeListaReady){
				carpinchoAux = list_get(listaReady,j);
				if(carpinchoAux->pid == carpinchoConMenorEst->pid){

					pthread_mutex_lock(&mutexExec);
					moverDeLista(carpinchoConMenorEst,listaReadyAux,listaExec);
					pthread_mutex_unlock(&mutexExec);

					carpinchoConMenorEst->estado = EXEC;
					carpinchoConMenorEst->tiempoSalidaReady = temporalConversion();
					carpinchoConMenorEst->tiempoInicioExec = carpinchoConMenorEst->tiempoSalidaReady;
					list_remove(listaReady,j);

					listaReady = list_duplicate(listaReadyAux);
					pthread_mutex_unlock(&mutexReady);
					list_destroy(listaReadyAux);
					eliminarCarpincho(carpinchoAux);

					break;
				}
				else{
					j++;
				}
			}
			list_destroy(listaReadyAux);
		}
	}
}


/*void PlanifCortoPlazo(void){
	if(string_equals_ignore_case(configKernel.algoritmo, "HRRN")){
		HRRN();
	}
	else{
		SJF();
	}
}*/


void planiMedianoPlazoBloqASuspendedBloq(void* voidSocketMemoriaSuspension){ // chequea primero las condiciones
	int socketMemoriaSuspension = (int)voidSocketMemoriaSuspension;
	while(1){
		if(list_size(listaBloq)>0 && list_size(listaReady)==0 && list_size(listaNew)>0){
			int indiceLista = list_size(listaBloq);
			t_pcbCarpincho* carpincho = malloc(sizeof(t_pcbCarpincho));
			carpincho->pedidosIO = list_create();
			carpincho->pedidosSEM = list_create();
			carpincho = list_get(listaBloq,indiceLista);
			carpincho->estado = SUSPENDED_BLOCKED;

			pthread_mutex_lock(&mutexBloq);
			pthread_mutex_lock(&mutexSuspendedBlocked);
			moverDeLista(carpincho, listaBloq,listaSuspendedBlocked);
			pthread_mutex_unlock(&mutexSuspendedBlocked);
			pthread_mutex_unlock(&mutexBloq);

			int tamanioMensaje = strlen(string_itoa(carpincho->pid)) + 1;
			char* mensaje = malloc(tamanioMensaje);
			string_append(&mensaje, string_itoa(carpincho->pid));
			enviarMensaje(socketMemoriaSuspension, KERNEL, mensaje, tamanioMensaje, SUSPENSION);

			sem_post(&contadorMultiprogramacion);
		}

		int indiceLista;
		if(list_size(listaSuspendedBlocked) == 0){
			t_pcbCarpincho* carpinchoAux = malloc(sizeof(t_pcbCarpincho));
			carpinchoAux->pedidosIO = list_create();
			carpinchoAux->pedidosSEM = list_create();
			indiceLista = list_size(listaSuspendedBlocked);
			int IndiceAux=0;

			while(IndiceAux<indiceLista){
				carpinchoAux = list_get(listaSuspendedBlocked,IndiceAux);
				if(list_size(carpinchoAux->pedidosIO) == 0){
					pthread_mutex_lock(&mutexSuspendedBlocked);
					pthread_mutex_lock(&mutexSuspendedReady);
					moverDeLista(carpinchoAux,listaSuspendedBlocked,listaSuspendedReady);
					pthread_mutex_unlock(&mutexSuspendedReady);
					pthread_mutex_unlock(&mutexSuspendedBlocked);

					carpinchoAux->estado = SUSPENDED_READY;
					IndiceAux++;
				}
				else{
					IndiceAux++;
				}
			}
		}
	}
}



//desbloqueo para poner en mate_call_io,
void desbloqueoIO(mate_sem_name dispositivoIO){
	int indiceLista = list_size(listaBloq);
	t_pcbCarpincho* carpinchoAux = malloc(sizeof(t_pcbCarpincho));             //malloc;
	carpinchoAux->pedidosIO = list_create();
	carpinchoAux->pedidosSEM = list_create();

	while (indiceLista > 0){
		carpinchoAux = list_get(listaBloq,indiceLista);
		if(ioPedidoPorElCarpincho(dispositivoIO,carpinchoAux) == true){
			sem_wait(&contadorMultiprogramacion);
			kernel_sem_post(dispositivoIO);                                    //KERNEL_SEM_POST
			pthread_mutex_lock(&mutexBloq);
			pthread_mutex_lock(&mutexReady);
			moverDeLista(carpinchoAux,listaBloq,listaReady);
			pthread_mutex_unlock(&mutexReady);
			pthread_mutex_unlock(&mutexBloq);

			carpinchoAux->estado = READY;
			carpinchoAux->tiempoInicioReady = temporalConversion();

		}
		else{
			indiceLista--;
		}
	}

	if(list_size(listaSuspendedBlocked)>0){
		indiceLista = list_size(listaSuspendedBlocked);
		int IndiceAux=0;
		while(IndiceAux<indiceLista){
			carpinchoAux = list_get(listaSuspendedBlocked,IndiceAux);      //MOVER AL BUCLE DE ARRIBA
			if(ioPedidoPorElCarpincho(dispositivoIO,carpinchoAux) == true){
				kernel_sem_post(dispositivoIO);                            //KERNEL_SEM_POST   REVISAR PORQ ES DE IO

				pthread_mutex_lock(&mutexSuspendedBlocked);
				pthread_mutex_lock(&mutexSuspendedReady);
				moverDeLista(carpinchoAux,listaSuspendedBlocked,listaSuspendedReady);
				pthread_mutex_unlock(&mutexSuspendedReady);
				pthread_mutex_unlock(&mutexSuspendedBlocked);

				carpinchoAux->estado = SUSPENDED_READY;
				IndiceAux++;
			}
			else{
				IndiceAux++;
			}
		}
	}
	eliminarCarpincho(carpinchoAux);
}


void moverDeLista(t_pcbCarpincho* carpincho,t_list * ListaActual,t_list * ListaDestino){
	int i=0;
	t_pcbCarpincho* carpinchoAux = malloc(sizeof(t_pcbCarpincho));
	carpinchoAux->pedidosIO = list_create();
	carpinchoAux->pedidosSEM = list_create();
	carpinchoAux = list_get(ListaActual,i);

	while(carpinchoAux->pid != carpincho->pid){
		i++;
		carpinchoAux = list_get(ListaActual,i);
	}
	list_add(ListaDestino,carpincho);
	list_remove(ListaActual,i);
}


void mapeoManualHRRN(t_list* lista){
	int indice = list_size(lista);
	int i = 0;
	t_pcbCarpincho* carpinchoAux = malloc(sizeof(t_pcbCarpincho));
	carpinchoAux->pedidosIO = list_create();
	carpinchoAux->pedidosSEM = list_create();

	while(i<indice){
		carpinchoAux = list_get(lista, i);
		calculoDeRafagaHRRN(carpinchoAux);
		i++;
	}
	eliminarCarpincho(carpinchoAux);
}


void calculoDeRafagaHRRN(t_pcbCarpincho* carpincho){
	int tiempoActual =  temporalConversion();
	carpincho->tiempoDeEspera = tiempoActual - carpincho->tiempoInicioReady;
	if(carpincho->tiempoSalidaExec == 0){
		carpincho->rafagaRealAnterior = configKernel.estimacionInicial;
	}
	else {
		carpincho->rafagaRealAnterior = carpincho->tiempoSalidaExec - carpincho->tiempoInicioExec;
	}
	carpincho->estimacionActual = (carpincho->tiempoDeEspera + carpincho->rafagaRealAnterior) / carpincho->rafagaRealAnterior;
}

void mapeoManualSJF(t_list* lista){
	int indice = list_size(lista);
	int i = 0;
	t_pcbCarpincho* carpinchoAux = malloc(sizeof(t_pcbCarpincho));
	carpinchoAux->pedidosIO = list_create();
	carpinchoAux->pedidosSEM = list_create();
	while(i<indice){
		calculoDeRafagaSJF(carpinchoAux);
		i++;
	}
	eliminarCarpincho(carpinchoAux);
}

void calculoDeRafagaSJF(t_pcbCarpincho* carpincho){
	carpincho->rafagaRealAnterior = carpincho->tiempoSalidaExec - carpincho->tiempoInicioExec;
	carpincho->estimacionActual = carpincho->estimacionAnterior * (carpincho->rafagaRealAnterior + configKernel.alfa) * (1 - configKernel.alfa);
}


void registrarRecursoIOenLista(t_pcbCarpincho* carpincho,mate_sem_name IO){
	list_add(carpincho->pedidosIO,IO);
}

void liberarRecursoIO(t_pcbCarpincho* carpincho,mate_sem_name IO){
	int indice = list_size(carpincho->pedidosIO);
	int i = 0;
	t_SEMAFORO* ioAux= malloc(sizeof(t_SEMAFORO));             //malloc;
	ioAux->carpinchos = list_create();
	ioAux->nombre = string_new();

	while(i<indice){
		ioAux=list_get(carpincho->pedidosIO,i);
		if(ioAux->nombre == IO){
			list_remove(carpincho->pedidosIO,i);
			eliminarSemaforo(ioAux); //COMPROBAR QUE NO ROMPA ESTO
			return;
		}
		else{i++;}
	}
eliminarSemaforo(ioAux);
}

void desBloquear(mate_sem_name sem){
	int size = list_size(listaBloq);
	int i = 0;
	t_pcbCarpincho* carpincho= malloc(sizeof(t_pcbCarpincho));             //malloc;
	carpincho->pedidosIO = list_create();
	carpincho->pedidosSEM = list_create();

	while(i<size){
		carpincho = list_get(listaBloq,i);
		if(pedidoPorElCarpincho(sem, carpincho) == true){
			sem_wait(&contadorMultiprogramacion);

			pthread_mutex_lock(&mutexBloq);
			pthread_mutex_lock(&mutexReady);
			moverDeLista(carpincho,listaBloq,listaReady);
			pthread_mutex_unlock(&mutexReady);
			pthread_mutex_unlock(&mutexBloq);

			carpincho->estado = READY;
			carpincho->tiempoInicioReady = temporalConversion();
			return;
		}
		else {i++;}
	}

	if(listaSuspendedBlocked!=NULL){
		int indiceLista = list_size(listaSuspendedBlocked);
		int Aux=0;
		while(Aux<indiceLista){
			carpincho = list_get(listaSuspendedBlocked,Aux);
			if(pedidoPorElCarpincho(sem, carpincho) == true){

				pthread_mutex_lock(&mutexSuspendedBlocked);
				pthread_mutex_lock(&mutexSuspendedReady);
				moverDeLista(carpincho,listaSuspendedBlocked,listaSuspendedReady);
				pthread_mutex_unlock(&mutexSuspendedReady);
				pthread_mutex_unlock(&mutexSuspendedBlocked);

				carpincho->estado = SUSPENDED_READY;
				Aux++;
			}
			else{Aux++;}
		}
	}
}

bool pedidoPorElCarpincho(mate_sem_name sem,t_pcbCarpincho* carpincho){
	int size = list_size(carpincho->pedidosSEM);
	int i = 0;
	mate_sem_name semAux = string_new();     //string_new

	while(i<size){
		semAux = list_get(carpincho->pedidosSEM,i);
		if(semAux == sem){
			free(semAux); //COMPROBAR QUE NO ROMPA ESTO
			return true;
		}
		else{
			i++;
		}
	}

	free(semAux); //COMPROBAR QUE NO ROMPA ESTO
	return false;
}

bool ioPedidoPorElCarpincho(mate_sem_name io,t_pcbCarpincho* carpincho){
	int size = list_size(carpincho->pedidosIO);
	int i = 0;
	mate_sem_name ioAux = string_new();     //string_new;

	while(i<size){
		ioAux = list_get(carpincho->pedidosSEM,i);
		if(ioAux == io){
			free(ioAux); //COMPROBAR QUE NO ROMPA ESTO
			return true;
		}
		else{
			i++;
		}
	}

	free(ioAux); //COMPROBAR QUE NO ROMPA ESTO
	return false;
}



void kernel_sem_wait(mate_sem_name nombre){
	int indice = list_size(list_semaforos);
	int i=0;
	t_SEMAFORO* semaforo= malloc(sizeof(t_SEMAFORO));             //malloc;
	semaforo->carpinchos = list_create();
	semaforo->nombre = string_new();

	while(i<indice){
		semaforo = list_get(list_semaforos,i);
		if(semaforo->nombre == nombre){
			semaforo->recursosDisponibles = semaforo->recursosDisponibles - 1;
			break;
		}
		else{i++;}
	}
	indice = buscarIndiceSemaforos(list_semaforos,semaforo->nombre);
	pthread_mutex_lock(&mutexListaSemaforos);
	list_remove_and_destroy_element(list_semaforos,indice,(void*)eliminarSemaforo);
	list_add_in_index(list_semaforos,indice,semaforo);
	pthread_mutex_unlock(&mutexListaSemaforos);

}

void kernel_sem_post(mate_sem_name nombre){
	int indice = list_size(list_semaforos);
	int i=0;
	t_SEMAFORO* semaforoAux= malloc(sizeof(t_SEMAFORO));             //malloc;
	semaforoAux->carpinchos = list_create();
	semaforoAux->nombre = string_new();

	while(i<indice){
		semaforoAux = list_get(list_semaforos,i);
		if(semaforoAux->nombre == nombre){
			semaforoAux->recursosDisponibles = semaforoAux->recursosDisponibles + 1;
			break;
		}
		else{i++;}
	}
	indice = buscarIndiceSemaforos(list_semaforos,semaforoAux->nombre);
	pthread_mutex_lock(&mutexListaSemaforos);
	list_remove_and_destroy_element(list_semaforos,indice,(void*)eliminarSemaforo);
	list_add_in_index(list_semaforos,indice,semaforoAux);
	pthread_mutex_unlock(&mutexListaSemaforos);
}


int kernel_sem_getvalue(mate_sem_name nombre){
	int indice = list_size(list_semaforos);
	int i=0;
	t_SEMAFORO* semaforoAux= malloc(sizeof(t_SEMAFORO));             //malloc;
	semaforoAux->carpinchos = list_create();
	semaforoAux->nombre = string_new();

	while(i<indice){
		semaforoAux = list_get(list_semaforos,i);
		if(semaforoAux->nombre == nombre){
			int valorDeSem = semaforoAux->recursosDisponibles;
			eliminarSemaforo(semaforoAux);
			return valorDeSem;
		}
		else{i++;}
    }
	return 0;
}

int kernel_sem_getvalueIO(mate_sem_name nombre){
	int indice = list_size(configKernel.dispositivosIO);
	int i=0;
	t_SEMAFORO* semaforoAux= malloc(sizeof(t_SEMAFORO));             //malloc;
	semaforoAux->carpinchos = list_create();
	semaforoAux->nombre = string_new();

	while(i<indice){
		semaforoAux = list_get(configKernel.dispositivosIO,i);
		if(semaforoAux->nombre == nombre){
			int semValueIO = semaforoAux->recursosDisponibles;
			eliminarSemaforo(semaforoAux);
			return semValueIO;
		}
		else{i++;}
    }
	return 0;
}


void kernel_sem_waitIO(mate_sem_name nombre){
	int indice = list_size(configKernel.dispositivosIO);
	int i=0;
	t_SEMAFORO* semaforoAux= malloc(sizeof(t_SEMAFORO));             //malloc;
	semaforoAux->carpinchos = list_create();
	semaforoAux->nombre = string_new();

	while(i<indice){
		semaforoAux = list_get(configKernel.dispositivosIO,i);
		if(semaforoAux->nombre == nombre){
			semaforoAux->recursosDisponibles = semaforoAux->recursosDisponibles - 1;
			break;
		}
		else{i++;}
	}
	indice = buscarIndiceSemaforos(list_semaforos,semaforoAux->nombre);
	pthread_mutex_lock(&mutexListaSemaforos);
	list_remove_and_destroy_element(list_semaforos,indice,(void*)eliminarSemaforo);
	list_add_in_index(list_semaforos,indice,semaforoAux);
	pthread_mutex_unlock(&mutexListaSemaforos);
}

void kernel_sem_postIO(mate_sem_name nombre){
	int indice = list_size(configKernel.dispositivosIO);
	int i=0;
	t_SEMAFORO* semaforoAux= malloc(sizeof(t_SEMAFORO));             //malloc;
	semaforoAux->carpinchos = list_create();
	semaforoAux->nombre = string_new();

	while(i<indice){
		semaforoAux = list_get(configKernel.dispositivosIO,i);
		if(semaforoAux->nombre == nombre){
			semaforoAux->recursosDisponibles = semaforoAux->recursosDisponibles + 1;
			break;
		}
		else{i++;}
	}
	indice = buscarIndiceSemaforos(list_semaforos,semaforoAux->nombre);
	pthread_mutex_lock(&mutexListaSemaforos);
	list_remove_and_destroy_element(list_semaforos,indice,(void*)eliminarSemaforo);
	list_add_in_index(list_semaforos,indice,semaforoAux);
	pthread_mutex_unlock(&mutexListaSemaforos);
}

int temporalConversion(void){
	char* stringTemporal = string_new();
	stringTemporal = temporal_get_string_time("%H:%M:%S:%MS");
	char** arrayString =  string_array_new();
	arrayString = string_split(stringTemporal, ":");
	free(stringTemporal);
	//string_split devuelve NULL en la ultima posicion del array, hay que eliminarlo
	char* stringNull = string_new();
	stringNull =  string_array_pop(arrayString); //elimina y retorna el ultimo elemento del array
	free(stringNull); //lo borro
	/*
	 * hasta aca el arrayString tiene [horas, minutos, segundos, milisegundos] en strings
	 */
	char* horasString = string_new();
	horasString = arrayString[0];
	char* minutosString = string_new();
	minutosString = arrayString[1];
	char* segundosString = string_new();
	segundosString = arrayString[2];
	char* milisegundosString = string_new();
	milisegundosString = arrayString[3];
	int minutos = atoi(minutosString);
	int segundos = atoi(segundosString);
	//int milisegundos = atoi(milisegundosString);
	free(horasString);
	free(minutosString);
	free(segundosString);
	free(milisegundosString);
	int milisegundosTotales = (minutos*60000) + (segundos*1000);
//milisegundos
	return milisegundosTotales;
}




