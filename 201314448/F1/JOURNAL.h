#ifndef JOURNAL_H_INCLUDED
#define JOURNAL_H_INCLUDED
#include "Estructuras.h"
#include "Analizador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


void CrearJournal(FILE *DISCO, int InicioParticion, int Cantidad_N);

void NuevoOperacionJournaling(FILE *DISCO, int InicioParticion);


void EjecutarLOSS(char pID[]);

void EjecutarRECOVERY(char pID[]);

#endif // JOURNAL_H_INCLUDED
