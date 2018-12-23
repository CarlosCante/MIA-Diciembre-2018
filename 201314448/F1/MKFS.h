#ifndef MKFS_H_INCLUDED
#define MKFS_H_INCLUDED
#include "Analizador.h"
#include "Estructuras.h"
#include "JOURNAL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void EjecutarMKFS(char pID[], char pType[]);
void CrearUsersTXT(FILE *DISCO, int InicioParticion);
void ReiniciarBitMaps(FILE *DISCO, int InicioParticion);
void ComprobarUsersTXT(FILE *DISCO, int InicioParticion);


#endif // MKFS_H_INCLUDED
