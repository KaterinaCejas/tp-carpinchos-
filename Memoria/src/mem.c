#include "mem.h"

t_marco* swap_in(t_proceso* proceso, t_pagina* pagina);
t_marco* swap_out(t_proceso* proceso);
t_proceso* buscar_proceso(int pid);
void liberar_pagina(t_proceso* proceso, int num_pagina);

int recibir_entero(void* buffer){

		int entero;
		memcpy(&entero, buffer, sizeof(int));

		return entero;
	}

	void* recibir_buffer(int* size, int socket_cliente){
		void * buffer;
		int tamanio;
		recv(socket_cliente, &tamanio, sizeof(int), MSG_WAITALL);
		buffer = malloc(tamanio);
		recv(socket_cliente, buffer, tamanio, MSG_WAITALL);

		return buffer;
	}

	void* recibir_cadena_de_bytes(void* buffer, int* tamanio_cadena){

		*tamanio_cadena = recibir_entero(buffer);

		void* cadena = malloc(*tamanio_cadena);

		memcpy(cadena, buffer+sizeof(int), *tamanio_cadena);

		return cadena;
	}

	int recibir_de_paquete_entero(int socket_cliente){
		int size = 0;
		int pid;

		void* buffer = recibir_buffer(&size, socket_cliente);

		memcpy(&pid, buffer, sizeof(int));

		free(buffer);
		return pid;
	}

	void* recibir_de_paquete_cadena(int socket_cliente)
	{
		int size = -1;
		int tamanio_cadena = -1;

		void* buffer = recibir_buffer(&size, socket_cliente);

		char* nbytes = recibir_cadena_de_bytes(buffer, &tamanio_cadena);

		printf("estoy en recibir_de_paquete_cadena \ntamanio recibido %d \n", tamanio_cadena);
		return nbytes;
	}

void delete_carpincho(int idCarpincho){
	pthread_mutex_lock(&mutexLogger);
	log_debug(LOGGER, "MATE_CLOSE iniciado del carpincho %d", idCarpincho);
	pthread_mutex_unlock(&mutexLogger);

	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso = buscar_proceso(idCarpincho);
	t_pagina* pagina = malloc(sizeof(t_pagina));

	for(int i=0; i<list_size(proceso->tabla_de_paginas); i++){
		pagina = list_get(proceso->tabla_de_paginas, i);
		if(pagina->esta_presente){
			liberar_pagina(proceso, pagina->num_pagina);
		}

	}

	pthread_mutex_lock(&mutexLogger);
	log_debug(LOGGER, "MATE_CLOSE terminado del carpincho %d", idCarpincho);
	pthread_mutex_unlock(&mutexLogger);
}


int es_fecha_anterior(char* anterior, char* posterior){
	if(strcmp(anterior, posterior) == 0){ //SON IGUALES
		return false;
	} else if(strcmp(anterior, posterior) > 0){ //El primero es mas grande que el segundo
		return false;
	} else { //El segundo es mas g rande que el primero
		return true;
	}
}

t_marco* obtener_marco_lru(t_list* lista_marcos){
	int i = 0;
	t_marco* actual = list_get(lista_marcos, i);
	t_marco* leastRecentlyUsed = actual; //Victima

	//BUSQUEDA DE VICTIMA
	for(i=1; i< list_size(lista_marcos); i++){
		actual = list_get(lista_marcos, i);
		if(es_fecha_anterior(actual->last_use, leastRecentlyUsed->last_use)){
			leastRecentlyUsed = actual;
		}
	}

	return leastRecentlyUsed;
}

void avanzar_puntero(int tam_lista){
	puntero_clock++;
	if(puntero_clock >= tam_lista){
		puntero_clock = 0;
	}
}

t_marco* obtener_marco_clock_modificado(t_list* lista_marcos){
	t_marco* actual;
	//Primera vuelta: Caso Bit uso 0 && Bit modificado en 0

	for(int i = 0; i<list_size(lista_marcos); i++){
		actual = list_get(lista_marcos, puntero_clock);
		//Si existe la entrada reemplaza y sale.
		if(!actual->modificado && !actual->uso){
			avanzar_puntero(list_size(lista_marcos));
			return actual;
		}
		avanzar_puntero(list_size(lista_marcos));
	}

	//Segunda vuelta, busca Bit uso 0 y modificado 1, si el bit de uso esta en 1, lo pasa a 0
	for(int i = 0; i<list_size(lista_marcos); i++){
		actual = list_get(lista_marcos, puntero_clock);
		//Si existe la entrada reemplaza y sale.
		if(!actual->uso){
			avanzar_puntero(list_size(lista_marcos));
			return actual;
		}

		actual->uso = false;
		avanzar_puntero(list_size(lista_marcos));
	}


	//Tercera vuelta, caso borde, para esta altura todos los bits de uso deberian estar en 0 llegado el caso.
	//Reemplaza directamente por el primero (busca mismo PID si es asignacion FIJA)
	actual = list_get(lista_marcos, puntero_clock);
	avanzar_puntero(list_size(lista_marcos));

	return actual;
}

t_marco* obtener_victima(t_list* lista){
	//Marco en caso de LRU
	if(strcmp(CONFIG_MEMORIA->ALGORITMO_REEMPLAZO_MMU, "LRU")== 0)
		return obtener_marco_lru(lista);

	//Marco en caso de Clock Modificado
	return obtener_marco_clock_modificado(lista);

}


