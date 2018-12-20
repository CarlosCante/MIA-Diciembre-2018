#ifndef ABC_GU_H_INCLUDED
#define ABC_GU_H_INCLUDED
#include "Estructuras.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void EjecutarMKGRP(char name[]);
void CrearGrupo(char name[]);
int VerificarSiGrupoExiste(char name[]);
int ObtenerBloqueLibre(FILE *DISCO, SuperBloque *SB, int cantidad, char Ajuste);
int ObtenerPrimerBloqueLibre(FILE *DISCO, SuperBloque *SB);

#endif // ABC_GU_H_INCLUDED
