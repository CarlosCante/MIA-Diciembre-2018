#include "FDISK.h"
#include "Estructuras.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void EjecutarFDISK(char pSize[], char pUnit, char pPath[], char pType[], char pFit[], char pDelete[], char pName[], char pAdd[])
{
    int SeEjecuta = 1;

    int Tamanio = atoi(pSize);
    int Unidades;
    int ADD = 0;

    /**Se verifica si el size es mayor a 0**/

    if(Tamanio <= 0)
    {
        printf("EL valor de SIZE es menor que 0 para el disco: %s\n", pPath);
        SeEjecuta = 0;
    }

    /**Se verifica el parametro UNIT que contenga un valor correcto**/

    if(pUnit == 'k' || pUnit == 'K' || pUnit == 'N')
    {
        Unidades = 1024;
    }
    else if(pUnit == 'm' || pUnit == 'M')
    {
        Unidades = 1024*1024;
    }
    else if(pUnit == 'b' || pUnit == 'B')
    {
        Unidades = 1;
    }
    else
    {
        printf("El valor ingresado en UNIT no es permitido, disco: %s\n", pPath);
        SeEjecuta = 0;
    }

    /**Se verifica el parametro TYPE que contenga un valor correcto**/

    if(!(pType[0] == 'p' || pType[0] == 'P' || pType[0] == 'e' || pType[0] == 'E' || pType[0] == 'l' || pType[0] == 'L'))
    {
        printf("El valor ingreado en TYPE no es permitido. disco: %s\n", pPath);
        SeEjecuta = 0;
    }

    /**Se verifica el parametro FIT que contenga un valor correcto**/

    if(!((pFit[0] == 'B' && pFit[1] == 'F')  || (pFit[0] == 'F' && pFit[1] == 'F') || (pFit[0] == 'W' && pFit[1] == 'F') || (pFit[0] == 'N' && pFit[1] == 'F')))
    {
        printf("El valor ingreado en FIT no es permitido. disco: %s\n", pPath);
        SeEjecuta = 0;
    }

    if(pName[0] == '\0')
    {
        printf("No se deficnio el NAME de la particion con la cual trabajar, disco: %s\n", pPath);
        SeEjecuta = 0;
    }

    if(pAdd[0] != '\0')
    {
        ADD = atoi(pAdd);
    }

    if(SeEjecuta == 1)
    {
        FILE *F;
        F = fopen(pPath,"r+b");
        if(F != NULL)
        {
            fclose(F);

            if(pDelete[0] == '\0' && ADD == 0) /**Si entra aca es que no vinieron esos 2 por lo que es una creacion de particion**/
            {
                if(pType[0] == 'p' || pType[0] == 'P')
                {
                    CrearParticionPrimaria(pPath, Tamanio * Unidades, pName, pFit);
                }
                else if(pType[0] == 'e' || pType[0] == 'E')
                {
                    CrearParticionExtendida(pPath, Tamanio * Unidades, pName, pFit);
                }
                else if(pType[0] == 'l' || pType[0] == 'L')
                {
                    CrearParticionLogica(pPath, Tamanio * Unidades, pName, pFit);
                }
            }
            else if(pDelete[0] != '\0')
            {
                EliminarParticion(pPath, pName);
            }
        }
        else
        {
            printf("El disco \"%s\" no se pudo abrir, probablemente no exista\n", pPath);
        }
    }

}