void actualizar_tabla_de_marcos(t_marco* marco, uint32_t pid, uint32_t num_pagina){
	//t_marco *marco = list_get(TABLA_DE_MARCOS, posTabla);
	marco->esta_libre = false;
	marco->num_pagina = num_pagina;
	marco->pid = pid;

	//logica para el caso de LRU
	if(string_equals_ignore_case(CONFIG_MEMORIA->ALGORITMO_REEMPLAZO_MMU, "LRU"))
		marco->last_use = temporal_get_string_time("%d/%m/%y %H:%M:%S");

	//logica para el caso de Clock Modificado
	if(string_equals_ignore_case(CONFIG_MEMORIA->ALGORITMO_REEMPLAZO_MMU, "CLOCK-M"))
		marco->uso = true;

}

char* traduccion(int numero)
{
	char* mensaje = NULL;

	switch (numero){
		case MATE_INIT:
			mensaje = string_duplicate("start_process");
			break;

		case MATE_MEMALLOC:
			mensaje = string_duplicate("reserve_memory");
			break;

		case MATE_MEMFREE:
			mensaje = string_duplicate("free_memory_");
			break;

		case MATE_MEMREAD:
			mensaje = string_duplicate("read_memory_");
			break;

		case MATE_MEMWRITE:
			mensaje = string_duplicate("write_memory_");
			break;
	}

	return mensaje;
}

void error_mems(int nivel, char* funcion_afectada, char* detalle)
{

	char* palabra1 = traduccion(nivel);

	error_show("error en: \"%s\" funcion: \"%s\" \nproblema: \"%s\"  \n", palabra1, funcion_afectada, detalle);

	free(palabra1);
	//exit(1);
}

t_proceso* buscar_proceso(int pid)
{
	bool funcionn(void* el_pid)
	{
		return (pid==(*(int*)el_pid))? true: false;
	}

	t_proceso* proceso = list_find(TABLA_GENERAL, funcionn);

	return proceso;
}

bool alcanza_un_proceso_mas_en_mmu()
{
	bool bandera = false;

	if(string_equals_ignore_case(CONFIG_MEMORIA->TIPO_ASIGNACION, "FIJA"))
	{
		int cant_procesos_en_mmu = list_size(TABLA_GENERAL);
		int cant_procesoso_maximos_en_mmu = (CONFIG_MEMORIA->TAMANIO/CONFIG_MEMORIA->TAMANIO_PAGINA)/CONFIG_MEMORIA->MARCOS_POR_CARPINCHO;

		bandera = cant_procesos_en_mmu < cant_procesoso_maximos_en_mmu;
	}

	return bandera;
}

t_proceso* crear_proceso(int pid)
{
	t_proceso* proceso = NULL;

	if(string_equals_ignore_case(CONFIG_MEMORIA->TIPO_ASIGNACION, "FIJA"))
	{
		if(alcanza_un_proceso_mas_en_mmu())
		{
			proceso = malloc(sizeof(t_proceso));

			proceso->pid = pid;
			proceso->tabla_de_paginas = list_create();
		}
		if(!alcanza_un_proceso_mas_en_mmu())
			log_error(LOGGER, "No se puede agregar a otro proceso mas en mmu");
					//exit(1);
	}
	//no le veo limitaciones para el tema de asignacion dinamica
	if(string_equals_ignore_case(CONFIG_MEMORIA->TIPO_ASIGNACION, "DINAMICA"))
	{
		proceso = malloc(sizeof(t_proceso));
		proceso->pid = pid;
		proceso->tabla_de_paginas = list_create();
	}

	return proceso;
}

t_pagina* create_pagina(int num_pagina, int num_marco)
{
	t_pagina* pagina = malloc(sizeof(t_pagina));

	pagina->num_pagina = num_pagina;
	pagina->marco_asignado = num_marco;
	pagina->esta_presente = true;
	pagina->esta_modificado = false;

	return pagina;
}

t_heap_metadata* create_heap_metadata(uint32_t direccion_anterior, uint32_t direccion_siguiente)
{
	t_heap_metadata* heap_metadata = malloc(sizeof(t_heap_metadata));

	heap_metadata->prevAlloc = direccion_anterior;
	heap_metadata->nextAlloc = direccion_siguiente;
	heap_metadata->isFree = true;

	return heap_metadata;
}

int parte_usada(int direccion)
{
	return direccion - ((direccion/CONFIG_MEMORIA->TAMANIO_PAGINA) * CONFIG_MEMORIA->TAMANIO_PAGINA);
}

int espacio_restante(int direccion_logica)
{
	return CONFIG_MEMORIA->TAMANIO_PAGINA - parte_usada(direccion_logica);
}

int buscar_en_memoria(t_proceso* proceso, int num_pagina)
{
	t_pagina* pagina = list_get(proceso->tabla_de_paginas, num_pagina);

	if(!(pagina->esta_presente))
	{
		swap_in(proceso, pagina);
	}

	//actualizar_TLB(num_pagina, proceso->pid, pagina->marco_asignado);


	//despues descomento essto de la TLB

	return pagina->marco_asignado;
}

