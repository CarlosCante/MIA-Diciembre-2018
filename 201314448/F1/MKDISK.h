#ifndef MKDISK_H_INCLUDED
#define MKDISK_H_INCLUDED
#include "Estructuras.h"
#include <string.h>
#include <time.h>

void EJecutarMKDISK(char pSize[], char pUnit, char pPath[], char pFit[]);
void EjecutarRMDISK(char path[]);
void CargarMBR(int tamanio, char pPath[], char pFit[]);
void ProbarMBR(char pPath[]);
void CrearDirectorio(char *ruta);
char *FechaYHoraActual();

#endif // MKDISK_H_INCLUDED
