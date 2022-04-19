#include "Kernel.h"

void registrarRecursosEnListas(t_pcbCarpincho* carpincho,mate_sem_name SEM){
	list_add(carpincho->pedidosSEM,SEM);
	int indice = buscarIndiceSemaforos(list_semaforos,SEM);
	t_SEMAFORO* semAux = list_get(list_semaforos,indice);
	list_add(semAux->carpinchos,(int*)carpincho->pid);
}


void liberarRecursosRegistrados(mate_sem_name nombre, int carpinchoPID){
	int indice = list_size(list_semaforos);
	int i;
	t_SEMAFORO* SEMAUX = malloc(sizeof(t_SEMAFORO));            //malloc;
	SEMAUX->carpinchos = list_create();
	SEMAUX->nombre = string_new();
	int numeroARemover;
	while(i<indice){
		SEMAUX = list_get(list_semaforos,i);
		if(SEMAUX->nombre == nombre){
			numeroARemover = buscarIndiceCarpinchos(SEMAUX->carpinchos,carpinchoPID);
			list_remove(SEMAUX->carpinchos,numeroARemover);
		}
		else{i++;}
	}
	
	indice = list_size(listaExec);
	i=0;
	t_pcbCarpincho* carpinchoAux = malloc(sizeof(t_pcbCarpincho));            //malloc;
	carpinchoAux->pedidosIO = list_create();
	carpinchoAux->pedidosSEM = list_create();
	mate_sem_name semNombreAux = string_new();

	while(i<indice){
		carpinchoAux = list_get(listaExec,i);
		if(carpinchoAux->pid == carpinchoPID){
			int j = 0;
			int otroIndice = list_size(carpinchoAux->pedidosSEM);
			while(j<otroIndice){
				semNombreAux = list_get(carpinchoAux->pedidosSEM,j);
				if(semNombreAux == nombre){break;}
				else{j++;}
			}
			list_remove(carpinchoAux->pedidosSEM,j);
		}
		else{i++;}
	}
}

void deteccionYrecuperacion(void){
	int filas = list_size(list_semaforos);
	int columnas = list_size(listaBloq);
	int vectorRecursos[filas][columnas];
	int f=0;
	int c=0;
	int indice = 0;
	int carpinchosEnUnSEM = 0;
	t_pcbCarpincho* carpinchoEXEC = malloc(sizeof(t_pcbCarpincho));            //malloc
	carpinchoEXEC->pedidosIO = list_create();
	carpinchoEXEC->pedidosSEM = list_create();
	int pidAux;
	t_SEMAFORO* semAux = malloc(sizeof(t_SEMAFORO));            //malloc
	semAux->carpinchos = list_create();
	semAux->nombre = string_new();
	int pidEXEC;

	while(f<filas){
		semAux = list_get(list_semaforos,f);                                                         //ok esto es flojo quilombo: primero agarro el primer semaforo de la lista de semaforos generales,con f como indice
		carpinchosEnUnSEM = list_size(semAux->carpinchos);                                        //luego, de ese semoforo q agarre, agarro los pid de los carpinchos q solicitaron a ese semaforo(pidAux) con "indice" como indice
		while(indice<carpinchosEnUnSEM){
			c=0;	                                                							  //y agarro los carpinchos en exec y sus pid(pidEXEC), con c como indice
			pidAux = (int)list_get(semAux->carpinchos,indice);                                      //y ahora comparo  los pidAux y pidEXEC, si son iguales lo anoto(*) y cierro el loop de c, vuelvo c a 0, y sigo con el proximo pid de la listas de pid del sem
			while(c<columnas){
				carpinchoEXEC = list_get(listaBloq,c);             			                                   //si no son igules sigo con el proximo carpincho en exec
				pidEXEC = carpinchoEXEC->pid;                                                 //una vez terminado todos los carpinchos q tiene un semaforo, sigo al proximo semaforo (f++)
				if(pidEXEC==pidAux){
					if(indice == 1){
						vectorRecursos[f][c]=1;
					}
					else{ vectorRecursos[f][c]=2; }
					c=columnas;
					indice++;
				}                                                            				  //(*)anotar se volvio mas complejo, ahora al anotar me fijo, si el carpincho es el primero en las lista del semaforo anoto 1 (lo tiene asignado)
				else{ c++; }
			}
		}
																					//si no es el primero en la lista, anota 2 ( solicitado). esto es para tomar la decision de cual mandar a exit para deshacer el deadlock
		f++;
	}

	f=0;
	int indiceParaEmpezarDetecc = 0;
	int recursosTomados=0;
	c=0;  //antes era c = columnas

	indice = 0;

	while(f<filas){
		recursosTomados = recursosTomados + vectorRecursos[f][1];
		f++;
	}

	f=0;

	while(f<filas){
		c=0;
		while(c<columnas){
			indice = indice + vectorRecursos[f][c];
			if(indice >= 3){indiceParaEmpezarDetecc = f; break;}
			else{c++;}
		}
		f++;
	}

	f++;

	int indiceParaEmpezarDetecc2 =indiceParaEmpezarDetecc;
	while(f<filas){
		c=0;
		while(c<columnas){
			indice = indice + vectorRecursos[f][c];
			if(indice >= 3){
				indiceParaEmpezarDetecc2 = f;
				break;
			}
			else{c++;}
		}
		f++;
	}

	int indiceParaEmpezarDetecc3 = indiceParaEmpezarDetecc2;
	while(f<filas){
		c=0;
		while(c<columnas){
			indice = indice + vectorRecursos[f][c];
			if(indice >= 3){
				indiceParaEmpezarDetecc3 = f;
				break;
			}
			else{c++;}
		}
		f++;
	}

	if(recursosTomados >= 4 ){
		log_debug(loggerKernel, "Comienza POSIBLE deteccion de deadlock");
		deteccion(indiceParaEmpezarDetecc);
		if(indiceParaEmpezarDetecc2 == indiceParaEmpezarDetecc){
			deteccion(indiceParaEmpezarDetecc2);
		}
		if(indiceParaEmpezarDetecc3 == indiceParaEmpezarDetecc2){
			deteccion(indiceParaEmpezarDetecc3);
		}
	}

	//eliminarCarpincho(carpinchoEXEC);
	//eliminarSemaforo(semAux);
	log_debug(loggerKernel, "Termino proceso de deteccion de deadlock. Inicio sleep deadlock");
	sleep(configKernel.tiempoDeadlock);
	deteccionYrecuperacion();
}



