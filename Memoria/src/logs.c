#include "logs.h"


void inicializar_logs(){
	if((LOGGER = log_create(PATH_LOGS,"MEMORIA",1,LOG_LEVEL_INFO )) == NULL){
		printf("No se pudo crear el log");
		exit(30);
	}
}

void log_TLB_Hit(int Pid,int numero_pagina,int marco){
	char* mensaje = string_new();
	string_append(&mensaje, "TLB_Hit PID=");
	string_append(&mensaje, string_itoa(Pid));
	string_append(&mensaje, " Numero de Pagina=");
	string_append(&mensaje, string_itoa(numero_pagina));
	string_append(&mensaje, " Marco=");
	string_append(&mensaje, string_itoa(marco));
	log_info(LOGGER,mensaje);
	free(mensaje);
}


void log_TLB_Miss(int Pid,int numero_pagina){
	char* mensaje = string_new();
	string_append(&mensaje, "TLB_Miss PID=");
	string_append(&mensaje, string_itoa(Pid));
	string_append(&mensaje, " Numero de Pagina=");
	string_append(&mensaje, string_itoa(numero_pagina));
	log_info(LOGGER,mensaje);
	free(mensaje);
}


void log_conexion(int id){
	char* mensaje = string_from_format("Se conectó correctamente el carpincho de id %d",id);

	log_info(LOGGER,mensaje);
	free(mensaje);
}

void log_desconexion(int id){
	char* mensaje = string_from_format("Se ha desconectado el carpincho de id %d",id);

	log_info(LOGGER,mensaje);
	free(mensaje);
}


void logs_reemplazo_entrada(int Pid, int numero_pagina, int marco, int Pid2, int numero_pagina2, int marco2){
	char* mensaje = string_new();
	string_append(&mensaje, " Reemplazo Victima PID=");
	string_append(&mensaje, string_itoa(Pid));
	string_append(&mensaje, " Numero de Pagina=");
	string_append(&mensaje, string_itoa(numero_pagina));
	string_append(&mensaje, " Marco=");
	string_append(&mensaje, string_itoa(marco));
	string_append(&mensaje, " Reemplazo Nuevo PID=");
	string_append(&mensaje, string_itoa(Pid2));
	string_append(&mensaje, " Numero de Pagina=");
	string_append(&mensaje, string_itoa(numero_pagina2));
	string_append(&mensaje, " Marco=");
	string_append(&mensaje, string_itoa(marco2));
	log_info(LOGGER,mensaje);
	free(mensaje);
}