int buscar_numero_de_marco(t_proceso* proceso, int num_pagina)
{
	int num_marco = -1;

	//num_marco = buscar_en_TLB(num_pagina, proceso->pid);

	//despues le agrego la tlb por que algo esta rnompiendo.

	//if(num_marco == -1)
		num_marco = buscar_en_memoria(proceso, num_pagina);

	return num_marco;
}
/*siguiente direccion en desuso ahora la nueva funcion es direccion_fisica();
  y que de controlar si es asignacion fija o dinamica se encargue solamente el asignar marco
void* siguiente_direccion(t_proceso* proceso)
{
	int df = -1;

	if(string_equals_ignore_case(CONFIG_MEMORIA->TIPO_ASIGNACION, "FIJA"))
	{
		if(hay_pagina_vacia(proceso->tabla_de_paginas))
		{
			t_pagina* pagina = buscar_pagina_vacia(proceso->tabla_de_paginas);
			df = pagina->marco_asignado * CONFIG_MEMORIA->TAMANIO_PAGINA ;
		}

	}
	if(string_equals_ignore_case(CONFIG_MEMORIA->TIPO_ASIGNACION, "DINAMICA"))
	{
		int df = (((t_marco*)asignar_marco(proceso))->num_marco) * CONFIG_MEMORIA->TAMANIO_PAGINA ;

	}

	return MI_MEMORIA + df;
}*/

void* direccion_fisica(t_proceso* proceso, int direccion_logica)
{
	int num_pagina = direccion_logica / CONFIG_MEMORIA->TAMANIO_PAGINA;
	int offset = direccion_logica % CONFIG_MEMORIA->TAMANIO_PAGINA;

	int num_frame = buscar_numero_de_marco(proceso, num_pagina);//se puede buscar el marco tanto en tlb como en la ram

	int df = num_frame * CONFIG_MEMORIA->TAMANIO_PAGINA + offset;

	return MI_MEMORIA + df;
}

t_pagina* que_este_en_memoria(t_proceso* proceso, t_pagina* pagina)
{
	if(pagina->esta_presente==true)
	{
		return pagina;
	}
	else
	{
		swap_in(proceso, pagina);
		return pagina;
	}

	return pagina;
}


t_pagina* que_este_en_memoria_para_sacar(t_proceso* proceso, t_pagina* pagina)
{
	t_marco* marco_ = NULL;

	if(pagina->esta_presente==true)
	{
		marco_ = list_get(TABLA_DE_MARCOS, pagina->marco_asignado);

	}
	else
	{
		marco_ = swap_in(proceso, pagina);

		//	pagina->esta_presente = true; lo hice adentro del swap in
		//	pagina->marco_asignado = marco->num_marco;
	}

	//logica para el caso de LRU
	if(string_equals_ignore_case(CONFIG_MEMORIA->ALGORITMO_REEMPLAZO_MMU, "LRU"))
		marco_->last_use = temporal_get_string_time("%d/%m/%y %H:%M:%S:%MS");
	//logica para el caso de Clock Modificado
	if(string_equals_ignore_case(CONFIG_MEMORIA->ALGORITMO_REEMPLAZO_MMU, "CLOCK-M"))
		marco_->uso = true;


	//despues ver el caso em asigancion dinamica
	actualizar_TLB(pagina->num_pagina, proceso->pid, marco_->num_marco);

	return pagina;
}

void* direccion_fisica_para_sacar(t_proceso* proceso, int direccion_logica)
{
	int num_pagina = direccion_logica / CONFIG_MEMORIA->TAMANIO_PAGINA;
	int offset = direccion_logica % CONFIG_MEMORIA->TAMANIO_PAGINA;

	t_pagina* pagina = list_get(proceso->tabla_de_paginas, num_pagina);

	int num_marco = buscar_en_TLB(num_pagina, proceso->pid);

	if (num_marco == -1){ //No lo encontro en la TLB
		pagina = que_este_en_memoria_para_sacar(proceso, pagina);
		num_marco = pagina->marco_asignado;
	}

	//actualizamos estado de variables
	t_marco* marco = list_get(TABLA_DE_MARCOS, num_marco);
	marco->last_use = temporal_get_string_time("%d/%m/%y %H:%M:%S:%MS");
	marco->uso = true;
	printf("actualizamos marco %d \n", marco->num_marco);
	//hasta aqui

	int df = num_marco * CONFIG_MEMORIA->TAMANIO_PAGINA + offset;
	//printf("la direcion_fisica = %d \n", df);

	return MI_MEMORIA + df;
}

t_pagina* que_este_en_memoria_para_guardar(t_proceso* proceso, t_pagina* pagina)
{
	t_marco* marco_ = NULL;

	if(pagina->esta_presente==true)
	{
		marco_ = list_get(TABLA_DE_MARCOS, pagina->marco_asignado);

	}
	else
	{
		marco_ = swap_out(proceso);

		pagina->esta_presente = true;
		pagina->marco_asignado = marco_->num_marco;
	}

	//logica para el caso de LRU
	//if(string_equals_ignore_case(CONFIG_MEMORIA->ALGORITMO_REEMPLAZO_MMU, "LRU"))
	//	marco_->last_use = temporal_get_string_time("%d/%m/%y %H:%M:%S:%MS");
	//logica para el caso de Clock Modificado
	//if(string_equals_ignore_case(CONFIG_MEMORIA->ALGORITMO_REEMPLAZO_MMU, "CLOCK-M"))
	//	marco_->uso = true;


	//despues ver el caso em asigancion dinamica
	actualizar_TLB(pagina->num_pagina, proceso->pid, marco_->num_marco);

	return pagina;
}

