#include "Memoria.h"

void mostrar_memoria();

int main(void) {

	iniciar_memoria();

	//iniciarServidorKernel();

	//iniciarConexionPorCarpinchoConKernel();
	pthread_t hiloServidorKernel;
	//pthread_t hiloServidorMateLib;
	pthread_create(&hiloServidorKernel, NULL, (void*) iniciarConexionPorCarpinchoConKernel, NULL);
	pthread_join(hiloServidorKernel, NULL);
	//pthread_create(&hiloServidorMateLib, NULL, (void*) iniciarConexionSoloConMateLib, NULL);


//	pthread_join(hiloServidorKernel, NULL);
	//pthread_join(hiloServidorMateLib, NULL);


	//	char* palabra = malloc(23);

/*	start_process(1);
	int alloc0_1 = reserve_memory(1, 23);

	int alloc1_1 = reserve_memory(1, 23);

	int alloc2_1 = reserve_memory(1, 23);

	//mostrar_memoria();


	int alloc3_1 = reserve_memory(1, 10);



	//mostrar_memoria();
	printf("Linea 43. \n\n\n");
	write_memory(1, "aaaaaaaaaaaaaaa", alloc0_1, 16);
	write_memory(1, "bbbbbbbb", alloc1_1, 9);
	write_memory(1, "cccccccccc", alloc2_1, 11);
	write_memory(1, "dddd", alloc3_1, 5);
	//mostrar_memoria();
	printf("Linea 52. \n\n\n");

	//mostrar_memoria();
	mostrar_taba_de_paginas(1);
	//bajar_proceso_a_disco(1);
	enviar_primer_marco_a_disco(1);
	mostrar_taba_de_paginas(1);
	//traer_proceso_entero_a_memoria(1);

	close_process(1);
	//printf("Empieza a traer de disco \n");
	//traer_proceso_entero_a_memoria(1);
	printf("Adios. \n");

	//int alloc6_1 = reserve_memory(1, 10);

	mostrar_taba_de_paginas(1);
	/*
	alloc0_1 = reserve_memory(1, 23);

	alloc1_1 = reserve_memory(1, 23);

	alloc2_1 = reserve_memory(1, 23);

*/
	//mostrar_memoria();


	//alloc3_1 = reserve_memory(1, 10);
//	signal(SIGINT, handle_SIGINT);
//	signal(SIGUSR1, handle_SIGUSR1);
//	signal(SIGUSR2, handle_SIGUSR2);


	//void* prueba1 = traer_n_bytes_de_disco(2, 1);
	//char* prueba2 = string_substring_until((char*)prueba1, CONFIG_MEMORIA->TAMANIO_PAGINA);
	//printf("BYTES 2 : %s \n", prueba2);

	//void* prueba2 = traer_n_bytes_de_disco(2, 2);



	//mostrar_memoria();


	//bajar_proceso_a_disco(1);
	//mostrar_taba_de_paginas(1);
	/*
	mostrar_memoria();
	printf("Memoria con el proceso bajado a disco. \n");



	mostrar_memoria();
	printf("Memoria con el proceso traido de disco. \n");
	*/

	return EXIT_SUCCESS;

}
void iniciar_conexion_con_swamp()
{
	CONEXION_SWAMP = crear_conexion(CONFIG_MEMORIA->IP_SWAMP, CONFIG_MEMORIA->PUERTO_SWAMP);

	if(CONEXION_SWAMP==-1)
		printf("nose pudo conectar con el swamp");

	t_paquete_swamp* paquete = crear_paquete(ASIGNACION);
	agregar_cadena_a_paquete(paquete, CONFIG_MEMORIA->TIPO_ASIGNACION);
	enviar_paquete(paquete, CONEXION_SWAMP);

	/*CONEXION_SWAMP = crear_conexion("127.0.0.1", "5001");
	t_paquete* paquete2 = crear_paquete(ASIGNACION2);
	agregar_entero_a_paquete(paquete2, 2);
	agregar_entero_a_paquete(paquete2, 5);
	enviar_paquete(paquete2, CONEXION_SWAMP);
	*/
}

void reservamos_espacio_general()
{
	MI_MEMORIA = malloc(CONFIG_MEMORIA->TAMANIO);

	if(MI_MEMORIA == NULL) //abortar_memoria("error al crear espacio de direcciones de memoria\n");
		exit(-20);
}

void dividimos_la_memoria_en_marcos() //nos servira mas adelante para enviar marcos al disco.
{
	TABLA_DE_MARCOS = list_create();

	int cant_marcos = CONFIG_MEMORIA->TAMANIO/CONFIG_MEMORIA->TAMANIO_PAGINA;

	for(int i=0; i<cant_marcos; i++)
	{
		t_marco* marco = malloc(sizeof(t_marco));

		marco->inicio = MI_MEMORIA + i*(CONFIG_MEMORIA->TAMANIO_PAGINA);
		marco->num_marco = i;
		marco->num_pagina = -1;
		marco->pid = -1;
		marco->esta_libre = true;
		marco->uso = false;
		marco->modificado = false;
		//marco->final = la_memoria + (i+1)*(CONFIG_MEMORIA->TAMANIO_PAGINA);
		list_add(TABLA_DE_MARCOS, marco);
	}
}

void creamos_tabla_general()
{
	TABLA_GENERAL = list_create();
}

void inicializar_memoria()
{
	reservamos_espacio_general();

	dividimos_la_memoria_en_marcos();

	creamos_tabla_general();

	iniciar_conexion_con_swamp();
}

void iniciar_memoria()
{
	puntero_clock = 0;

	inicializar_config();
	iniciarSemaforos();
	inicializar_memoria();
	iniciar_TLB(CONFIG_MEMORIA);
	inicializar_logs();


}

void iniciarSemaforos(void){
	pthread_mutex_init(&mutexLogger, NULL);
	pthread_mutex_init(&mutexHitMetrics, NULL);
	pthread_mutex_init(&mutexMissMetrics, NULL);
	pthread_mutex_init(&mutexMemClose, NULL);
	pthread_mutex_init(&mutexTLB, NULL);
}

void mostrar_memoria()
{
	printf("=====================================================================\n");
	/*	printf("#marco  pid  #pagina  libre  uso  modificado\n");

	void ff(void* un_marco)
	{
		t_marco* marco = un_marco;

		printf("\"%d\"     \"%d\"   \"%d\"     \"%d\"    \"%d\"   \"%d\"   \n",
		marco->num_marco, marco->pid, marco->num_pagina, marco->esta_libre, marco->uso, marco->modificado);
	}

	list_iterate(TABLA_DE_MARCOS, ff); */
	printf("mostrar_memoria\n");
	printf("#m   pid #pag   libre     last_use            uso     modificado  \n");
	for(int i=0; i<list_size(TABLA_DE_MARCOS); i++)
	{
		t_marco* marco = list_get(TABLA_DE_MARCOS, i);
		printf("%d    %d      %d     %d     %s       %d    %d  \n", marco->num_marco, marco->pid, marco->num_pagina, (int)marco->esta_libre, marco->last_use, (int)marco->uso, (int)marco->modificado);
	}
	printf("=====================================================================\n");
}