void CrearParticionPrimaria(char pPath[], int Tamanio, char pName[], char pFit[])
{
    int CooPart = -1;

    FILE *F;
    F = fopen(pPath,"r+b");


    struct MBR *MBRtmp = (struct MBR*)malloc(sizeof(struct MBR));
    fseek(F, 0, SEEK_SET);
    fread(MBRtmp, sizeof(MBR), 1, F);


    for(int i = 0 ; i < 4 ; i++)
    {
        if(MBRtmp->Particiones[i].Status == '0')
        {
            CooPart = i;
            break;
        }
    }

    if(CooPart != -1)   /**Si hay espacio para crear otra particion**/
    {
        int PosInicial = 0;

        int coordenadas[6];
        int coordenadasFinales[6];

        coordenadas[0] = sizeof(MBR);
        coordenadas[1] = MBRtmp->Tamanio;
        coordenadas[2] = -1;
        coordenadas[3] = -1;
        coordenadas[4] = -1;
        coordenadas[5] = -1;

        coordenadasFinales[0] = sizeof(MBR);
        coordenadasFinales[1] = MBRtmp->Tamanio;
        coordenadasFinales[2] = -1;
        coordenadasFinales[3] = -1;
        coordenadasFinales[4] = -1;
        coordenadasFinales[5] = -1;

        for(int i = 0 ; i < 4 ; i++)
        {

            if(MBRtmp->Particiones[i].Status == '1')
            {

                if(MBRtmp->Particiones[i].Inicio < coordenadas[1] || coordenadas[1] == -1)
                {
                    coordenadas[4] = coordenadas[3];
                    coordenadas[3] = coordenadas[2];
                    coordenadas[2] = coordenadas[1];
                    coordenadas[1] = MBRtmp->Particiones[i].Inicio;

                    coordenadasFinales[5] = coordenadasFinales[4];
                    coordenadasFinales[4] = coordenadasFinales[3];
                    coordenadasFinales[3] = coordenadasFinales[2];
                    coordenadasFinales[2] = coordenadasFinales[1];
                    coordenadasFinales[1] = MBRtmp->Particiones[i].Inicio + MBRtmp->Particiones[i].Tamanio;
                }
                else if(MBRtmp->Particiones[i].Inicio < coordenadas[2] || coordenadas[2] == -1)
                {
                    coordenadas[4] = coordenadas[3];
                    coordenadas[3] = coordenadas[2];
                    coordenadas[2] = MBRtmp->Particiones[i].Inicio;

                    coordenadasFinales[5] = coordenadasFinales[4];
                    coordenadasFinales[4] = coordenadasFinales[3];
                    coordenadasFinales[3] = coordenadasFinales[2];
                    coordenadasFinales[2] = MBRtmp->Particiones[i].Inicio + MBRtmp->Particiones[i].Tamanio;
                }
                else if(MBRtmp->Particiones[i].Inicio < coordenadas[3] || coordenadas[3] == -1)
                {
                    coordenadas[4] = coordenadas[3];
                    coordenadas[3] = MBRtmp->Particiones[i].Inicio;

                    coordenadasFinales[5] = coordenadasFinales[4];
                    coordenadasFinales[4] = coordenadasFinales[3];
                    coordenadasFinales[3] = MBRtmp->Particiones[i].Inicio + MBRtmp->Particiones[i].Tamanio;
                }
                else if(MBRtmp->Particiones[i].Inicio < coordenadas[4] || coordenadas[4] == -1)
                {
                    coordenadas[4] = MBRtmp->Particiones[i].Inicio;

                    coordenadasFinales[5] = coordenadasFinales[4];
                    coordenadasFinales[4] = MBRtmp->Particiones[i].Inicio + MBRtmp->Particiones[i].Tamanio;
                }
            }
        }

        /**Segun el ajuste que se aplicara se busca un espacio**/

        PosInicial = -1;
        int TamanioActual = 0;

        if(MBRtmp->Ajuste == 'b' || MBRtmp->Ajuste == 'B')/********************Mejor ajuste***********************/
        {
            TamanioActual = MBRtmp->Tamanio;

            for(int i = 0 ; i < 5 ; i++)
            {
                if(Tamanio <= coordenadas[i+1] - coordenadasFinales[i])
                {
                    if(coordenadas[i+1] - coordenadasFinales[i] < TamanioActual)
                    {
                        PosInicial = coordenadasFinales[i];
                        TamanioActual = coordenadas[i+1] - coordenadasFinales[i];
                    }
                }
            }
        }
        else if(MBRtmp->Ajuste == 'f' || MBRtmp->Ajuste == 'F')/********************Primer ajuste***********************/
        {

            for(int i = 0 ; i < 5 ; i++)
            {
                if(Tamanio <= coordenadas[i+1] - coordenadasFinales[i])
                {
                    PosInicial = coordenadasFinales[i];
                    break;
                }
            }

        }
        else if(MBRtmp->Ajuste == 'W' || MBRtmp->Ajuste == 'W')/********************Peor ajuste***********************/
        {
            TamanioActual = 0;

            for(int i = 0 ; i < 5 ; i++)
            {
                if(Tamanio <= coordenadas[i+1] - coordenadasFinales[i])
                {
                    if(coordenadas[i+1] - coordenadasFinales[i] > TamanioActual)
                    {
                        PosInicial = coordenadasFinales[i];
                        TamanioActual = coordenadas[i+1] - coordenadasFinales[i];
                    }
                }
            }
        }


        /**Se agregan los atributos de la particion**/

        if(PosInicial != -1)
        {
            MBRtmp->Particiones[CooPart].Status = '1';
            if(pFit[0] == 'N')
            {
                MBRtmp->Particiones[CooPart].Ajuste = 'W';
            }
            else
            {
                MBRtmp->Particiones[CooPart].Ajuste = pFit[0];
            }
            MBRtmp->Particiones[CooPart].Tipo = 'P';
            MBRtmp->Particiones[CooPart].Inicio = PosInicial;
            MBRtmp->Particiones[CooPart].Tamanio = Tamanio;
            strcpy(MBRtmp->Particiones[CooPart].Nombre,pName);

            fseek(F, 0, SEEK_SET);
            fwrite(MBRtmp,sizeof(struct MBR),1,F);
            printf("Se creo la particion primaria \"%s\" en el disco: %s\n\n", pName, pPath);
        }
        else
        {
            printf("No hay espacio dentro del disco para crear la particion primaria, disco: %s.\n\n", pPath);
        }

    }
    else
    {
        printf("Las particiones del disco ya suman 4, no se puede crear otra particion primaria, disco: %s\n\n", pPath);
    }

    fclose(F);

    free(MBRtmp);

}