void* direccion_fisica_para_guardar(t_proceso* proceso, int direccion_logica)
{
	int num_pagina = direccion_logica / CONFIG_MEMORIA->TAMANIO_PAGINA;
	int offset = direccion_logica % CONFIG_MEMORIA->TAMANIO_PAGINA;

	t_pagina* pagina = list_get(proceso->tabla_de_paginas, num_pagina);

	int num_marco = buscar_en_TLB(num_pagina, proceso->pid);

	if (num_marco == -1){ //No lo encontro en la TLB
		pagina = que_este_en_memoria_para_guardar(proceso, pagina);
		num_marco = pagina->marco_asignado;

	}

	//de aqui a aqui
	//actualizamos estado de variables
	t_marco* marco = list_get(TABLA_DE_MARCOS, num_marco);
	marco->last_use = temporal_get_string_time("%d/%m/%y %H:%M:%S:%MS");
	marco->uso = true;
	printf("actualizamos marco %d", marco->num_marco);

	//hasta aqui

	int df = num_marco * CONFIG_MEMORIA->TAMANIO_PAGINA + offset;
	//printf("la direcion_fisica = %d \n", df);

	return MI_MEMORIA + df;
}

void guardar_heap_metadata(t_proceso* proceso, t_heap_metadata* heap, int direccion_logica)
{
	int tam_del_heap = sizeof(t_heap_metadata); // el tamanio vale 9 bytes
	int espacio_disponible = espacio_restante(direccion_logica);
	int aux = 0;
	void* aqui = NULL;

	aqui = direccion_fisica_para_guardar(proceso, direccion_logica);
	while(tam_del_heap > espacio_disponible)
	{
		memcpy((void*)aqui, (void*)heap +aux, espacio_disponible);
		tam_del_heap = tam_del_heap - espacio_disponible;
		direccion_logica = direccion_logica + espacio_disponible;
		aux = aux + espacio_disponible;
		espacio_disponible = CONFIG_MEMORIA->TAMANIO_PAGINA;
		aqui = direccion_fisica_para_guardar(proceso, direccion_logica);
	}

	memcpy((void*)aqui, (void*)heap + aux, tam_del_heap);
	printf("se va a guardar heap ant: %d sig: %d %d en %d  \n", heap->prevAlloc, heap->nextAlloc, heap->isFree, direccion_logica);

}
void asignar_primer_heapmetadata(t_proceso* proceso)
{
	t_heap_metadata* heap = create_heap_metadata(-1, -1);

	guardar_heap_metadata(proceso, heap, 0);
}

bool hay_marco_libre(t_marco* marco)
{
	return marco!=NULL;
}

bool no_hay_marco_libre(t_marco* marco)
{
	return marco==NULL;
}


t_marco* buscar_marco_libre_en_ram(t_proceso* proceso) //revisar si asi funcionaba esto de usar funciones de listas.
{
	bool funcionn(void* marco_X)
	{
		return ((t_marco*)(marco_X))->esta_libre == true;
	}

	t_marco* marco = list_find(TABLA_DE_MARCOS, funcionn);

	if(marco==NULL) return NULL;

	return marco;
}

void enviar_marco_a_disco(t_marco* marco)
{
	//marco->pid, marco->num_pagina, marco->inicio
	//usamos las funciones de enviar_ppaquete de clienteconeccion.h
	//CONEXION_SWAMP = crear_conexion("127.0.0.1", "5001");


	t_paquete_swamp* paquete = crear_paquete(WRITE);
	agregar_entero_a_paquete(paquete, marco->pid);
	agregar_entero_a_paquete(paquete, marco->num_pagina);
	//agregar_a_paquete(paquete, marco->inicio, CONFIG_MEMORIA->TAMANIO_PAGINA);
	/*
	t_heap_metadata* heap = malloc(sizeof(t_heap_metadata));
	memcpy((void*)heap, marco->inicio, sizeof(t_heap_metadata));
	*/
	//printf("Marco verga: %d \n\n\n\n\n\n", marco->inicio);

	agregar_n_bytes_a_paquete(paquete, marco->inicio, CONFIG_MEMORIA->TAMANIO_PAGINA);
	enviar_paquete(paquete, CONEXION_SWAMP);

	op_code recibir_operacion(int socket_cliente){
		int cod_op = -1;
		if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		{
			cod_op = -1;
		}

		return cod_op;
	}

	op_code op_code = recibir_operacion(CONEXION_SWAMP);
	printf("op_code: %d \n", op_code );
	int recibido = recibir_de_paquete_entero(CONEXION_SWAMP);

	printf("envie marco a disco  #marc %d, #pid %d, #pag %d  \n", marco->num_marco, marco->pid, marco->num_pagina);

	printf("lo que recibi %d \n", recibido);
}

void* recibir_nbytes(void* buffer){

	void* mensaje = malloc(CONFIG_MEMORIA->TAMANIO_PAGINA);
	memcpy(mensaje, buffer, CONFIG_MEMORIA->TAMANIO_PAGINA);

	return mensaje;
}



