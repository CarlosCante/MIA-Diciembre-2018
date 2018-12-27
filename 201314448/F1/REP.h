#ifndef REP_H_INCLUDED
#define REP_H_INCLUDED
#include "Analizador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void EjecutarREP(char pName[], char pPath[], char pID[], char pRuta[]);
void ReporteMBR(char pPath[], char pID[]);
void ReporteDISK(char pPath[], char pID[]);

/**Reportes de la fase 2**/
void ReporteINODE(char pPath[], char pID[]);
void ReporteBM_INODE(char pPath[], char pID[]);
void ReporteBM_BLOCK(char pPath[], char pID[]);
void ReporteSB(char pPath[], char pID[]);

void ReporteTREE(char pPath[], char pID[]);
void ApuntarNodosTREE(FILE *DISCO, FILE *DOT, SuperBloque *SB, int AP_Inodo);

void ReporteBLOCK(char pPath[], char pID[]);
void RecorrerBloques(FILE *DISCO, FILE *DOT, SuperBloque *SB, int AP_Inodo);

void ReporteFILE(char pPath[], char pID[], char Ruta[]);
void ReporteJOURNALING(char pPath[], char pID[]);

#endif // REP_H_INCLUDED