void CrearParticionExtendida(char pPath[], int Tamanio, char pName[], char pFit[])
{
    int CooPart = -1;

    FILE *F;
    F = fopen(pPath,"r+b");


    struct MBR *MBRtmp = (struct MBR*)malloc(sizeof(struct MBR));
    fseek(F, 0, SEEK_SET);
    fread(MBRtmp, sizeof(MBR), 1, F);

    int HayExtendida = 0;

    for(int i = 0 ; i < 4 ; i++)
    {
        if(MBRtmp->Particiones[i].Tipo == 'E')
        {
            HayExtendida = 1;
            break;
        }
    }


    if(HayExtendida == 0)
    {
        for(int i = 0 ; i < 4 ; i++)
        {
            if(MBRtmp->Particiones[i].Status == '0')
            {
                CooPart = i;
                break;
            }
        }

        if(CooPart != -1)   /**Si hay espacio para crear otra particion**/
        {
            int PosInicial = 0;

            int coordenadas[5];
            int coordenadasFinales[6];

            coordenadas[0] = sizeof(MBR);
            coordenadas[1] = MBRtmp->Tamanio;
            coordenadas[2] = -1;
            coordenadas[3] = -1;
            coordenadas[4] = -1;
            coordenadas[5] = -1;

            coordenadasFinales[0] = sizeof(MBR);
            coordenadasFinales[1] = MBRtmp->Tamanio;
            coordenadasFinales[2] = -1;
            coordenadasFinales[3] = -1;
            coordenadasFinales[4] = -1;
            coordenadasFinales[5] = -1;

            for(int i = 0 ; i < 4 ; i++)
            {
                if(MBRtmp->Particiones[i].Status == '1')
                {

                if(MBRtmp->Particiones[i].Inicio < coordenadas[1] || coordenadas[1] == -1)
                {
                    coordenadas[4] = coordenadas[3];
                    coordenadas[3] = coordenadas[2];
                    coordenadas[2] = coordenadas[1];
                    coordenadas[1] = MBRtmp->Particiones[i].Inicio;

                    coordenadasFinales[5] = coordenadasFinales[4];
                    coordenadasFinales[4] = coordenadasFinales[3];
                    coordenadasFinales[3] = coordenadasFinales[2];
                    coordenadasFinales[2] = coordenadasFinales[1];
                    coordenadasFinales[1] = MBRtmp->Particiones[i].Inicio + MBRtmp->Particiones[i].Tamanio;
                }
                else if(MBRtmp->Particiones[i].Inicio < coordenadas[2] || coordenadas[2] == -1)
                {
                    coordenadas[4] = coordenadas[3];
                    coordenadas[3] = coordenadas[2];
                    coordenadas[2] = MBRtmp->Particiones[i].Inicio;

                    coordenadasFinales[5] = coordenadasFinales[4];
                    coordenadasFinales[4] = coordenadasFinales[3];
                    coordenadasFinales[3] = coordenadasFinales[2];
                    coordenadasFinales[2] = MBRtmp->Particiones[i].Inicio + MBRtmp->Particiones[i].Tamanio;
                }
                else if(MBRtmp->Particiones[i].Inicio < coordenadas[3] || coordenadas[3] == -1)
                {
                    coordenadas[4] = coordenadas[3];
                    coordenadas[3] = MBRtmp->Particiones[i].Inicio;

                    coordenadasFinales[5] = coordenadasFinales[4];
                    coordenadasFinales[4] = coordenadasFinales[3];
                    coordenadasFinales[3] = MBRtmp->Particiones[i].Inicio + MBRtmp->Particiones[i].Tamanio;
                }
                else if(MBRtmp->Particiones[i].Inicio < coordenadas[4] || coordenadas[4] == -1)
                {
                    coordenadas[4] = MBRtmp->Particiones[i].Inicio;

                    coordenadasFinales[5] = coordenadasFinales[4];
                    coordenadasFinales[4] = MBRtmp->Particiones[i].Inicio + MBRtmp->Particiones[i].Tamanio;
                }
                }
            }


            /**Segun el ajuste que se aplicara se busca un espacio**/

            PosInicial = -1;
            int TamanioActual = 0;

            if(MBRtmp->Ajuste == 'b' || MBRtmp->Ajuste == 'B')/********************Mejor ajuste***********************/
            {
                TamanioActual = MBRtmp->Tamanio;

                for(int i = 0 ; i < 5 ; i++)
                {
                    if(Tamanio <=  coordenadas[i+1] - coordenadasFinales[i])
                    {
                        if(coordenadas[i+1] - coordenadasFinales[i] < TamanioActual)
                        {
                            PosInicial = coordenadasFinales[i];
                            TamanioActual = coordenadas[i+1] - coordenadasFinales[i];
                        }
                    }
                }
            }
            else if(MBRtmp->Ajuste == 'f' || MBRtmp->Ajuste == 'F')/********************Primer ajuste***********************/
            {

                for(int i = 0 ; i < 5 ; i++)
                {
                    if(Tamanio <= coordenadas[i+1] - coordenadasFinales[i])
                    {
                        PosInicial = coordenadasFinales[i];
                        break;
                    }
                }

            }
            else if(MBRtmp->Ajuste == 'W' || MBRtmp->Ajuste == 'W')/********************Peor ajuste***********************/
            {
                TamanioActual = 0;

                for(int i = 0 ; i < 5 ; i++)
                {
                    if(Tamanio <= coordenadas[i+1] - coordenadasFinales[i])
                    {
                        if(coordenadas[i+1] - coordenadasFinales[i] > TamanioActual)
                        {
                            PosInicial = coordenadasFinales[i];
                            TamanioActual = coordenadas[i+1] - coordenadasFinales[i];
                        }
                    }
                }
            }



            /**Se agregan los atributos de la particion**/

            if(PosInicial != -1)
            {
                MBRtmp->Particiones[CooPart].Status = '1';
                if(pFit[0] == 'N')
                {
                    MBRtmp->Particiones[CooPart].Ajuste = 'W';
                }
                else
                {
                    MBRtmp->Particiones[CooPart].Ajuste = pFit[0];
                }
                MBRtmp->Particiones[CooPart].Tipo = 'E';
                MBRtmp->Particiones[CooPart].Inicio = PosInicial;
                MBRtmp->Particiones[CooPart].Tamanio = Tamanio;
                strcpy(MBRtmp->Particiones[CooPart].Nombre,pName);

                fseek(F, 0, SEEK_SET);
                fwrite(MBRtmp,sizeof(struct MBR),1,F);

                struct EBR *EBRtmpSig = (struct EBR*)malloc(sizeof(struct EBR));

                EBRtmpSig->Inicio = PosInicial;
                EBRtmpSig->Ajuste = ' ';
                strcpy(EBRtmpSig->Nombre, "Ninguno");
                EBRtmpSig->Status = '0';
                EBRtmpSig->Tamanio = 0;
                EBRtmpSig->Ap_Sig = -1;

                fseek(F, PosInicial, SEEK_SET);
                fwrite(EBRtmpSig, sizeof(EBR), 1, F);

                free(EBRtmpSig);
                printf("Se creo la particion extendida \"%s\" en el disco: %s\n\n", pName, pPath);
            }
            else
            {
                printf("No hay espacio dentro del disco para crear la particion extendida, disco: %s.\n\n", pPath);
            }



        }
        else
        {
            printf("Las particiones del disco ya suman 4, no se puede crear otra particion extendida, disco: %s\n\n", pPath);
        }

    }
    else
    {
        printf("No se puede crear una particion extendida porque ya hay una, disco: %s\n", pPath);
    }

    fclose(F);
    free(MBRtmp);
}