t_marco* eleccion_de_victima(t_list* lista)
{
	printf("#m   pid   #pag   esta_libre   last_use   uso   modificado  \n");
	for(int i=0; i<list_size(lista); i++)
	{
		t_marco* marco = list_get(lista, i);
		printf("%d    %d    %d     %d     %s       %d      %d  \n", marco->num_marco, marco->pid, marco->num_pagina, marco->esta_libre, marco->last_use, marco->uso, marco->modificado);
	}
	t_marco* marco = obtener_victima(lista);

	printf("la victima fue: %d\n", marco->num_marco);

	return marco;
}

t_marco* swap_out(t_proceso* proceso)
{
	bool ff(void* un_marco)
	{
		return ((t_marco*) un_marco)->pid == proceso->pid;
	}

	t_list* lista = TABLA_DE_MARCOS;

	//despues fijarse lo de las limitaciones de asignacion fija
	if(string_equals_ignore_case(CONFIG_MEMORIA->TIPO_ASIGNACION, "FIJA"))
	{
		lista = list_filter(TABLA_DE_MARCOS, ff);
	}
	t_marco* marco = eleccion_de_victima(lista);

	//ponemos como "pagina no presente" del marco que enviamos a memoria
	t_proceso* era_de_este_proceso = buscar_proceso(marco->pid);
	t_pagina* pagina = list_get(era_de_este_proceso->tabla_de_paginas, marco->num_pagina);
	pagina->esta_presente = false;
	//hasta aqui

	enviar_marco_a_disco(marco);

	return marco;
}

t_marco* asignar_marco(t_proceso* proceso)
{
	t_marco* marco = buscar_marco_libre_en_ram(proceso);

	if(no_hay_marco_libre(marco))
		marco = swap_out(proceso);

	actualizar_tabla_de_marcos(marco, proceso->pid, list_size(proceso->tabla_de_paginas));

	t_pagina* pagina = create_pagina(list_size(proceso->tabla_de_paginas), marco->num_marco);

	list_add(proceso->tabla_de_paginas, pagina);

	actualizar_TLB(pagina->num_pagina , proceso->pid , marco->num_marco);

	return marco;
}

void asignarle_marcos(t_proceso* proceso, int cantidad_a_asignar)
{
	for(int i=0; i<cantidad_a_asignar; i++)
	{
		asignar_marco(proceso);
	}

	//printf("no quedan marcos libres en memoria para el %d ", proceso->pid);

}

int start_process(int pid)
{
	pthread_mutex_lock(&mutexLogger);
	log_debug(LOGGER, "MATE_INIT iniciado del carpincho %d", pid);
	pthread_mutex_unlock(&mutexLogger);

	t_proceso* proceso = crear_proceso(pid);

	if(proceso==NULL)
		return -1;

	list_add(TABLA_GENERAL, proceso);

	if(string_equals_ignore_case(CONFIG_MEMORIA->TIPO_ASIGNACION, "FIJA"))
	{
		asignarle_marcos(proceso, CONFIG_MEMORIA->MARCOS_POR_CARPINCHO);
		asignar_primer_heapmetadata(proceso);

		for(int i=0, num_pagina=list_size(proceso->tabla_de_paginas); i<6; i++, num_pagina++)//despues borra esto una vez que este la logia de -1 de disco
		{
			//printf("numero pagina agregada %d \n", num_pagina);
			t_pagina* pagina = create_pagina(num_pagina, -1);
			pagina->esta_presente = false;
			list_add(proceso->tabla_de_paginas, pagina);
		}
	}

	pthread_mutex_lock(&mutexLogger);
	log_debug(LOGGER, "MATE_INIT iniciado del carpincho %d", pid);
	pthread_mutex_unlock(&mutexLogger);

	return 0;
}

void* siguiente_direccionn(t_list* paginas, int posicion)
{
	t_pagina* pagina = list_get(paginas, posicion);

	void* direccion_fisica = MI_MEMORIA + (pagina->marco_asignado * CONFIG_MEMORIA->TAMANIO_PAGINA + 0);

	return direccion_fisica;
}

t_marco* buscar_marco_libre_en_mmu(t_proceso* proceso)
{
	bool ff(void* un_marco)
	{
		t_marco* marco = un_marco;

		return ((marco->pid==proceso->pid) && marco->esta_libre);
	}


	t_marco* marco = list_find(TABLA_DE_MARCOS, ff);

	if(marco!=NULL)
		printf("buscar_memoria_libre #marco %d \n", marco->num_marco);
	if(marco==NULL)
		printf("buscar_memoria_libre no se encontromarco \n");

	return marco;
}

void* traer_n_bytes_de_disco(int pid, int num_pagina)
{

	//CONEXION_SWAMP = crear_conexion("127.0.0.1", "5001");

	t_paquete_swamp* paquete = crear_paquete(READ);

	agregar_entero_a_paquete(paquete, num_pagina);
	agregar_entero_a_paquete(paquete, pid);

	enviar_paquete(paquete, CONEXION_SWAMP);

	op_code recibir_operacion(int socket_cliente){
		int cod_op = -1;
		if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		{
			cod_op = -1;
		}

		return cod_op;
	}

	op_code op_code = recibir_operacion(CONEXION_SWAMP);

	int a;
	void* buffer = recibir_buffer(&a, CONEXION_SWAMP);
	void* cadena_bytes = recibir_nbytes(buffer);

	printf("traemos de disco la pagina de este proceeso");

	return cadena_bytes;
}

