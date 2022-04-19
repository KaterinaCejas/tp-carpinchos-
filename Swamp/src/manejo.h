/*
 * manejo.h
 *
 *  Created on: 13 nov. 2021
 *      Author: utnso
 */

#ifndef MANEJO_H_
#define MANEJO_H_

#include <stdio.h>
#include <commons/string.h>
#include <sys/stat.h>
#include <string.h>
#include <commons/collections/list.h>
#include <stdint.h>


void set_asignacion(char* asignacion);
int tamanio_array(char **array);
bool estaLibre(int numero_pagina, int file);
int posicion_pagina(int numero_pagina);
int corresponde_a_file(int posicion_pagina);
void clean_pagina(int numero_pagina, int pid);
bool es_nuevo(int pid);
int espacios_libres_archivo(int file);
int buscar_pagina(int numero_pagina_principal, int pid);
int primer_espacio_libre(int file);
void reservar_marcos(int posicion, int pid);
int libre_battle_royale(int file);
int permite_reservar(int file);
int primer_espacio_libre(int file);
int siguiente_reserva_libre(int posicion,int file);
int posicion_pid(int pid,int file);
int primer_espacio_libre_no_reservado(int file);
int posicion_pagina_swap(int numero_pagina_swap);
int archivo_mas_libre();

#endif /* MANEJO_H_ */
