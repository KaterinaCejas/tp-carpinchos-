#include "signals.h"


//Inicio SIGINT
void iniciar_metricas(){
	metrics = malloc(sizeof(metricas));
	metrics->TLB_hits_por_carpincho = list_create();
	metrics->TLB_miss_por_carpincho = list_create();
	metrics->TLB_hits_totales = 0;
	metrics->TLB_miss_totales = 0;
}

void miss_metrics(int pid){
	//Sumo total de miss
	pthread_mutex_lock(&mutexMissMetrics);
	metrics->TLB_miss_totales++;

	//Sumo miss por carpincho
	sumar_o_crear_carpincho(pid, metrics->TLB_miss_por_carpincho);
	pthread_mutex_unlock(&mutexMissMetrics);
}

void hit_metrics(int pid){
	//Sumo total de hits
	pthread_mutex_lock(&mutexHitMetrics);
	metrics->TLB_hits_totales++;

	//Sumo hit por carpincho
	sumar_o_crear_carpincho(pid, metrics->TLB_hits_por_carpincho);
	pthread_mutex_unlock(&mutexHitMetrics);
}

//Revisa la lista para ver si existe el carpincho, de existir le suma 1, si no lo agrega a la lista
void sumar_o_crear_carpincho(int pid, t_list* lista){
	for(int i = 0; i < lista->elements_count; i++){
		registro_carpincho* carpincho = list_get(lista, i);
		if(carpincho->pid == pid){
			carpincho->total++;
			return;
		}
	}
	registro_carpincho* nuevoCarpincho = malloc(sizeof(registro_carpincho));
	nuevoCarpincho->pid = pid;
	nuevoCarpincho->total = 1;
	list_add(lista, nuevoCarpincho);

}

void handle_SIGINT(){
	char* mensaje = string_new();
	//TLB Hits totales
	string_append(&mensaje, "Cantidad de TLB Hits totales: ");
	string_append(&mensaje, string_itoa(metrics->TLB_hits_totales));
	string_append(&mensaje, "\n");

	//TLB Hits por carpincho
	for(int i = 0; i < metrics->TLB_hits_por_carpincho->elements_count; i++){
		registro_carpincho* carpincho = list_get(metrics->TLB_hits_por_carpincho, i);
		string_append(&mensaje, "Hits de carpincho ");
		string_append(&mensaje, string_itoa(carpincho->pid));
		string_append(&mensaje, ": ");
		string_append(&mensaje, string_itoa(carpincho->total));
		string_append(&mensaje, "\n");

	}


	//TLB Miss totales
	string_append(&mensaje, "Cantidad de TLB Miss totales: ");
	string_append(&mensaje, string_itoa(metrics->TLB_miss_totales));
	string_append(&mensaje, "\n");

	//TLB Miss por carpincho
	for(int i = 0; i < metrics->TLB_miss_por_carpincho->elements_count; i++){
		registro_carpincho* carpincho = list_get(metrics->TLB_miss_por_carpincho, i);
		string_append(&mensaje, "Miss de carpincho ");
		string_append(&mensaje, string_itoa(carpincho->pid));
		string_append(&mensaje, ": ");
		string_append(&mensaje, string_itoa(carpincho->total));
		string_append(&mensaje, "\n");

	}

	log_info(LOGGER,mensaje);
	free(mensaje);
}

//Fin SIGINT

//Inicio SIGUSR1


void armar_mensaje_SIGURS1(tlb* TLB, FILE* dump){
	char* mensaje = string_new();
	string_append(&mensaje, "Dump: ");
	string_append(&mensaje, temporal_get_string_time("%d/%m/%y %H:%M:%S"));
	string_append(&mensaje, "\n");

	txt_write_in_file(dump, mensaje);

//	char* entrada;
	entrada_tlb* entrada_tlb;
	int i;
	for(i = 0; i< TLB->entradas->elements_count; i++){
		entrada_tlb = list_get(TLB->entradas, i);
		mensaje = string_new();
		string_append(&mensaje, "Entrada: ");
		string_append(&mensaje, string_itoa(i));
		string_append(&mensaje, "\n");
		string_append(&mensaje, "Estado: Ocupado");
		string_append(&mensaje, "\n");
		string_append(&mensaje, "Carpincho: ");
		string_append(&mensaje, string_itoa(entrada_tlb->pid));
		string_append(&mensaje, "\n");
		string_append(&mensaje, "Pagina: ");
		string_append(&mensaje, string_itoa(entrada_tlb->nroPagina));
		string_append(&mensaje, "\n");
		string_append(&mensaje, "Marco: ");
		string_append(&mensaje, string_itoa(entrada_tlb->nroFrame));
		string_append(&mensaje, "\n");
		string_append(&mensaje, "--------------\n");
		txt_write_in_file(dump, mensaje);
	}

	for(int j = i; j < TLB->size; j++){
		mensaje = string_new();
		string_append(&mensaje, "Entrada: ");
		string_append(&mensaje, string_itoa(j));
		string_append(&mensaje, "\n");
		string_append(&mensaje, "Estado: Libre \n");
		string_append(&mensaje, "Carpincho: - \n");
		string_append(&mensaje, "Pagina: - \n");
		string_append(&mensaje, "Marco: - \n");
		string_append(&mensaje, "--------------\n");
		txt_write_in_file(dump, mensaje);
	}

}

void handle_SIGUSR1(){
	char* path_completo = string_new();
	string_append(&path_completo, CONFIG_MEMORIA->PATH_DUMP_TLB);
	string_append(&path_completo, "/Dump_");
	string_append(&path_completo, temporal_get_string_time("%d-%m-%y-%H:%M:%S"));
	string_append(&path_completo, ".tlb");


	FILE* dump = txt_open_for_append(path_completo);
	armar_mensaje_SIGURS1(TLB, dump);
	txt_close_file(dump);
	free(path_completo);
	free(dump);
}
//Fin SIGURS1

//Inicio SIGURS2

void handle_SIGUSR2(){
	limpiar_entradas_TLB();
}

//Fin SIGURS2

void cerrar_metrics(){
	list_destroy_and_destroy_elements(metrics->TLB_hits_por_carpincho, (void*) destroy_registro_carpincho);
	/*int i;
	registro_carpincho* registro;

	for(i=0; i< metrics->TLB_hits_por_carpincho->elements_count; i++){
		registro = list_get(metrics->TLB_hits_por_carpincho, i);
		list_remove_and_destroy_element(metrics->TLB_hits_por_carpincho, i, destroy_registro_carpincho);
	}

	for(i=0; i< metrics->TLB_miss_por_carpincho->elements_count; i++){
		registro = list_get(metrics->TLB_miss_por_carpincho, i);
		list_remove_and_destroy_element(metrics->TLB_miss_por_carpincho, i, destroy_registro_carpincho);
	}

	free(metrics);*/
}

void destroy_registro_carpincho(registro_carpincho* registro){
	free(registro);
}


