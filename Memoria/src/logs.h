#ifndef LOGS_H_
#define LOGS_H_

#define PATH_LOGS "../logs.txt"

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>

t_log* LOGGER;

void inicializar_logs();
void log_TLB_Hit(int Pid,int numero_pagina,int marco);
void log_TLB_Miss(int Pid,int numero_pagina);
void log_conexion(int id);
void log_desconexion(int id);
void logs_reemplazo_entrada(int Pid, int numero_pagina, int marco, int Pid2, int numero_pagina2, int marco2);


#endif /* LOGS_H_ */