t_marco* swap_in(t_proceso* proceso, t_pagina* pagina)
{
	void* n_bytes = traer_n_bytes_de_disco(proceso->pid, pagina->num_pagina);
	char* nbytes = string_substring_until((char*)n_bytes, CONFIG_MEMORIA->TAMANIO_PAGINA);
	t_marco* marco = buscar_marco_libre_en_mmu(proceso);

	if(marco==NULL)
		marco = swap_out(proceso);

	memcpy(marco->inicio, (void*)nbytes, CONFIG_MEMORIA->TAMANIO_PAGINA);
	//actualizar_tabla_de_marcos(marco, proceso->pid, pagina->num_pagina)
	marco->pid = proceso->pid;
	marco->num_pagina = pagina->num_pagina;
	marco->esta_libre = false;

	pagina->esta_presente = true;
	pagina->marco_asignado = marco->num_marco;

	return marco;
}


t_heap_metadata* sacar_heap_metadata(t_proceso* proceso, int direccion_logica)
{
	t_heap_metadata* heap = malloc(sizeof(t_heap_metadata));
	int tam_del_heap = sizeof(t_heap_metadata);
	int espacio_disponible = espacio_restante(direccion_logica);
	int aux = 0;
	void* aqui = NULL;

	aqui = direccion_fisica_para_sacar(proceso, direccion_logica);
	while(tam_del_heap > espacio_disponible)
	{
		memcpy((void*)heap +aux, (void*)aqui, espacio_disponible);
		aux = aux + espacio_disponible;
		direccion_logica = direccion_logica + espacio_disponible;
		tam_del_heap = tam_del_heap - espacio_disponible;
		aqui = direccion_fisica_para_sacar(proceso, direccion_logica);
		espacio_disponible = CONFIG_MEMORIA->TAMANIO_PAGINA;
	}

	memcpy((void*)heap +aux, (void*)aqui, tam_del_heap);

	printf("heap ant: %d sig: %d estado %d     en %d \n", heap->prevAlloc, heap->nextAlloc, heap->isFree, direccion_logica);

	return heap;
}

t_pagina* primera_pagina(t_proceso* proceso)
{
	t_pagina* pagina = list_get(proceso->tabla_de_paginas, 0);

	return pagina;
}

void asignar_primer_marco(t_proceso* proceso)
{
	asignar_marco(proceso);
	asignar_primer_heapmetadata(proceso);
}

bool tiene_marcos(t_proceso* proceso)
{
	return (list_size(proceso->tabla_de_paginas)==0)? false : true;
}

bool NO_tiene_marcos_asignados(t_proceso* proceso)
{
	return !tiene_marcos(proceso);
}


int cabe_en_esa_pagina(int direccion, int size)
{
	int tamanio_libre = CONFIG_MEMORIA->TAMANIO_PAGINA - parte_usada(direccion);
	return tamanio_libre >= (size +9);
}

int calcular_paginas_a_usar(int direccion, int size)
{
	int tamanio_libre = CONFIG_MEMORIA->TAMANIO_PAGINA - parte_usada(direccion);

	int tamanio_nuevo = size +9 - tamanio_libre;

	int cant_paginas = tamanio_nuevo / CONFIG_MEMORIA->TAMANIO_PAGINA;

	if(tamanio_nuevo % CONFIG_MEMORIA->TAMANIO_PAGINA != 0)
		cant_paginas++;

	return cant_paginas;
}

int reserve_memory(int pid, int size)
{
	t_proceso* proceso = buscar_proceso(pid);
	t_heap_metadata* heap = NULL;
	t_heap_metadata* heap_aux = NULL;
	int inicio = 9;
	int final = 0;
	int tamanio_libre = 0;
	int direccion_logica = 0;

	if(proceso==NULL)
		log_error(LOGGER, "el proceso es null, no se puede reservar memoria");

	//asignar espacio dispnible para el heap en: dinamica.
	if(NO_tiene_marcos_asignados(proceso))//si esta vacio es por que la asignacion es dinamica y no tiene marcos asignados.
		asignar_primer_marco(proceso);

	heap = sacar_heap_metadata(proceso, 0);

	while(heap->nextAlloc != -1)
	{
		if(heap->isFree == true)
		{
			final = heap->nextAlloc;
			tamanio_libre = final - inicio;

			if(tamanio_libre >= size +9) //si tam_libre es == size +9 habra dos heapsseguidos.
			{
				heap->nextAlloc = inicio + size;
				heap->isFree = false;

				guardar_heap_metadata(proceso, heap, inicio -9);

				heap->prevAlloc = inicio -9;
				heap->nextAlloc = final;
				heap->isFree = true;

				guardar_heap_metadata(proceso, heap, inicio + size);

				heap_aux = heap;

				heap = sacar_heap_metadata(proceso, heap->nextAlloc);

				heap->prevAlloc = inicio + size;

				guardar_heap_metadata(proceso, heap, final);

				free(heap_aux);

				break;
			}

		}
		if((heap->isFree == false) || (tamanio_libre < size +9))
		{
			heap_aux = heap;
			inicio = heap->nextAlloc +9;// verificar si esto sigue teniendo el valor de inicio deseado
			heap = sacar_heap_metadata(proceso, heap->nextAlloc);
			free(heap_aux);
		}

	}

	if(heap->nextAlloc == -1) //le agregamos el size en el ultimo heap.
	{
		//despues ver si estos dos if se puede hacer con un while
		if(cabe_en_esa_pagina(inicio, size)) //si tam_libre es == size +9 habra unheap al final de pagina sin espacio libre.
		{
			heap->nextAlloc = inicio + size;
			heap->isFree = false;

			guardar_heap_metadata(proceso, heap, inicio -9);

			heap->prevAlloc = inicio -9;
			heap->nextAlloc = -1;
			heap->isFree = true;

			guardar_heap_metadata(proceso, heap, inicio + size);
		}
		else//hay que pedir mas paginas
		{
			if(string_equals_ignore_case(CONFIG_MEMORIA->TIPO_ASIGNACION, "DINAMICA"))
			{
				int cant_paginas = calcular_paginas_a_usar(inicio, size);

				for(int i=0; i<cant_paginas; i++)
				{
					asignar_marco(proceso);
				}
			}

			heap->nextAlloc = inicio + size;
			heap->isFree = false;

			guardar_heap_metadata(proceso, heap, inicio -9);

			heap->prevAlloc = inicio -9;
			heap->nextAlloc = -1;
			heap->isFree = true;

			guardar_heap_metadata(proceso, heap, inicio + size);
		}
	}

	free(heap);

	direccion_logica = inicio/* + size + 9*/;

	return direccion_logica;
}

