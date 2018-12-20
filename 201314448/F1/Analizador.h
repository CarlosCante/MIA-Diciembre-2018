#ifndef ANALIZADOR_H_INCLUDED
#define ANALIZADOR_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MKDISK.h"
#include "FDISK.h"
#include "MOUNT.h"
#include "REP.h"
#include "MKFS.h"
#include "LOGIN.h"
#include "ABC_GU.h"

int EjecutarComando(char Comando[]);
void ConvertirMayusculas(char cadena[]);
void Split(char Linea[], char *Destino[]);
void EjecutarArchivo(char* ruta);

#endif // ANALIZADOR_H_INCLUDED
