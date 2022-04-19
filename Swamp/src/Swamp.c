#include "Swamp.h"

int main(void) {
	configSwamp.IP = string_new();
	configSwamp.PUERTO = string_new();
	configSwamp.ARCHIVOS_SWAP = list_create();
	leerConfig();
	archivos_swap = configSwamp.ARCHIVOS_SWAP;
	iniciar_logger();


	swamp_list = list_create();
	iniciar_swamp();

	/*
	char* ip = string_new();
	string_append(&ip,configSwamp.IP);
	printf("%s",ip);
	int marcos = configSwamp.MARCOS_POR_CARPINCHO;
	printf("%d",marcos);
	char* path1 = string_new();
	string_append(&path1,list_get(configSwamp.ARCHIVOS_SWAP,0));
	printf("%s",path1);
	*/
	/*
	SwampMan* memory2 = malloc(sizeof(SwampMan));
	memory2->PID = 1;
	memory2->numero_pagina_principal = 3;
	memory2->numero_pagina_swap = 0;
	memory2->swap_file = 1;
	memory2->esta_libre = false;
	SwampMan* memory3 = malloc(sizeof(SwampMan));
	memory3->PID = 2;
	memory3->numero_pagina_principal = 4;
	memory3->numero_pagina_swap = 2;
	memory3->swap_file = 1;
	memory3->esta_libre = false;
	void* remplazo = list_replace(swamp_list,  0, memory2);
	void* remplazo3 = list_replace(swamp_list,  2, memory3);


	char* entrada2 = string_new();
	char* entrada5 = string_new();
	char* entrada4 = string_new();
	char* entrada3 = string_new();
	char* entrada = string_new();
	string_append(&entrada2,"44444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444");
	string_append(&entrada,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	string_append(&entrada3,"cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
	string_append(&entrada4,"55555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555");
	string_append(&entrada5,"mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm");


	write_in_swap(45,entrada,1);
	write_in_swap(69,entrada2,5);
	write_in_swap(77,entrada3,6);
	write_in_swap(46,entrada,1);
	write_in_swap(47,entrada,1);
	write_in_swap(48,entrada,1);
	write_in_swap(70,entrada2,5);
	write_in_swap(78,entrada3,6);
	write_in_swap(79,entrada3,6);
	delete_in_swap(1);
	delete_in_swap(6);
	delete_in_swap(5);


	int k = 0;
	int j = 0;
	int i = 0;
	k = write_in_swap(45,entrada,1);
	k = write_in_swap(46,entrada2,2);
	k = write_in_swap(53,entrada3,3);
	k = write_in_swap(54,entrada4,4);
	k = write_in_swap(47,entrada,1);
	k = write_in_swap(48,entrada2,2);
	k = write_in_swap(55,entrada3,3);
	k = write_in_swap(56,entrada4,4);
	k = write_in_swap(49,entrada,1);
	k = write_in_swap(50,entrada2,2);
	k = write_in_swap(57,entrada3,3);
	k = write_in_swap(58,entrada4,4);
	k = write_in_swap(51,entrada,1);
	k = write_in_swap(52,entrada2,2);
	k = write_in_swap(59,entrada3,3);
	k = write_in_swap(60,entrada4,4);
	/*
	k = write_in_swap(45,entrada5,1);
	k = write_in_swap(46,entrada5,2);
	k = write_in_swap(53,entrada5,3);
	k = write_in_swap(54,entrada5,4);
	k = write_in_swap(47,entrada5,1);
	k = write_in_swap(48,entrada5,2);
	k = write_in_swap(55,entrada5,3);
	k = write_in_swap(56,entrada5,4);
	k = write_in_swap(49,entrada5,1);
	k = write_in_swap(50,entrada5,2);
	k = write_in_swap(57,entrada5,3);
	k = write_in_swap(58,entrada5,4);
	k = write_in_swap(51,entrada5,1);
	k = write_in_swap(52,entrada5,2);
	k = write_in_swap(59,entrada5,3);
	k = write_in_swap(60,entrada5,4);


	char* jaja = string_new();
	jaja = read_in_swap(51,1);
	char* jaja2 = string_new();
	jaja2 = read_in_swap(52,2);


	delete_in_swap(1);
	delete_in_swap(3);
	delete_in_swap(2);
	delete_in_swap(4);

	printf("%s\n",jaja);
	printf("%s",jaja2);

	j = list_size(configSwamp.ARCHIVOS_SWAP);
	i = list_size(archivos_swap);
	printf("%d",j);
	printf("%d",i);
	//int test_reserva = permite_reservar(1);
	//int test_reserva2 = permite_reservar(2);
	//printf("%d",test_reserva);
	//printf("%d",test_reserva2);
	//list_destroy(configSwamp.ARCHIVOS_SWAP);

	 */

	iniciar_servidor();


	return EXIT_SUCCESS;
}