bool la_direccion_es_valida(t_proceso* proceso, int direccion_logica)
{
	int direcciones_maxima = list_size(proceso->tabla_de_paginas) * CONFIG_MEMORIA->TAMANIO_PAGINA;

	return direcciones_maxima >= direccion_logica;
}

void si_hay_que_consolidar(t_proceso* proceso, t_heap_metadata* heap, int* direccion)
{
	t_heap_metadata* heap_sig = sacar_heap_metadata(proceso, heap->nextAlloc);
	t_heap_metadata* heap_ant = sacar_heap_metadata(proceso, heap->prevAlloc);

	if(heap_sig->isFree==true && heap_ant->isFree==true)
	{
		*direccion = heap->prevAlloc;
		heap->prevAlloc = heap_ant->prevAlloc;
		heap->nextAlloc = heap_sig->nextAlloc;
	}
	else
	{
		if(heap_sig->isFree==true)
		{
			heap->nextAlloc = heap_sig->nextAlloc;
		}
		if(heap_ant->isFree==true)
		{
			*direccion = heap->prevAlloc;
			heap->prevAlloc = heap_ant->prevAlloc;
		}
	}

}

void liberamos_marco(int num_marco)
{
	t_marco* marco = list_get(TABLA_DE_MARCOS, num_marco);

	marco->esta_libre = true;
}

void liberar_pagina(t_proceso* proceso, int num_pagina)
{
	t_pagina* pagina = list_get(proceso->tabla_de_paginas, num_pagina);

	//pagina-> ver que hacemos con las paginas del proceso



	liberamos_marco(pagina->marco_asignado);
}

void liberamos_paginas_libres(t_proceso* proceso, int inicio, int final)
{
	int pag_A = inicio/CONFIG_MEMORIA->TAMANIO_PAGINA;
	int pag_B = final/CONFIG_MEMORIA->TAMANIO_PAGINA;

	while(pag_A < pag_B -1)
	{
		liberar_pagina(proceso, pag_B -1); //que hacemos conla pagina liberada ?????????????? ver funcion
		pag_B = pag_B -1;
	}
}

int free_memory(int pid, int direccion_logica)
{

	t_proceso* proceso = buscar_proceso(pid);

	if(la_direccion_es_valida(proceso, direccion_logica))
	{
		pthread_mutex_lock(&mutexLogger);
		log_debug(LOGGER, "MATE_MEM_FREE iniciado del carpincho %d", pid);
		pthread_mutex_unlock(&mutexLogger);

		int direccion = direccion_logica -9;
		t_heap_metadata* heap = sacar_heap_metadata(proceso, direccion);
		heap->isFree = true;

		si_hay_que_consolidar(proceso, heap, &direccion);

		guardar_heap_metadata(proceso, heap, direccion);

		liberamos_paginas_libres(proceso, direccion +9, heap->nextAlloc);

		free(heap);
		pthread_mutex_lock(&mutexLogger);
		log_debug(LOGGER, "MATE_MEM_FREE del carpincho %d terminado", pid);
		pthread_mutex_unlock(&mutexLogger);
		return 0;
	}
	else
	{
		pthread_mutex_lock(&mutexLogger);
		log_error(LOGGER, "Error en el MATE_MEM_FREE del carpincho %d ", pid);
		pthread_mutex_unlock(&mutexLogger);

		//error_mems(_free_memory, "la_direccion_es_valida", "la direccion logica no es valida");
		return MATE_FREE_FAULT;

	}

}

