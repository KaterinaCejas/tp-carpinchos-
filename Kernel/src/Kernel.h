#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include <matelib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <semaphore.h>
#include <pthread.h>
#include <commons/temporal.h>

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCKED,
	SUSPENDED_BLOCKED,
	SUSPENDED_READY,
	EXIT,
}t_estado;

typedef struct {
	int socketCarpincho;
	int socketMemoria;
}t_conexion;

typedef struct {
	t_estado estado;
	int pid;
	float estimacionAnterior;
	int rafagaRealAnterior;
	float estimacionActual;
	int tiempoInicioExec;
	int tiempoSalidaExec;
	int tiempoDeEspera;
	int tiempoInicioReady;
	int tiempoSalidaReady;  //los cambie a int
	t_list* pedidosSEM;   //lista de mate_sem_name
	t_list* pedidosIO;    //lista de mate_io_resource
	int socket;
	int socketMemoria;
}t_pcbCarpincho;

typedef struct {
	char* ipKernel;
	char* ipMemoria;
	int puertoMemoria;
	int puertoEscucha;
	char* algoritmo;
	int estimacionInicial;
	int alfa;
	t_list* dispositivosIO; //lista de mate_io_resource
	t_list* duracionesIO;   //lista de int 
	int gradoMultiprogramacion;
	int gradoMultiprocesamiento;
	int tiempoDeadlock;
}t_configKernel;


typedef struct{
	mate_sem_name nombre;
	int recursosDisponibles;
	t_list* carpinchos;  //ES UNA LISTA DE INT, LOS ID DE LOS CARPINCHOS
}t_SEMAFORO;

typedef struct{
	mate_io_resource nombre;
	t_list* idCarpinchosEnEspera;
	int duracion;
	pthread_mutex_t mutexEditarDispositivoIO;
	sem_t carpinchoEnEspera;
	bool enUso;
}t_dispositivoIO;


t_list* listaNew;
t_list* listaReady;
t_list* listaExec;
t_list* listaBloq;
t_list* listaExit;
t_list* listaSuspendedBlocked;
t_list* listaSuspendedReady;

t_list* list_semaforos;
t_list* lista_carpinchos_deadlock;
t_list* listaGeneralTodosCarpinchos;
t_list* listaDispositivosIO;

#define PATH_CONFIG_KERNEL "../configKernel.config"
#define PATH_LOG_KERNEL "../logKernel.log"

bool SERVIDOR_CPU_ABIERTO;

t_log* loggerKernel;
t_configKernel configKernel;

//sem_t mutexLoggerKernel;
sem_t contadorMultiprogramacion;
sem_t contadorMultiprocesamiento;

sem_t ordenCortoPlazo;
sem_t ordenLargoPlazo;

pthread_mutex_t mutexLoggerKernel;

pthread_mutex_t mutexNew;
pthread_mutex_t mutexReady;
pthread_mutex_t mutexExec;
pthread_mutex_t mutexBloq;
pthread_mutex_t mutexSuspendedBlocked;
pthread_mutex_t mutexSuspendedReady;
pthread_mutex_t mutexExit;

pthread_mutex_t mutexListaSemaforos;
pthread_mutex_t mutexListaCarpinchosDeadlock;
pthread_mutex_t mutexListaIO;
pthread_mutex_t mutexTodosLosCarpinchos;


//funciones generales de arranque del kernel
void iniciarListas(void);
void iniciarSemaforosGenerales(void);
int crearHiloCPU(void);
void iniciarConfigYLog(void);
void extraerDatosConfig(t_config* archivoConfig);
void cerrarKernel(void);
void eliminarConfig(void);
void eliminarCarpincho(t_pcbCarpincho* carpincho);
void eliminarSemaforo(t_SEMAFORO* semaforo);
int iniciarHiloDeadlock(void);
void iniciarDispositivosIO(void);


