#ifndef MKDIR_H_INCLUDED
#define MKDIR_H_INCLUDED
#include "Analizador.h"
#include "Estructuras.h"
#include "JOURNAL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void EjecutarMKDIR(char Path[], char P);
int ExisteHijo(FILE *DISCO, SuperBloque *SB, int Padre, char Nombre[]);
int CrearCarpeta(FILE *DISCO, SuperBloque *SB, int Padre, char Nombre[]);
int BuscarPosicionInodo(FILE *DISCO, SuperBloque *SB, int Cantidad);
int BuscarPosicionBloques(FILE *DISCO, SuperBloque *SB, int Cantidad);

int VerificarPermisos(FILE *DISCO, SuperBloque *SB, int AP_Inodo, char Operacion);


#endif // MKDIR_H_INCLUDED
