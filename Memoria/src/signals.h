#ifndef SIGNALS_H_
#define SIGNALS_H_

#include <stdint.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/temporal.h>
#include <commons/txt.h>
#include "logs.h"
#include "tlb.h"
#include "configMemoria.h"

typedef struct{
	int pid;
	int total;
} registro_carpincho;


typedef struct{
	int TLB_hits_totales;
	int TLB_miss_totales;
	t_list* TLB_hits_por_carpincho;
	t_list* TLB_miss_por_carpincho;
} metricas;

metricas* metrics;

pthread_mutex_t mutexHitMetrics;
pthread_mutex_t mutexMissMetrics;
pthread_mutex_t mutexMemClose;

void iniciar_metricas();
void hit_metrics(int pid);
void miss_metrics(int pid);
void sumar_o_crear_carpincho(int pid, t_list* lista);
void handle_SIGINT();
void handle_SIGUSR1();
void handle_SIGUSR2();
void cerrar_metrics();
void destroy_registro_carpincho(registro_carpincho* reg);

#endif /* SIGNALS_H_ */
