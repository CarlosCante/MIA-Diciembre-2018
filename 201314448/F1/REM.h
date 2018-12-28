#ifndef REM_H_INCLUDED
#define REM_H_INCLUDED
#include "Analizador.h"
#include "Estructuras.h"
#include "MKDIR.h"
#include "JOURNAL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void EjecutarREM(char Path[]);
void EliminarInodoYBloques(FILE *DISCO, SuperBloque *SB, int AP_Inodo);
void EliminarRefernciaPadre(FILE *DISCO, SuperBloque *SB, int Padre, int Hijo);



#endif // REM_H_INCLUDED
