/*
 * config.c
 *
 *  Created on: 13 nov. 2021
 *      Author: utnso
 */
#include "configSwamp.h"


void leerConfig(){

	t_config* archivoConfig =  config_create(PATH_CONFIG_SWAMP);
	if (archivoConfig == NULL){
		printf("archivoConfig es NULL");
		exit(100);
	}
	string_append(&configSwamp.IP,config_get_string_value(archivoConfig, "IP"));
	string_append(&configSwamp.PUERTO, config_get_string_value(archivoConfig, "PUERTO"));
	configSwamp.TAMANIO_SWAP = config_get_int_value(archivoConfig, "TAMANIO_SWAP");
	configSwamp.TAMANIO_PAGINA  = config_get_int_value(archivoConfig, "TAMANIO_PAGINA");
	char ** array_string = string_array_new();
	array_string  = config_get_array_value(archivoConfig, "ARCHIVOS_SWAP");
	int size_array_string = string_array_size(array_string);
	for(int i = 0; i < size_array_string; i++){
		char* file = string_new();
		string_append(&file,array_string[i]);
		list_add(configSwamp.ARCHIVOS_SWAP,file);

	}
	string_array_destroy(array_string);
	configSwamp.MARCOS_POR_CARPINCHO = config_get_int_value(archivoConfig, "MARCOS_POR_CARPINCHO");
	configSwamp.RETARDO_SWAP = config_get_int_value(archivoConfig, "RETARDO_SWAP");
	config_destroy(archivoConfig);
}
