#ifndef FDISK_H_INCLUDED
#define FDISK_H_INCLUDED

void EjecutarFDISK(char pSize[], char pUnit, char pPath[], char pType[], char pFit[], char pDelete[], char pName[], char pAdd[]);

void CrearParticionPrimaria(char pPath[], int Tamanio, char pName[], char pFit[]);
void CrearParticionExtendida(char pPath[], int Tamanio, char pName[], char pFit[]);
void CrearParticionLogica(char pPath[], int Tamanio, char pName[], char pFit[]);

void EliminarParticion(char pPath[], char pName[]);

#endif // FDISK_H_INCLUDED
