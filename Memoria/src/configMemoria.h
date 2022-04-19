#ifndef CONFIGMEMORIA_H_
#define CONFIGMEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>

#define PATH_CONFIG_MEMORIA "../configMemoria.config"

typedef struct {
	char* IP_MEMORIA;
	int PUERTO_ESCUCHA;
	int TAMANIO;
	int TAMANIO_PAGINA;
	char* TIPO_ASIGNACION;
	char* ALGORITMO_REEMPLAZO_MMU;
	int MARCOS_POR_CARPINCHO;
	int CANTIDAD_ENTRADAS_TLB;
	char* ALGORITMO_REEMPLAZO_TLB;
	int RETARDO_ACIERTO_TLB;
	int RETARDO_FALLO_TLB;
	char* PATH_DUMP_TLB;
	char* PUERTO_SWAMP;
	char* IP_SWAMP;
}t_config_memoria;

t_config_memoria* CONFIG_MEMORIA; //para tener todos los datos de la config


void inicializar_config();

#endif