int read_memory(int pid, int direccion_logica, void* destino, int tamanio)
{
	t_proceso* proceso = buscar_proceso(pid);
	int cant_a_leer = tamanio;
	int espacio_disponible = espacio_restante(direccion_logica);
	int aux = 0;

	if(la_direccion_es_valida(proceso, direccion_logica))
	{
		pthread_mutex_lock(&mutexLogger);
		log_debug(LOGGER, "MATE_MEM_READ iniciado del carpincho %d", pid);
		pthread_mutex_unlock(&mutexLogger);
		while(cant_a_leer > espacio_disponible)
		{
			memcpy(destino +aux, direccion_fisica_para_sacar(proceso, direccion_logica), espacio_disponible);
			cant_a_leer = cant_a_leer - espacio_disponible;
			direccion_logica = direccion_logica + espacio_disponible;
			aux = aux + espacio_disponible;
			espacio_disponible = CONFIG_MEMORIA->TAMANIO_PAGINA;
		}

		memcpy(destino +aux, direccion_fisica_para_sacar(proceso, direccion_logica), cant_a_leer);
		pthread_mutex_lock(&mutexLogger);
		log_debug(LOGGER, "MATE_MEM_READ terminado del carpincho %d", pid);
		pthread_mutex_unlock(&mutexLogger);
		return 0;
	}
	else
	{
		pthread_mutex_lock(&mutexLogger);
		log_error(LOGGER, "Error en el MATE_MEM_READ del carpincho %d", pid);
		pthread_mutex_unlock(&mutexLogger);
		return MATE_READ_FAULT;
		//error_mems(_read_memory, "la_direccion_es_valida", "la direccion logica no es valida");
	}
}

int write_memory(int pid, void* origen, int direccion_logica, int tamanio)
{
	t_proceso* proceso = buscar_proceso(pid);
	int espacio_disponible = espacio_restante(direccion_logica);
	int aux = 0;

	if(la_direccion_es_valida(proceso, direccion_logica))
	{
		pthread_mutex_lock(&mutexLogger);
		log_debug(LOGGER, "MATE_MEM_WRITE iniciado del carpincho %d", pid);
		pthread_mutex_unlock(&mutexLogger);
		while(tamanio > espacio_disponible)
		{
			memcpy(direccion_fisica_para_guardar(proceso, direccion_logica), origen +aux, espacio_disponible);
			tamanio = tamanio - espacio_disponible;
			direccion_logica = direccion_logica + espacio_disponible;
			aux = aux + espacio_disponible;
			espacio_disponible = CONFIG_MEMORIA->TAMANIO_PAGINA;
		}

		memcpy(direccion_fisica_para_guardar(proceso, direccion_logica), origen +aux, tamanio);
		pthread_mutex_lock(&mutexLogger);
		log_debug(LOGGER, "MATE_MEM_WRITE terminado del carpincho %d", pid);
		pthread_mutex_unlock(&mutexLogger);
		return 0;
	}
	else
	{
		pthread_mutex_lock(&mutexLogger);
		log_debug(LOGGER, "Error en el MATE_MEM_WRITE del carpincho %d", pid);
		pthread_mutex_unlock(&mutexLogger);
		return MATE_WRITE_FAULT;
		//error_mems(_write_memory, "la_direccion_es_valida", "la direccion logica no es valida");
	}
}

void bajar_proceso_a_disco(int pid){
	t_proceso* proceso = buscar_proceso(pid);
	for(int i=0; i < list_size(proceso->tabla_de_paginas); i++){
		t_pagina* pagina = list_get(proceso->tabla_de_paginas, i);
		if(pagina->esta_presente){
			t_marco *marco = list_get(TABLA_DE_MARCOS, pagina->marco_asignado);
			enviar_marco_a_disco(marco);
			marco->esta_libre = true;
			pagina->esta_presente = false;
		}
	}
}

void traer_proceso_entero_a_memoria(int pid){
	t_proceso* proceso = buscar_proceso(pid);
	printf("Hasta afuera del for. \n");
	for(int i=0; i < list_size(proceso->tabla_de_paginas); i++){
		t_pagina* pagina = list_get(proceso->tabla_de_paginas, i);
		swap_in(proceso, pagina);
	}
}




//funciones de prueba

void mostrar_taba_de_paginas(int pid)
{
	t_proceso* proceso = buscar_proceso(pid);

	printf("#pag  #marco  esta_presente \n");

	void ff(void* una_pagina)
	{
		t_pagina* pagina = una_pagina;

		printf("   %d    %d    %d  \n", pagina->num_pagina, pagina->marco_asignado, pagina->esta_presente);
	}

	list_iterate(proceso->tabla_de_paginas, ff);


}

void enviar_primer_marco_a_disco(int pid){
	t_marco* marco = list_get(TABLA_DE_MARCOS, 1);
	enviar_marco_a_disco(marco);
}

void close_process(int pid){
	t_proceso* proceso = buscar_proceso(pid);
	for(int i=0; i < list_size(proceso->tabla_de_paginas); i++){
		t_pagina* pagina = list_get(proceso->tabla_de_paginas, i);
		if(pagina->esta_presente){
			t_marco *marco = list_get(TABLA_DE_MARCOS, pagina->marco_asignado);
			marco->esta_libre = true;
		}
	}
	t_paquete_swamp* paquete = crear_paquete(DELETE);
	agregar_entero_a_paquete(paquete, pid);
	enviar_paquete(paquete, CONEXION_SWAMP);


}