void iniciar_swamp(){


	int cantidad_paginas = configSwamp.TAMANIO_SWAP / configSwamp.TAMANIO_PAGINA;

	int tamanio_swap = configSwamp.TAMANIO_SWAP;

	int tamanio_lista = list_size(archivos_swap);
	for(int i = 0 ; i < tamanio_lista  ; i++){

		char* aux = string_new();

		string_append(&aux, list_get(archivos_swap,i));

		FILE *f = fopen(aux,"w");


		int respuesta = ftruncate(fileno(f),tamanio_swap);
		if(respuesta == 0){
			printf("Archivo swap creado correctamente %s\n",(char*)list_get(archivos_swap,i));
		}
		else{
			printf("Archivo swap no pudo ser creado correctamente %s\n",(char*)list_get(archivos_swap,i));
		}
		fclose(f);
		free(aux);
	}
	for(int j = 0 ; j < tamanio_lista; j++){

		for(int i = 0; i < cantidad_paginas ;i++ ){
				SwampMan* memory = malloc(sizeof(SwampMan));
				memory->PID = -1;
				memory->numero_pagina_swap = i;
				memory->numero_pagina_principal = -1;
				memory->swap_file = j+1;
				memory->esta_libre = true;
				list_add(swamp_list, memory);
			}
	}
}

int write_in_swap(int numero_pagina,void* bytes,int pid){
	char* bytes_char = (char*)bytes;
	int cantidad_paginas = configSwamp.TAMANIO_SWAP / configSwamp.TAMANIO_PAGINA;
	int posicion = -1;
	int file_proceso = corresponde_a_file(pid);
	char* asignacion = string_new();
	string_append(&asignacion,"FIJA");
	int pagina_swap;
	printf("Asignacion final: %s \n\n\n\n", asignacion_final);
	if(string_equals_ignore_case(asignacion_final,"FIJA")){
		if(file_proceso == -1){
			file_proceso = archivo_mas_libre();
			pagina_swap = primer_espacio_libre_no_reservado(file_proceso);
			posicion = posicion_pagina_swap(pagina_swap);
			int reserva = permite_reservar(file_proceso);
			if(reserva == 1){
			reservar_marcos(((file_proceso-1)*cantidad_paginas)+pagina_swap,pid);
			}
			else{
			pagina_swap = -1;
			}
		}
		else{
			pagina_swap = buscar_pagina(numero_pagina,pid);
			if(pagina_swap == -1){
			int ubicacion_marcos_reserva = posicion_pid(pid,file_proceso);
			pagina_swap = siguiente_reserva_libre(ubicacion_marcos_reserva,file_proceso);
			posicion = posicion_pagina_swap(pagina_swap);
			}
			else{
			//AGREGAR RETURN DE NO ENCONTRAR RESERVA LIBRE
			posicion = posicion_pagina_swap(pagina_swap);
			}
		}
	}
	else{
		//ASIGNACION GLOBAL
		if(file_proceso == -1){
			file_proceso = archivo_mas_libre();
			pagina_swap = primer_espacio_libre(file_proceso);
			posicion = posicion_pagina_swap(pagina_swap);
		}
		else{
			pagina_swap = buscar_pagina(numero_pagina,pid);
			if(pagina_swap == -1){
				pagina_swap = libre_battle_royale(file_proceso);
				posicion = posicion_pagina_swap(pagina_swap);
			}
			else{
				posicion = posicion_pagina_swap(pagina_swap);
			}
		}
	}
	if(pagina_swap ==-1){
		return -1;
	}
	size_t tamanio = configSwamp.TAMANIO_SWAP;
	char* file_path = string_new();
	int tamanio_pagina = configSwamp.TAMANIO_PAGINA;
	string_append(&file_path,list_get(archivos_swap,file_proceso-1));
	int fd = open(file_path,O_RDWR, S_IRUSR| S_IWUSR);
	char* maped = mmap(NULL,tamanio, PROT_READ|PROT_WRITE, MAP_SHARED,fd,0);
	/*
	//LO DEJAMOS EN CHAR* VER QUE ONDA CON VOID*
	for(int i = 0 ; i < tamanio_pagina ; i++ ){
		maped[posicion] = bytes_char[i];
		posicion++;
	}
	*/
	memcpy(maped + configSwamp.TAMANIO_PAGINA * pagina_swap, bytes_char, configSwamp.TAMANIO_PAGINA);
	int error = munmap(maped,tamanio);
	if(error != 0){
		printf("Mapping failed");
	}
	close(fd);
	int elemento = ((file_proceso-1)*cantidad_paginas)+pagina_swap;
	SwampMan* memory = list_get(swamp_list,elemento);
	memory->PID = pid;
	memory->numero_pagina_principal= numero_pagina;
	memory->esta_libre = false;
	free(file_path);
	free(asignacion);
	return 1;
}

