#ifndef MV_H_INCLUDED
#define MV_H_INCLUDED
#include "Analizador.h"
#include "Estructuras.h"
#include "MKDIR.h"
#include "JOURNAL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void EjecutarMV(char Path[], char Dest[]);
int MoverInodo(FILE *DISCO, SuperBloque *SB, int Hijo, char Dest[]);
void AgregarAPadre(FILE *DISCO, SuperBloque *SB, int Padre, int Hijo);

#endif // MV_H_INCLUDED