void CrearParticionLogica(char pPath[], int Tamanio, char pName[], char pFit[])
{
    int CooPart = -1;

    FILE *F;
    F = fopen(pPath,"r+b");


    struct MBR *MBRtmp = (struct MBR*)malloc(sizeof(struct MBR));
    fseek(F, 0, SEEK_SET);
    fread(MBRtmp, sizeof(MBR), 1, F);

    int HayExtendida = 0;

    for(int i = 0 ; i < 4 ; i++)
    {
        if(MBRtmp->Particiones[i].Tipo == 'E')
        {
            HayExtendida = 1;
            CooPart = i;
            break;
        }
    }

    if(HayExtendida == 1)
    {
        struct EBR *EBRtmp = (struct EBR*)malloc(sizeof(struct EBR));

        int Inicio = MBRtmp->Particiones[CooPart].Inicio;

        while(1)
        {
            fseek(F, Inicio, SEEK_SET);
            fread(EBRtmp, sizeof(EBR), 1, F);

            if(EBRtmp->Status == '0' && EBRtmp->Ap_Sig == -1)
            {
                if(Inicio + Tamanio < MBRtmp->Particiones[CooPart].Inicio + MBRtmp->Particiones[CooPart].Tamanio)
                {
                    EBRtmp->Inicio = Inicio;
                    EBRtmp->Ajuste = pFit[0];
                    strcpy(EBRtmp->Nombre, pName);
                    EBRtmp->Status = '1';
                    EBRtmp->Tamanio = Tamanio;
                    EBRtmp->Ap_Sig = Inicio + EBRtmp->Tamanio;

                    struct EBR *EBRtmpSig = (struct EBR*)malloc(sizeof(struct EBR));

                    EBRtmpSig->Inicio = Inicio + EBRtmp->Tamanio;
                    EBRtmpSig->Ajuste = ' ';
                    strcpy(EBRtmpSig->Nombre, "Ninguno");
                    EBRtmpSig->Status = '0';
                    EBRtmpSig->Tamanio = 0;
                    EBRtmpSig->Ap_Sig = -1;

                    fseek(F, Inicio, SEEK_SET);
                    fwrite(EBRtmp, sizeof(EBR), 1, F);

                    fseek(F, Inicio + EBRtmp->Tamanio, SEEK_SET);
                    fwrite(EBRtmpSig, sizeof(EBR), 1, F);

                    free(EBRtmpSig);
                    printf("Se creo la particion logica \"%s\" en el disco: %s\n\n", pName, pPath);
                    break;
                }
                else
                {
                    printf("No hay espacio para crear la particion logica \"%s\" dentro de la extendida, disco: %s\n\n",pName, pPath);
                    break;
                }

            }

            fseek(F, EBRtmp->Ap_Sig, SEEK_SET);
            Inicio = EBRtmp->Ap_Sig;
        }

        free(EBRtmp);


    }
    else
    {
        printf("No existe una particion extendida para crear una logica, disco: %s\n", pPath);
    }

    fclose(F);
    free(MBRtmp);
}

