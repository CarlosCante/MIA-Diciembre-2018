#ifndef MOUNT_H_INCLUDED
#define MOUNT_H_INCLUDED
#include "Estructuras.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void EjecutarMOUNT(char pPath[], char pName[]);
void EjecutarUNMOUNT(char pID[]);
void InsertarLista(struct NodoParticion *nuevoaux);
int LetraParicion(char pPath[]);
int NumeroParticionMontada(char pPath[]);
void ImprimirLista();
int VerificarParticionMontada(char pPath[], char pName[]);



#endif // MOUNT_H_INCLUDED
