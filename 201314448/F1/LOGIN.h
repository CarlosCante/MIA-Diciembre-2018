#ifndef LOGIN_H_INCLUDED
#define LOGIN_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Estructuras.h"

void EjecutarLogin(char Usuario[], char Pass[], char pID[]);
void VerificarUsuario(FILE *DISCO, int InicioParticion,char Usuario[], char Pass[], char Path[]);

#endif // LOGIN_H_INCLUDED