void EliminarParticion(char pPath[], char pName[])
{
    FILE *F;
    F = fopen(pPath,"r+b");

    if(F == NULL)
    {
        printf("El disco conde quiere borrar la particion no existe, disco: %s",pPath);
        return 0;
    }

    struct MBR *MBRtmp = (struct MBR*)malloc(sizeof(struct MBR));
    fseek(F, 0, SEEK_SET);
    fread(MBRtmp, sizeof(MBR), 1, F);
    int BorroParticion = 0;


    /**Se busca la particion con el nombre deseado**/

    for(int i = 0 ; i < 4 ; i++)
    {
        if(strcmp(MBRtmp->Particiones[i].Nombre, pName) == 0)
        {
            if(MBRtmp->Particiones[i].Tipo == 'e' || MBRtmp->Particiones[i].Tipo == 'E')
            {
                struct EBR *EBRtmp = (struct EBR*)malloc(sizeof(struct EBR));
                fseek(F, MBRtmp->Particiones[i].Inicio, SEEK_SET);
                fread(EBRtmp, sizeof(EBR), 1, F);

                if(EBRtmp->Status == '1')
                {
                    int Opcion = 3;

                    while(Opcion != 0 && Opcion != 1)
                    {
                        printf("La particion contiene almenos una logica dentro, ¿Desea eliminarla?\n 1: SI | 0: NO \n");
                        printf(">Seleccionar:");
                        scanf("%i",&Opcion);
                        if(Opcion == 1)
                        {
                            MBRtmp->Particiones[i].Status = '0';
                            MBRtmp->Particiones[i].Ajuste = '0';
                            MBRtmp->Particiones[i].Tipo = '0';
                            MBRtmp->Particiones[i].Inicio = -1;
                            MBRtmp->Particiones[i].Tamanio = -1;
                            strcpy(MBRtmp->Particiones[i].Nombre,"Ninguno");
                            BorroParticion = 1;
                            break;
                        }
                        else if(Opcion == 0)
                        {
                            break;
                        }
                        else
                        {
                            printf("Opcion invalida!\n");
                        }
                    }
                }
                else
                {
                    MBRtmp->Particiones[i].Status = '0';
                    MBRtmp->Particiones[i].Ajuste = '0';
                    MBRtmp->Particiones[i].Tipo = '0';
                    MBRtmp->Particiones[i].Inicio = -1;
                    MBRtmp->Particiones[i].Tamanio = -1;
                    strcpy(MBRtmp->Particiones[i].Nombre,"Ninguno");
                    BorroParticion = 1;
                }
                free(EBRtmp);
                break;
            }
            else
            {
                MBRtmp->Particiones[i].Status = '0';
                MBRtmp->Particiones[i].Ajuste = '0';
                MBRtmp->Particiones[i].Tipo = '0';
                MBRtmp->Particiones[i].Inicio = -1;
                MBRtmp->Particiones[i].Tamanio = -1;
                strcpy(MBRtmp->Particiones[i].Nombre,"Ninguno");
                BorroParticion = 1;
                break;
            }
        }
    }

    if(BorroParticion == 1)
    {
        fseek(F, 0, SEEK_SET);
        fwrite(MBRtmp,sizeof(struct MBR),1,F);
        printf("Se elimino la particion \"%s\" del disco: %s\n", pName, pPath);
    }
    else
    {
        int CoorExt = -1;
        for(int i = 0 ; i < 4 ; i++)
        {
            if(MBRtmp->Particiones[i].Tipo == 'e' || MBRtmp->Particiones[i].Tipo == 'E')
            {
                CoorExt = i;
                break;
            }
        }

        if(CoorExt != -1)
        {
            struct EBR *EBRtmp = (struct EBR*)malloc(sizeof(struct EBR));
            struct EBR *EBRtmp2 = (struct EBR*)malloc(sizeof(struct EBR));

            fseek(F, MBRtmp->Particiones[CoorExt].Inicio, SEEK_SET);
            fread(EBRtmp, sizeof(EBR), 1, F);
            fseek(F, MBRtmp->Particiones[CoorExt].Inicio, SEEK_SET);
            fread(EBRtmp2, sizeof(EBR), 1, F);


            while(1)
            {
                if(strcmp(EBRtmp2->Nombre, pName) == 0)
                {
                    if(EBRtmp2->Inicio == MBRtmp->Particiones[CoorExt].Inicio)
                    {
                        EBRtmp2->Ajuste = ' ';
                        strcpy(EBRtmp2->Nombre, "Ninguno");
                        EBRtmp2->Status = '0';
                        EBRtmp2->Tamanio = 0;
                        fseek(F, EBRtmp2->Inicio, SEEK_SET);
                        fwrite(EBRtmp2, sizeof(struct EBR), 1, F);
                        printf("Se elimino la particion logica \"%s\" del disco: %s\n\n", pName, pPath);
                        break;
                    }
                    else
                    {
                        EBRtmp->Ap_Sig = EBRtmp2->Ap_Sig;
                        fseek(F, EBRtmp->Inicio, SEEK_SET);
                        fwrite(EBRtmp, sizeof(struct EBR), 1, F);
                        printf("Se elimino la particion logica \"%s\" del disco: %s\n\n", pName, pPath);
                        break;
                    }
                }

                if(EBRtmp2->Ap_Sig != -1)
                {
                    fseek(F, EBRtmp2->Inicio, SEEK_SET);
                    fread(EBRtmp, sizeof(EBR), 1, F);

                    fseek(F, EBRtmp2->Ap_Sig, SEEK_SET);
                    fread(EBRtmp2, sizeof(EBR), 1, F);

                }
                else
                {
                    break;
                }
            }

            free(EBRtmp2);
            free(EBRtmp);
        }
        else
        {
            printf("No se elimino la particion \"%s\" del disco: %s, probablemente no existe\n", pName, pPath);
        }

    }



    fclose(F);

    free(MBRtmp);
}