//funciones de las conexiones con la matelib
int iniciarServidor(void);
void iniciarConexionCarpinchoMemoria(void);
void conexionConCarpincho(void* sockCli);
int iniciarCliente(void);

//funciones de planificacion
void FIFO(void* voidSocketMemoriaSuspension);
void SJF(void);
void HRRN(void);
void PlanifCortoPlazo(void);
void planiMedianoPlazoBloqASuspendedBloq(void* voidSocketMemoriaSuspension);
void desbloqueoIO(mate_sem_name dispositivoIO);
void moverDeLista(t_pcbCarpincho* carpincho,t_list * ListaActual,t_list * ListaDestino);
void calculoDeRafagaSJF(t_pcbCarpincho* carpincho);
void registrarRecursosEnListas(t_pcbCarpincho* carpincho,mate_sem_name SEM);
void registrarRecursoIOenLista(t_pcbCarpincho* carpincho,mate_io_resource IO);
void liberarRecursoIO(t_pcbCarpincho* carpincho,mate_io_resource IO);
void desBloquear(mate_sem_name sem);
bool pedidoPorElCarpincho(mate_sem_name sem,t_pcbCarpincho* carpincho);
bool ioPedidoPorElCarpincho(mate_io_resource io,t_pcbCarpincho* carpincho);
int kernel_sem_getvalueIO(mate_sem_name nombre);
void mapeoManualHRRN(t_list* lista);
void calculoDeRafagaHRRN(t_pcbCarpincho* carpincho);
void mapeoManualSJF(t_list* lista);
int temporalConversion(void);
void kernel_sem_post(mate_sem_name nombre);
int kernel_sem_getvalue(mate_sem_name nombre);
void kernel_sem_wait(mate_sem_name nombre);
void kernel_sem_waitIO(mate_sem_name nombre);
void kernel_sem_postIO(mate_sem_name nombre);

//funciones de deadlock
void registrarUsoDeSemaforo(mate_sem_name nombre, int carpinchoPID);
void liberarRecursosRegistrados(mate_sem_name nombre, int carpinchoPID);
void deteccionYrecuperacion(void);
void deteccion(int indice);
void eliminarElDeMayorID(void);
void matarPorDeadlock(int indice);
int buscarIndiceCarpinchos(t_list* lista, int numeroABuscar);
int buscarIndiceSemaforos(t_list* lista,mate_sem_name sem);
bool buscarEnLista(int idCarpincho);

//funciones de mateLibKernel
void crearEstructuraDeSemaforo(mate_sem_name sem, int recursos);
int mate_sem_init_kernel (t_paquete* paquete, int socketMemoria);
int mate_sem_wait_kernel (t_paquete* paquete);
int mate_sem_post_kernel (t_paquete* paquete);
int mate_init_kernel(t_paquete* paquete, int socketMemoria);
void mate_close_kernel(t_paquete* paquete, int socketMemoria);
int mate_sem_destroy_kernel(t_paquete* paquete);
int mate_call_io_kernel(t_paquete* paquete);
mate_pointer mate_memalloc_kernel(t_paquete* paquete, int socketMemoria);
bool existeSemaforo(mate_sem_name nombreSem);
void removerSemDeListaGeneral(mate_sem_name sem);
t_pcbCarpincho* encontrarCarpincho(int idCarpincho);
int generarPID(void);
int mate_memfree_kernel(t_paquete* paquete, int socketMemoria);
int mate_memread_kernel(t_paquete* paquete, int socketMemoria);
int mate_memwrite_kernel(t_paquete* paquete, int socketMemoria);
t_dispositivoIO* encontrarDispositivoIO(mate_io_resource nombreIO);
int buscarPosicionIOEnLista(mate_io_resource nombreIO);
void eliminarDispositivoIO(t_dispositivoIO* dispo);
void ejecutarIO(t_pcbCarpincho* carpincho, t_dispositivoIO* dispositivoIO, char* msg);



#endif
