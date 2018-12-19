#ifndef REP_H_INCLUDED
#define REP_H_INCLUDED
#include "Analizador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void EjecutarREP(char pName[], char pPath[], char pID[]);
void ReporteMBR(char pPath[], char pID[]);
void ReporteDISK(char pPath[], char pID[]);

#endif // REP_H_INCLUDED
