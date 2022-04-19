#include "configMemoria.h"

void inicializar_config()
{
	CONFIG_MEMORIA = malloc(sizeof(t_config_memoria)); //estructura config
	t_config* archivoConfig = config_create(PATH_CONFIG_MEMORIA);

	if(archivoConfig == NULL)
		printf("archivo_config con error\n");

	CONFIG_MEMORIA->IP_MEMORIA = string_duplicate(config_get_string_value(archivoConfig, "IP_MEMORIA"));
	CONFIG_MEMORIA->PUERTO_ESCUCHA = config_get_int_value(archivoConfig, "PUERTO");
	CONFIG_MEMORIA->TAMANIO = config_get_int_value(archivoConfig, "TAMANIO");
	CONFIG_MEMORIA->TAMANIO_PAGINA = config_get_int_value(archivoConfig, "TAMANIO_PAGINA");
	CONFIG_MEMORIA->ALGORITMO_REEMPLAZO_MMU = string_duplicate(config_get_string_value(archivoConfig, "ALGORITMO_REEMPLAZO_MMU"));
	CONFIG_MEMORIA->TIPO_ASIGNACION  = string_duplicate(config_get_string_value(archivoConfig, "TIPO_ASIGNACION"));
	CONFIG_MEMORIA->MARCOS_POR_CARPINCHO = config_get_int_value(archivoConfig, "MARCOS_POR_CARPINCHO");
	CONFIG_MEMORIA->CANTIDAD_ENTRADAS_TLB = config_get_int_value(archivoConfig, "CANTIDAD_ENTRADAS_TLB");
	CONFIG_MEMORIA->ALGORITMO_REEMPLAZO_TLB = string_duplicate(config_get_string_value(archivoConfig, "ALGORITMO_REEMPLAZO_TLB"));
	CONFIG_MEMORIA->RETARDO_ACIERTO_TLB = config_get_int_value(archivoConfig, "RETARDO_ACIERTO_TLB");
	CONFIG_MEMORIA->RETARDO_FALLO_TLB = config_get_int_value(archivoConfig, "RETARDO_FALLO_TLB");
	CONFIG_MEMORIA->PATH_DUMP_TLB = string_duplicate(config_get_string_value(archivoConfig, "PATH_DUMP_TLB"));
	CONFIG_MEMORIA->PUERTO_SWAMP = string_duplicate(config_get_string_value(archivoConfig, "PUERTO_SWAMP"));
	CONFIG_MEMORIA->IP_SWAMP = string_duplicate(config_get_string_value(archivoConfig, "IP_SWAMP"));

	printf("IP: %s \n", CONFIG_MEMORIA->IP_SWAMP);
	printf("PUERTO: %s\n", CONFIG_MEMORIA->PUERTO_SWAMP);

	config_destroy(archivoConfig);
}
