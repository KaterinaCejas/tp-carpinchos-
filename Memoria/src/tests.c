/*#include <stdio.h>
#include <stdlib.h>
#include "Memoria.h"
#include <unistd.h>
#include <signal.h>
#include "logs.h"
#include "signals.h"

void test_function(){
	printf("PRINTEO ALGO RE LOCO.");
	exit(1);

}

int main(void){
	leerConfig();
	iniciar_logger();

	//Pruebas TLB
	t_configMemoria* config = malloc(sizeof(t_configMemoria));
	char* algoritmo = string_new();
	string_append(&algoritmo, "FIFO");
	int size = 4;
	config->ALGORITMO_REEMPLAZO_TLB = algoritmo;
	config->CANTIDAD_ENTRADAS_TLB = size;
	config->RETARDO_ACIERTO_TLB = 0;
	config->RETARDO_FALLO_TLB= 0;
	char* path = string_new();
	string_append(&path, "/home/utnso/Escritorio");
	config->PATH_DUMP_TLB = path;

	iniciar_TLB(config);

	for(int i=0; i<3; i++){
		//printf("Pos %i: %i\n", i, i);
		llenar_TLB(i, i, i);
	}

	//int resultado = buscar_en_TLB(6, 6);
	buscar_en_TLB(2, 2);
	buscar_en_TLB(3, 3);
	buscar_en_TLB(3, 3);
	buscar_en_TLB(2, 2);
	buscar_en_TLB(6, 6);
	buscar_en_TLB(6, 6);
	buscar_en_TLB(6, 6);
	buscar_en_TLB(6, 6);

	handle_SIGINT();

	printf("Registros en TLB: %i", TLB->entradas->elements_count);

	signal(SIGUSR1, handle_SIGUSR1);


	return EXIT_SUCCESS;
}*/
