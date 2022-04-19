/*
 * manejo.c
 *
 *  Created on: 13 nov. 2021
 *      Author: utnso
 */
#include "manejo.h"
#include "Swamp.h"

void set_asignacion(char *asignacion){

	asignacion_final = asignacion;
}


int tamanio_array(char **array){
	int i=0;
	if(!array[i]) return 0;
	while(array[i] != NULL){
		i++;
	}
	return i;
}

bool estaLibre(int numero_pagina, int file){
	int index_pagina = numero_pagina*file;
	SwampMan* memory = list_get(swamp_list,index_pagina);
	return memory->esta_libre;
}


int buscar_pagina(int numero_pagina_principal, int pid){
	int cantidad_paginas = configSwamp.TAMANIO_SWAP / configSwamp.TAMANIO_PAGINA;
	int file = corresponde_a_file(pid);
	int salida = (file-1)*cantidad_paginas;
	for(int i = salida; i < salida+cantidad_paginas; i++){
			SwampMan* memory = list_get(swamp_list,i);
			if(memory->numero_pagina_principal == numero_pagina_principal){
				int numero_pagina_swap = memory->numero_pagina_swap;
				return numero_pagina_swap;
			}
}
	return -1;
}

int posicion_pagina_swap(int numero_pagina_swap){
	return numero_pagina_swap*configSwamp.TAMANIO_PAGINA;
}


int corresponde_a_file(int pid){
	for(int i = 0; i < list_size(swamp_list); i++){
		SwampMan* memory = list_get(swamp_list,i);
		if(memory->PID == pid){
			int swap_file = memory->swap_file;
			return swap_file;
		}
}
	return -1;
}

void clean_pagina(int numero_pagina, int pid){
	char* bytes = string_new();
	//for(int i = 0; i < configSwamp.TAMANIO_PAGINA; i++){
		//string_append(&bytes,"");
	//	string_append(&bytes,"\0");
	//}
	bytes = string_repeat('\0',configSwamp.TAMANIO_PAGINA);
	write_page(numero_pagina,bytes,pid);
}


bool es_nuevo(int pid){
	if(corresponde_a_file(pid)==-1){
		return true;
	}
return false;
}


int archivo_mas_libre(){
	int aux = 0;
	int file = -1;
	for(int i = 0; i < list_size(archivos_swap); i++){
		int espacios_libres = espacios_libres_archivo(i+1);
		if(espacios_libres > aux){
			aux = espacios_libres;
			file = i+1;
		}
	}
	return file;
}


int espacios_libres_archivo(int file){
	int cantidad_paginas = configSwamp.TAMANIO_SWAP / configSwamp.TAMANIO_PAGINA;
	int arranca_file = (file-1)*cantidad_paginas;
	int espacios_libres = 0;
	for(int i = arranca_file; i < arranca_file+cantidad_paginas ; i++){
		SwampMan* memory = list_get(swamp_list,i);
		if(memory->esta_libre == true){
			espacios_libres++;
		}
	}
	return espacios_libres;
}

void reservar_marcos(int posicion, int pid){
	int marcos_max = configSwamp.MARCOS_POR_CARPINCHO;
	for(int i = 0; i < marcos_max;i++ ){
	SwampMan* memory = list_get(swamp_list,posicion+i);
	memory->PID = pid;
}
}


int libre_battle_royale(int file){
	int libre = -1;
	int cantidad_paginas = configSwamp.TAMANIO_SWAP / configSwamp.TAMANIO_PAGINA;
	int arranca_file = (file-1)*cantidad_paginas;
	for(int i = arranca_file; i < arranca_file+cantidad_paginas ; i++){
		SwampMan* memory = list_get(swamp_list,i);
			if(memory->esta_libre == true){
				libre = memory->numero_pagina_swap;
				return libre;
			}
		}
	return libre;
}

int permite_reservar(int file){
	int libre = -1;
	int cantidad_paginas = configSwamp.TAMANIO_SWAP / configSwamp.TAMANIO_PAGINA;
	int marcos_max = configSwamp.MARCOS_POR_CARPINCHO;
	int arranca_file = (file-1)*cantidad_paginas;
	for(int i = arranca_file; i < arranca_file+cantidad_paginas ; i++){
		SwampMan* memory = list_get(swamp_list,i);
		if(memory->PID == -1){
			if(i > arranca_file + cantidad_paginas - marcos_max){
				return -1;
			}
			for(int j = 1; j < marcos_max; j++){
				SwampMan* memory2 = list_get(swamp_list,i+j);
				if(memory2->PID != -1){
					break;
				}
			}
			return 1;
		}
	}
	return 0;
}


int primer_espacio_libre(int file){
	int cantidad_paginas = configSwamp.TAMANIO_SWAP / configSwamp.TAMANIO_PAGINA;
	int arranca_file = (file-1)*cantidad_paginas;
	for(int i = arranca_file; i < arranca_file+cantidad_paginas ; i++){
		SwampMan* memory = list_get(swamp_list,i);
		if(memory->esta_libre){
			return memory->numero_pagina_swap;
		}
	}
	return -1;
}

int primer_espacio_libre_no_reservado(int file){
	int cantidad_paginas = configSwamp.TAMANIO_SWAP / configSwamp.TAMANIO_PAGINA;
	int arranca_file = (file-1)*cantidad_paginas;
	for(int i = arranca_file; i < arranca_file+cantidad_paginas ; i++){
		SwampMan* memory = list_get(swamp_list,i);
		if(memory->esta_libre == true && memory->PID == -1){
			return memory->numero_pagina_swap;
		}
	}
	return -1;
}

int siguiente_reserva_libre(int posicion,int file){
	int cantidad_paginas = configSwamp.TAMANIO_SWAP / configSwamp.TAMANIO_PAGINA;
	int posicion_ajustada = posicion+(file-1)*cantidad_paginas;
	int marcos_max = configSwamp.MARCOS_POR_CARPINCHO;
	for(int j = 0; j<marcos_max;j++){
		SwampMan* memory = list_get(swamp_list,posicion_ajustada+j);
		if(memory->esta_libre == true){
			int pagina_swap = memory->numero_pagina_swap;
			return pagina_swap;
		}
	}
	return -1;
}

int posicion_pid(int pid,int file){
	int cantidad_paginas = configSwamp.TAMANIO_SWAP / configSwamp.TAMANIO_PAGINA;
	int arranca_file = (file-1)*cantidad_paginas;
	for(int i = arranca_file; i < arranca_file+cantidad_paginas ; i++){
		SwampMan* memory = list_get(swamp_list,i);
		if(memory->PID == pid){
			return memory->numero_pagina_swap;
		}
	}
	return -1;
}


