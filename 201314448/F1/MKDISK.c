#include "MKDISK.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void EJecutarMKDISK(char pSize[], char pUnit, char pPath[], char pFit[])
{
    for(int j = 0 ; j < strlen(pPath) ; j++)
    {
        if(pPath[j] == ' ')
        {
            pPath[j] = '_';
        }
    }

    char pPathAux[100] = "\0";


    strcpy(pPathAux, pPath);
    CrearDirectorio(pPathAux);

    FILE *F;

    F = fopen(pPath,"wb+");
    if(F != NULL)
    {
        int Tamnio = atoi(pSize);
        int Unidades;

        if(pUnit == 'k' || pUnit == 'K')
        {
            Unidades = 1024;
        }
        else if(pUnit == 'm' || pUnit == 'M' || pUnit == 'N')
        {
            Unidades = 1024*1024;
        }

        fseek(F, 0, SEEK_SET);
        fwrite("\0", 1, 1, F);

        fseek(F, (Tamnio * Unidades) - 1, SEEK_SET);
        fwrite("\0", 1, 1, F);



        fflush(F);
        fclose(F);

        CargarMBR(Tamnio * Unidades, pPath, pFit);

        printf("************Disco creado con extito************\nRuta: ");
        printf("%s\n", pPath);
        printf("Tamaño: %s%c\n\n", pSize, pUnit);

        return 1;
    }

    printf("Error al crear el disco: %s\n\n", pPath);

}

void EjecutarRMDISK(char path[])
{
    FILE *F;

    F = fopen(path,"r+b");

    if(F != NULL)
    {
        fclose(F);

        int Opcion = 3;

        while(Opcion != 0 && Opcion != 1)
        {
            printf("¿Desea eliminar el disco: %s?\n 1: SI | 0: NO \n", path);
            printf(">Seleccionar:");
            scanf("%i",&Opcion);
            if(Opcion == 1)
            {
                if(remove(path) == 0)
                {
                    printf("************Disco eliminado con extito************\nRuta: ");
                    printf("%s",path);
                    printf("\n\n");
                }
                else
                {
                    printf("************No se puedo eliminar el disco************\nRuta: ");
                    printf("%s",path);
                    printf("\n\n");
                }
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
        printf("¡¡¡¡¡Error al buscar el archivo, probablemente no exista!!!\n\n");
    }

}

void CargarMBR(int tamanio, char pPath[], char pFit[])
{
    FILE *F;

    F = fopen(pPath,"r+b");
    if(F != NULL)
    {
        /**Se crean el mbr y una particion las cuales se escribiran al disco**/

        struct MBR *MBRDisco = (struct MBR*)malloc(sizeof(struct MBR));
        struct Particion *ParticionDisco = (struct Particion*)malloc(sizeof(struct Particion));
        memset(MBRDisco,0, sizeof(struct MBR));
        memset(ParticionDisco,0,sizeof(struct Particion));


        /**Se agregan los atributos al MBR**/

        MBRDisco->Tamanio =  tamanio;
        MBRDisco->disk_asignature = rand();
        if(pFit[0] == 'N')
        {
            MBRDisco->Ajuste =  'F';
        }
        else
        {
            MBRDisco->Ajuste =  pFit[0];
        }
        strcpy(MBRDisco->FechaCreacion, FechaYHoraActual());

        /**Se agregan los atributos de la particion**/

        ParticionDisco->Status = '0';
        ParticionDisco->Ajuste = '0';
        ParticionDisco->Tipo = '0';
        ParticionDisco->Inicio = -1;
        ParticionDisco->Tamanio = -1;
        strcpy(ParticionDisco->Nombre,"Ninguno");



        /**Se cargan las particiones al MBR y se escriben en el archivo**/

        for(int i = 0 ; i < 4 ; i++)
        {
            MBRDisco->Particiones[i] = ParticionDisco[0];
        }

        fseek(F, 0, SEEK_SET);
        fwrite(MBRDisco,sizeof(struct MBR),1,F);

        fclose(F);

        return 1;
    }

    return 0;
}

void ProbarMBR(char pPath[])
{
    FILE *F;

    F = fopen(pPath,"r+b");
    if(F != NULL)
    {
        struct MBR *MBRtmp = (struct MBR*)malloc(sizeof(struct MBR));
        fseek(F, 0, SEEK_SET);
        fread(MBRtmp, sizeof(MBR), 1, F);
        printf("Disco: %s\n", pPath);
        printf("Tamaño: %i\n", MBRtmp->Tamanio);
        printf("Fecha de creacion: %s\n", MBRtmp->FechaCreacion);
        printf("ID: %i\n\n", MBRtmp->disk_asignature);

        for(int i = 0 ; i < 4 ; i++)
        {
            printf("Particion%i\n", i);
            printf("Status: %c\n", MBRtmp->Particiones[i].Status);
            printf("Tipo: %c\n", MBRtmp->Particiones[i].Tipo);
            printf("Ajuste: %c\n", MBRtmp->Particiones[i].Ajuste);
            printf("Inicio: %i\n", MBRtmp->Particiones[i].Inicio);
            printf("Tamaño: %i\n", MBRtmp->Particiones[i].Tamanio);
            printf("Nombre: %s\n\n", MBRtmp->Particiones[i].Nombre);

            if(MBRtmp->Particiones[i].Status == '1' && MBRtmp->Particiones[i].Tipo == 'E')
            {
                struct EBR *EBRtmp = (struct EBR*)malloc(sizeof(struct EBR));

                fseek(F, MBRtmp->Particiones[i].Inicio, SEEK_SET);

                while(1)
                {
                    fread(EBRtmp, sizeof(EBR), 1, F);
                    if(EBRtmp->Status == '1')
                    {
                        printf("       Inicio: %i\n", EBRtmp->Inicio);
                        printf("       Ajuste: %c\n", EBRtmp->Ajuste);
                        printf("       Nombre: %s\n", EBRtmp->Nombre);
                        printf("       Status: %c\n", EBRtmp->Status);
                        printf("       Tamaño: %i\n", EBRtmp->Tamanio);
                        printf("       AP Sig: %i\n\n", EBRtmp->Ap_Sig);
                    }
                    else
                    {
                        break;
                    }
                    fseek(F, EBRtmp->Ap_Sig, SEEK_SET);
                }
                free(EBRtmp);

            }
        }

        fclose(F);
        free(MBRtmp);
    }
}

char *FechaYHoraActual(){
    time_t t = time(0);
    struct tm *tiempoactual = localtime(&t);

    char *FH = malloc(sizeof(char)*30);

    strftime(FH,30,"%d/%m/%y %H:%M:%S",tiempoactual);

    return FH;
}

void CrearDirectorio(char *ruta)
{
    char *spl;
    int i = 0;
    char Destino[500] = "\0";

    char Ejecutar[500] = "\0";

    int agregar = 1;
    spl = strtok(ruta,"/");

    while(spl != NULL)
    {
        for(int j = 0 ; j < strlen(spl) ; j++)
        {
            if(spl[j] == '.')
            {
                agregar = 0;
            }
        }

        if(agregar == 1)
        {

            strcat(Destino,"/");
            strcat(Destino,spl);

            strcpy(Ejecutar,"sudo mkdir -p ");
            strcat(Ejecutar, Destino);
            system(Ejecutar);

            strcpy(Ejecutar,"sudo chmod -R 777 ");
            strcat(Ejecutar, Destino);
            system(Ejecutar);
        }

        agregar = 1;

        spl = strtok(NULL,"/");
    }

}