void deteccion(int indice){
	bool estaEnLista = false;
	mate_sem_name idSemaforo = string_new();     //string_new;
	t_pcbCarpincho* carpinchoAux= malloc(sizeof(t_pcbCarpincho));            //malloc;
	carpinchoAux->pedidosIO = list_create();
	carpinchoAux->pedidosSEM = list_create();
	int aux=0;
	t_SEMAFORO* semaforoAux= malloc(sizeof(t_SEMAFORO));            //malloc;
	semaforoAux->nombre = string_new();
	semaforoAux->carpinchos = list_create();

	semaforoAux = list_get(list_semaforos,indice);
	int idCarpincho = (int)list_get(semaforoAux->carpinchos,0);                                      //ok que es lo q pasa aca: esto es como voy a detectar el deadlock, basicamente estoy creando una lista que tiene nombres de carpinchos
	estaEnLista = buscarEnLista(idCarpincho);                                                   // esta lista esta compuesta por el primer carpincho de cada semaforo en la lista de semaforos general 

	if(estaEnLista == false){                                                                   // una vez q anoto a un carpincho en la lista, le pregunto al segundo carpincho ,en la lista de ese semaforo, cual es su primer semaforo pedido, voy a ese semaforo y anoto el primer carpincho y se repite
		list_add(lista_carpinchos_deadlock,(int*)idCarpincho);                                        //(si no hay un segundo carpincho en la lista del semaforo no hay deadlock -> return)
		aux = list_size(semaforoAux->carpinchos);
		if(aux<2){
			free(lista_carpinchos_deadlock);
			return;
		}                             //el loop termina cuando encuentre un carpincho repetido,un carpincho q era el primero en un semaforo y el segundo en otro semaforo, ahi elimino el mas grande
		idCarpincho = (int)list_get(semaforoAux->carpinchos,1);
		indice = buscarIndiceCarpinchos(listaBloq,idCarpincho);  
		carpinchoAux = list_get(listaBloq,indice);
		idSemaforo = list_get(carpinchoAux->pedidosSEM,0);
		indice = buscarIndiceSemaforos(list_semaforos,idSemaforo);
		deteccion(indice);
	}
	else{
		eliminarElDeMayorID();
	}
}



