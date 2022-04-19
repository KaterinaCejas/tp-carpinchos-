#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdint.h>
#include "configMemoria.h"
#include "logs.h"
#include "tlb.h"
#include "signals.h"
#include <signal.h>
#include "mem.h"
#include "cliente_conexion.h"
#include <unistd.h>
#include <matelib.h>
#include <pthread.h>

pthread_mutex_t mutexLogger;

typedef struct {
	void* inicio;
	uint32_t num_marco;
	uint32_t pid;
	uint32_t num_pagina;
	char* last_use;
	bool esta_libre;
	bool uso;
	bool modificado;
} t_marco;

typedef struct {
	uint32_t num_pagina;
	uint32_t marco_asignado;
	bool esta_presente;//si esta o no en memoria
	bool esta_modificado;//pf
} t_pagina;

typedef struct {
	uint32_t prevAlloc;//la direccion logica del heap_metada anterior
	uint32_t nextAlloc;//la direccion logica del heap_metada siguiente
	uint8_t isFree;
} t_heap_metadata;

typedef struct {
	uint32_t pid;
	t_list* tabla_de_paginas; //una lista de  --> t_paginas
} t_proceso;

void* MI_MEMORIA;
int CONEXION_SWAMP;
t_list* TABLA_DE_MARCOS; //para facilitar el encuentro de marcos libres y para el envio de marcos al disco
t_list* TABLA_GENERAL; //tabla_general a la que se debe ir abuscar cuando no se encuentre en la TLB

void iniciar_memoria();
void iniciarSemaforos(void);


//conexiones con la matelib y el kernel
int iniciarConexionPorCarpinchoConKernel(void);
void conexionConCarpincho(void* conexionVoid);
void iniciarServidorKernel(void);
void iniciarServidorMateLib(void);
int iniciarConexionSoloConMateLib(void);
void conexionConMateLib(void* voidSocketMateLib);


#endif
