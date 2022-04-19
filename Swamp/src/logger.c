/*
 * logger.c
 *
 *  Created on: 18 nov. 2021
 *      Author: utnso
 */


#include "logger.h"


void iniciar_logger(){
	char* path = "../logger.log";
	if((logger = log_create(path,"SWAMP",1,LOG_LEVEL_INFO )) == NULL){
		printf("No se pudo crear el log");
		exit(1);
	}
}