void* read_in_swap(int numero_pagina,int pid){
	size_t tamanio = configSwamp.TAMANIO_SWAP;
	char* file_path = string_new();
	int pagina_swap = buscar_pagina(numero_pagina,pid);
	int numero_archivo = corresponde_a_file(pid);
	int posicion = posicion_pagina_swap(pagina_swap);
	int tamanio_pagina = configSwamp.TAMANIO_PAGINA;
	if(numero_archivo == -1){
		printf("PID no ubicado");
		return "error";
	}
	else{
		string_append(&file_path,list_get(archivos_swap,numero_archivo-1));
		int fd = open(file_path,O_RDWR, S_IRUSR| S_IWUSR);
		char* maped = mmap(NULL,tamanio, PROT_WRITE, MAP_SHARED,fd,0);
		char* bytes = malloc(tamanio);
		for(int i = 0 ; i < tamanio_pagina ; i++ ){
			bytes[i] = maped[posicion];
			posicion++;
		}
		char* bytes_truncados = string_new();
		bytes_truncados = string_substring_until(bytes, configSwamp.TAMANIO_PAGINA);
		int error = munmap(maped,tamanio);
		if(error != 0){
			printf("Mapping failed");
			free(bytes);
			free(bytes_truncados);
			free(file_path);
			return "error";
		}
		else{
			free(file_path);
			free(bytes);
			return bytes_truncados;
		}
	}
}


void delete_in_swap(int pid){
	int cantidad_paginas = configSwamp.TAMANIO_SWAP / configSwamp.TAMANIO_PAGINA;
	int file = corresponde_a_file(pid);
	int salida = (file-1)*cantidad_paginas;
	for(int i = salida; i < salida+cantidad_paginas; i++){
			SwampMan* memory = list_get(swamp_list,i);
			if(memory->PID == pid){
				int pagina = memory->numero_pagina_swap;
				clean_pagina(pagina,pid);
				memory->PID = -1;
				memory->numero_pagina_principal = -1;
				memory->esta_libre = true;
			}
	}
}


void write_page(int pagina_swap,void* bytes,int pid){
	char* bytes_char;
	bytes_char = (char *)bytes;
	size_t tamanio = configSwamp.TAMANIO_SWAP;
	char* file_path = string_new();
	int posicion = posicion_pagina_swap(pagina_swap);
	int numero_archivo = corresponde_a_file(pid);
	int tamanio_pagina = configSwamp.TAMANIO_PAGINA;
	if(numero_archivo == -1){
		printf("PID no ubicado");
	}
	else{
		string_append(&file_path,list_get(archivos_swap,numero_archivo-1));
		int fd = open(file_path,O_RDWR, S_IRUSR| S_IWUSR);
		char* maped = mmap(NULL,tamanio, PROT_READ|PROT_WRITE, MAP_SHARED,fd,0);
		for(int i = 0 ; i < tamanio_pagina ; i++ ){
			//string_append(&bytes,maped);
			maped[posicion] = bytes_char[i];
			posicion++;
		}
		//LO DEJAMOS EN CHAR* VER QUE ONDA CON VOID*
		//memcpy(maped + configSwamp.TAMANIO_PAGINA*pagina_swap,bytes_char, configSwamp.TAMANIO_PAGINA);
		int error = munmap(maped,tamanio);
		if(error != 0){
			printf("Mapping failed");
		}
		close(fd);
	}
	free(file_path);
}
