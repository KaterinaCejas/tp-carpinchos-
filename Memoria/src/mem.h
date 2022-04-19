#ifndef MEM_H_
#define MEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <commons/string.h>
#include <commons/error.h>
#include <commons/collections/list.h>
#include "cliente_conexion.h"
#include "logs.h"

#include "Memoria.h"
int puntero_clock;


int start_process(int pid);
int reserve_memory(int pid, int size);
int free_memory(int pid, int direccion_logica);
int read_memory(int pid, int direccion_logica, void* destino, int tamanio);
int write_memory(int pid, void* origen, int direccion_logica, int tamanio);
void delete_carpincho(int idCarpincho);
void mostrar_taba_de_paginas(int pid);
void bajar_proceso_a_disco(int pid);
void traer_proceso_entero_a_memoria(int pid);
void close_process(int pid);
void enviar_primer_marco_a_disco(int pid);

#endif
