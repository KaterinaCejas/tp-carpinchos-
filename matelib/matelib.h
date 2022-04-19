#ifndef MATELIB_H_
#define MATELIB_H_

#include <stdint.h>
#include <stdint.h>
#include <commons/string.h>
#include <string.h>
#include <commons/config.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/log.h>
#include <unistd.h>


//-------------------Type Definitions----------------------/

typedef char *mate_io_resource;
typedef char *mate_sem_name;
typedef int32_t mate_pointer;


typedef struct mate_instance
{
    void *group_info;
} mate_instance;

enum mate_errors {
    MATE_FREE_FAULT = -5,
    MATE_READ_FAULT = -6,
    MATE_WRITE_FAULT = -7
};

typedef struct {
	char* ipBackend;
	int puertoBackend;
}t_configCarpincho;


typedef enum {
	MATE_INIT,
	MATE_CLOSE,
	MATE_SEM_INIT,
	MATE_SEM_WAIT,
	MATE_SEM_POST,
	MATE_SEM_DESTROY,
	MATE_MEMALLOC,
	MATE_MEMFREE,
	MATE_MEMREAD,
	MATE_MEMWRITE,
	MATE_CALL_IO,
	SUSPENSION,
	DES_SUSPENSION,
}t_tipoMensaje;

typedef struct {
	int idCarpincho;
	int socket;
}t_groupInfo;

typedef enum {
	CARPINCHO,
	KERNEL,
	MEMORIA,
	SERVIDOR_POLIMORFICO
}t_enviadoPor;

typedef struct {
	t_tipoMensaje tipoMensaje;
	int tamanioMensaje;
	t_enviadoPor cliente;
} t_infoMensaje;

typedef struct {
	t_infoMensaje header;
	void* mensaje;
}t_paquete;


// TODO: Docstrings

//------------------General Functions---------------------/
int mate_init(mate_instance *lib_ref, char*config);

int mate_close(mate_instance *lib_ref);

//-----------------Semaphore Functions---------------------/
int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value);

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem);

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem);

int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem);

//--------------------IO Functions------------------------/

int mate_call_io(mate_instance *lib_ref, mate_io_resource io, void *msg);

//--------------Memory Module Functions-------------------/

mate_pointer mate_memalloc(mate_instance *lib_ref, int size);

int mate_memfree(mate_instance *lib_ref, mate_pointer addr);

int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size);

int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size);

int recibirMensaje(int socket, t_enviadoPor servidor, t_paquete * paquete);
int crearConexion(char* ip, int puerto);
void enviarMensaje(int socket, t_enviadoPor cliente, void* mensaje, int tamanioMensaje, t_tipoMensaje tipoMensaje);
void enviarPaquete(int socket, t_paquete* paquete);
int recibirMensaje(int socket, t_enviadoPor servidor, t_paquete * paquete);
int recibirDatos(void* paquete, int socket, uint32_t cantARecibir);



#endif
