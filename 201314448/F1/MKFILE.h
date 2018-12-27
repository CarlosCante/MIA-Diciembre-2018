#ifndef MKFILE_H_INCLUDED
#define MKFILE_H_INCLUDED
#include "Analizador.h"
#include "Estructuras.h"
#include "MKDIR.h"
#include "JOURNAL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void EjecutarMKFILE(char Path[], char P, char pSize[], char Cont[]);
void CrearArchivo(FILE *DISCO, SuperBloque *SB, int Padre, char Nombre[], char Size[], char Cont[]);
void EJecutarCAT(char Path[], int reporte, char Ruta[]);
void MostrarContenidoArchivo(FILE *DISCO, SuperBloque *SB, int Padre, char Nombre[]);
void GenerarReporteFile(FILE *DISCO, SuperBloque *SB, int Padre, char Nombre[], char Destino[]);


#endif // MKFILE_H_INCLUDED