void eliminarElDeMayorID(void){
		int sizeListaDL = list_size(lista_carpinchos_deadlock);
		int i = 1;
		int mayorID = (int)list_get(lista_carpinchos_deadlock,0);
		int auxID = 0;
		while(i<sizeListaDL){
			auxID = (int)list_get(lista_carpinchos_deadlock,i);
			if(auxID > mayorID){
				mayorID = auxID;
				i++;
			}
			else{ i++; }
		}
		i = buscarIndiceCarpinchos(listaBloq, mayorID);
		matarPorDeadlock(i);
}

void matarPorDeadlock(int indice){
	t_pcbCarpincho* carpincho= malloc(sizeof(t_pcbCarpincho));            //malloc;
	carpincho->pedidosIO = list_create();
	carpincho->pedidosSEM = list_create();
	mate_sem_name semAux = string_new();     //string_new;
	carpincho = list_get(listaBloq,indice);
	indice = list_size(carpincho->pedidosSEM);
	int i = 0;

	while(i<indice){ 
		semAux = list_get(carpincho->pedidosSEM,i);                            //esto es para darle un "post" a todos los semaforos que tiene el carpincho pedido
		kernel_sem_post(semAux);                                        //KERNEL_SEM_POST
		i++;
	}
	
	indice = list_size(list_semaforos);
	int indiceChico;
	i=0;
	t_SEMAFORO* semaforoAux= malloc(sizeof(t_SEMAFORO));            //malloc;
	semaforoAux->nombre = string_new();
	semaforoAux->carpinchos = list_create();
	int j=0;
	int pidAux;

	while(i<indice){
		semaforoAux = list_get(list_semaforos,i);                               //esto de aca es para remover al carpincho de la listas de cada semaforo si presente
		indiceChico = list_size(semaforoAux->carpinchos);
		while(j<indiceChico){
			pidAux = (int)list_get(semaforoAux->carpinchos,j);
			if(pidAux == carpincho->pid){
				list_remove(semaforoAux->carpinchos,j);
				j=0;
				break;
			}
			else{
				j++;
			}
		}
		i++;
	}

	t_paquete paquete;
	paquete.header.tipoMensaje = MATE_CLOSE;
	paquete.header.cliente = KERNEL;
	int tamanioMensaje = strlen(string_itoa(carpincho->pid)) + 1;
	char* mensaje = malloc(tamanioMensaje);
	string_append(&mensaje, string_itoa(carpincho->pid));
	paquete.header.tamanioMensaje = tamanioMensaje;
	paquete.mensaje = mensaje;

	mate_close_kernel(&paquete,  carpincho->socketMemoria);

	carpincho->estado = EXIT;

	sem_post(&contadorMultiprogramacion);
	eliminarCarpincho(carpincho);
	eliminarSemaforo(semaforoAux);
	free(semAux);
}

int buscarIndiceCarpinchos(t_list* lista, int numeroABuscar){  
	int size = list_size(lista);
	int i =0;
	t_pcbCarpincho* carpinchoAux = malloc(sizeof(t_pcbCarpincho));
	carpinchoAux->pedidosIO = list_create();
	carpinchoAux->pedidosSEM = list_create();
	while(i<size){
		carpinchoAux = list_get(lista,i);
		if(carpinchoAux->pid == numeroABuscar){ return i; }
		else{ i++; }
	}
	return 0;
}

int buscarIndiceSemaforos(t_list* lista,mate_sem_name nombreABuscar){
	int size = list_size(lista);
	int i = 0;
	t_SEMAFORO* semAux= malloc(sizeof(t_SEMAFORO));            //malloc;
	semAux->nombre = string_new();
	semAux->carpinchos = list_create();
	while(i<size){
		semAux = list_get(lista,i);
		if(semAux->nombre == nombreABuscar){return i;}
		else{ i++; }
	}
	return 0;
}

bool buscarEnLista(int idCarpincho){
	int size = list_size(lista_carpinchos_deadlock);
	int i = 0;
	int aux;
	while(i<size){
		aux = (int)list_get(lista_carpinchos_deadlock,i);
		if(aux == idCarpincho){ return true; }
		else{i++;}
	}
	return false;
}

