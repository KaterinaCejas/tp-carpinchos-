/*
 * conexiones.h
 *
 *  Created on: 30 oct. 2021
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include "logs.h"
//#include "laMateLib.h"
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>


typedef enum {
	MEMALLOC,
	MEMFREE,
	MEMREAD,
	MEMWRITE,
}op_code;

#endif /* CONEXIONES_H_ */
