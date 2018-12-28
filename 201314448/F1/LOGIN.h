#ifndef LOGIN_H_INCLUDED
#define LOGIN_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Estructuras.h"
#include "MKDISK.h"

void EjecutarLogin(char Usuario[], char Pass[], char pID[]);
int VerificarUsuario(FILE *DISCO, int InicioParticion,char Usuario[], char Pass[], char Path[], char ajuste);
int ObtenerIDGrupo(char name[]);

#endif // LOGIN_H_INCLUDED
