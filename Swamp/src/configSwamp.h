/*
 * config.h
 *
 *  Created on: 13 nov. 2021
 *      Author: utnso
 */

#ifndef CONFIGSWAMP_H_
#define CONFIGSWAMP_H_

#include <commons/config.h>
#include <commons/collections/list.h>
#include <stdint.h>
#include <string.h>
#include <commons/string.h>
#include "Swamp.h"

typedef struct {
	char* IP;
	char* PUERTO;
	int TAMANIO_SWAP;
	int TAMANIO_PAGINA;
	t_list* ARCHIVOS_SWAP;
	int MARCOS_POR_CARPINCHO;
	int RETARDO_SWAP;
}t_configSwamp;

t_configSwamp configSwamp;
#define PATH_CONFIG_SWAMP "../Salvame.config"


void leerConfig();




#endif /* CONFIGSWAMP_H_ */
