#ifndef SWAMP_H_
#define SWAMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "configSwamp.h"
#include <stdbool.h>
#include <unistd.h>
#include "manejo.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "conexiones.h"
#include "logger.h"



typedef struct {
	int PID;
	int numero_pagina_swap;
	int numero_pagina_principal;
	int swap_file;
	bool esta_libre;
}  SwampMan;

void iniciar_swamp();
int write_in_swap(int numero_pagina,void* bytes,int pid);
void* read_in_swap(int numero_pagina,int pid);
void delete_in_swap(int pid);
void write_page(int pagina_swap,void* bytes,int pid);


t_list* swamp_list;
t_list* archivos_swap;
char* asignacion_final;

#endif
