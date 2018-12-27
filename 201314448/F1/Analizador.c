#include "Analizador.h"
#include <string.h>

int EjecutarComando(char Comando[], int tipo)
{
    extern UsuarioLogeado *UsuarioActual;
    extern Journaling *OperacionActual;

    if(Comando[0] == '#')
    {
        printf("\n%s\n", Comando);
    }
    else if(Comando[1] != '\n' && tipo == 0)
    {
        printf("\n\n=================================COMANDO========================================\n%s\n",Comando);
        printf("RESULTADO DEL COMANDO:\n\n");
    }

    if(Comando[0] != '\n' && Comando[0] != '#' && Comando[0] != '\r')
    {
        char Funsion[10] = "\0";
        char ParametroAux[10] = "\0";

        char pSize[10] = "\0";
        pSize[0] = '1';
        char pUnit = 'N';
        char pPath[100] = "\0";
        char pType[5];
        pType[0] = 'P';
        char pFit[2] = "\0";
        pFit[0] = 'N';
        pFit[1] = 'F';
        char pDelete[5] = "\0";
        char pName[50] = "\0";
        char pAdd[10] = "\0";
        char pId[10] = "\0";

        /**Parametros para el control de usuarios**/
        char pUSR[10] = "\0";
        char pPWD[10] = "\0";
        char pGRP[10] = "\0";

        /**Parametros varios**/
        char pP = 'N';
        char pCont[100] = "\0";
        char pfile[100] = "\0";
        char pRuta[100] = "\0";



        int i = 0;
        int j = 0;

        while(Comando[i] != ' ')
        {
            Funsion[j] = Comando[i];
            i++;
            j++;
        }
        j = 0;
        i++;

        ConvertirMayusculas(Funsion);

        if(strcmp(Funsion, "EXEC") == 0)
        {
            while(Comando[i] != '\n' && Comando[i] != '\r')
            {
                if(Comando[i] == '#')
                {
                    break;
                }

                if(Comando[i] == '>')
                {
                    j = 0;
                    i++;
                    ConvertirMayusculas(ParametroAux);

                    if(strcmp(ParametroAux,"PATH") == 0)
                    {
                        if(Comando[i] == '"')
                        {
                            i++;
                            while(Comando[i] != '"')
                            {
                                pPath[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                        else
                        {
                            while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                            {
                                pPath[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                    }
                    break;
                }
                else
                {
                    if(Comando[i] != '-')
                    {
                        ParametroAux[j] = Comando[i];
                        j++;
                    }
                }
                i++;

            }
        }
        else
        {
            while(Comando[i - 1] != '\n' && Comando[i] != '\r')
            {

                if(Comando[i - 1] == '#')
                {
                    break;
                }

                if(Comando[i] == '-' && (Comando[i+1] == 'p' || Comando[i+1] == 'P') && (Comando[i+2] == ' ' || Comando[i+2] == '\n' || Comando[i+2] == '\r'))
                {
                    pP = 'S';
                    i++;
                    i++;
                }

                if(Comando[i] == '>')
                {
                    j = 0;
                    i++;
                    ConvertirMayusculas(ParametroAux);

                    if(strcmp(ParametroAux,"SIZE") == 0)
                    {
                        while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                        {
                            pSize[j] = Comando[i];
                            i++;
                            j++;
                        }
                    }
                    else if(strcmp(ParametroAux,"ID") == 0)
                    {
                        while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                        {
                            pId[j] = Comando[i];
                            i++;
                            j++;
                        }
                    }
                    else if(strcmp(ParametroAux,"USR") == 0)
                    {
                        while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                        {
                            pUSR[j] = Comando[i];
                            i++;
                            j++;
                        }
                    }
                    else if(strcmp(ParametroAux,"PWD") == 0)
                    {
                        while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                        {
                            pPWD[j] = Comando[i];
                            i++;
                            j++;
                        }
                    }
                    else if(strcmp(ParametroAux,"GRP") == 0)
                    {
                        while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                        {
                            pGRP[j] = Comando[i];
                            i++;
                            j++;
                        }
                    }
                    else if(strcmp(ParametroAux,"UNIT") == 0)
                    {
                        while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                        {
                            pUnit = Comando[i];
                            i++;
                            j++;
                        }
                    }
                    else if(strcmp(ParametroAux,"TYPE") == 0)
                    {
                        while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                        {
                            pType[j] = Comando[i];
                            i++;
                            j++;
                        }
                    }
                    else if(strcmp(ParametroAux,"FIT") == 0)
                    {
                        while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                        {
                            pFit[j] = Comando[i];
                            i++;
                            j++;
                        }
                        ConvertirMayusculas(pFit);
                    }
                    else if(strcmp(ParametroAux,"NAME") == 0)
                    {
                        if(Comando[i] == '"')
                        {
                            i++;
                            while(Comando[i] != '"')
                            {
                                pName[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                        else
                        {
                            while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                            {
                                pName[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                    }
                    else if(strcmp(ParametroAux,"DELETE") == 0)
                    {
                        while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                        {
                            pDelete[j] = Comando[i];
                            i++;
                            j++;
                        }
                        ConvertirMayusculas(pDelete);
                    }
                    else if(strcmp(ParametroAux,"ADD") == 0)
                    {
                        while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                        {
                            pAdd[j] = Comando[i];
                            i++;
                            j++;
                        }
                    }
                    else if(strcmp(ParametroAux,"P") == 0)
                    {
                        pP = 'S';
                        i++;
                        j++;
                    }
                    else if(strcmp(ParametroAux,"PATH") == 0)
                    {
                        if(Comando[i] == '"')
                        {
                            i++;
                            while(Comando[i] != '"')
                            {
                                pPath[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                        else
                        {
                            while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                            {
                                pPath[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                    }
                    else if(strcmp(ParametroAux,"RUTA") == 0)
                    {
                        if(Comando[i] == '"')
                        {
                            i++;
                            while(Comando[i] != '"')
                            {
                                pRuta[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                        else
                        {
                            while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                            {
                                pRuta[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                    }
                    else if(strcmp(ParametroAux,"FILE") == 0)
                    {
                        if(Comando[i] == '"')
                        {
                            i++;
                            while(Comando[i] != '"')
                            {
                                pfile[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                        else
                        {
                            while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                            {
                                pfile[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                    }
                    else if(strcmp(ParametroAux,"CONT") == 0)
                    {
                        if(Comando[i] == '"')
                        {
                            i++;
                            while(Comando[i] != '"')
                            {
                                pCont[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                        else
                        {
                            while(Comando[i] != ' ' && Comando[i] != '\n' && Comando[i] != '\r')
                            {
                                pCont[j] = Comando[i];
                                i++;
                                j++;
                            }
                        }
                    }
                    else
                    {
                        printf("La palabra \"%s\" no es un parametro reconocido.\n\n", ParametroAux);
                        return 0;
                    }
                    for(int k = 0 ; k < 10 ; k++)
                    {
                        ParametroAux[k] = '\0';
                    }
                    j=0;
                }
                else
                {
                    if(Comando[i] != '-')
                    {
                        ParametroAux[j] = Comando[i];
                        j++;
                    }
                }
                i++;
            }
        }

        if(strcmp(Funsion,"EXEC") == 0)
        {
            EjecutarArchivo(pPath);
        }
        else if(strcmp(Funsion,"MKDISK") == 0)
        {
            /*
            printf("Comando: MKDISK\n");
            printf("Size: %s\n", pSize);
            printf("Unit: %c\n", pUnit);
            printf("Path: %s\n\n", pPath);
            */

            EJecutarMKDISK(pSize, pUnit, pPath, pFit);
        }
        else if(strcmp(Funsion,"RMDISK") == 0)
        {
            /*
            printf("Comando: RMDISK\n");
            printf("Path: %s\n\n", pPath);
            */

            EjecutarRMDISK(pPath);

        }
        else if(strcmp(Funsion,"FDISK") == 0)
        {
            /*
            printf("Comando: FDISK\n");
            printf("Size: %s\n", pSize);
            printf("Unit: %c\n", pUnit);
            printf("Path: %s\n", pPath);
            printf("Type: %c\n", pType);
            printf("Delete: %s\n", pDelete);
            printf("Name: %s\n", pName);
            printf("Add: %s\n", pAdd);
            printf("Fit: %s\n\n", pFit);
            */

            EjecutarFDISK(pSize, pUnit, pPath, pType, pFit, pDelete, pName, pAdd);
            //printf("\n==================================================\n");
            //ProbarMBR(pPath);

        }
        else if(strcmp(Funsion,"MOUNT") == 0)
        {
            /*
            printf("Comando: MOUNT\n");
            printf("Path: %s\n", pPath);
            printf("Name: %s\n\n", pName);
            */

            if(pPath[0] != '\0' && pName != '\0')
            {
                EjecutarMOUNT(pPath, pName);
            }
            else
            {
                ImprimirLista();
            }
        }
        else if(strcmp(Funsion,"UNMOUNT") == 0)
        {
            /*
            printf("Comando: UNMOUNT\n");
            printf("ID: %s\n\n", pId);
            */
            EjecutarUNMOUNT(pId);
            ImprimirLista();
        }
        else if(strcmp(Funsion,"REP") == 0)
        {
            EjecutarREP(pName, pPath, pId, pRuta);
        }
        else if(strcmp(Funsion,"MKFS") == 0)/*************************************Comandos de segunda fase proyecto*******************************************/
        {
            strcpy(OperacionActual->contenido, Comando);
            EjecutarMKFS(pId, pType);
        }
        else if(strcmp(Funsion, "LOGIN") == 0)
        {
            if(pUSR[0] != '\0' && pPWD[0] != '\0' && pId[0] != '\0')
            {
                EjecutarLogin(pUSR,pPWD,pId);
            }
            else
            {
                printf("Falta algun parametro de los requeridos para hacer login: USR, PWD, ID\n\n");
            }
        }
        else if(strcmp(Funsion, "MKGRP") == 0)
        {
            strcpy(OperacionActual->contenido, Comando);
            EjecutarMKGRP(pName);
        }
        else if(strcmp(Funsion, "MKUSR") == 0)
        {
            strcpy(OperacionActual->contenido, Comando);
            EjecutarMKUSR(pUSR, pPWD, pGRP);
        }
        else if(strcmp(Funsion, "MKFILE") == 0)
        {
            strcpy(OperacionActual->contenido, Comando);
            EjecutarMKFILE(pPath, pP, pSize, pCont);
        }
        else if(strcmp(Funsion, "MKDIR") == 0)
        {
            strcpy(OperacionActual->contenido, Comando);
            EjecutarMKDIR(pPath, pP);
        }
        else if(strcmp(Funsion, "CAT") == 0)
        {
            EJecutarCAT(pfile, 0, "");
        }
        else if(strcmp(Funsion, "REM") == 0)
        {
            EjecutarREM(pPath);
        }
        else if(strcmp(Funsion, "LOSS") == 0)
        {
            EjecutarLOSS(pId);
        }
        else if(strcmp(Funsion, "RECOVERY") == 0)
        {
            EjecutarRECOVERY(pId);
        }
        else if(strcmp(Funsion, "LOGOUT") == 0)
        {
            if(UsuarioActual->Estado != '0')
            {
                UsuarioActual->Estado = '0';
                printf("Se cerro sesion del usuario \"%s\"\n\n",UsuarioActual->Nombre);
                return;
            }
            else if(UsuarioActual->Estado == '0')
            {
                printf("No existe ninguna sesion activa en este momento\n\n");
                return;
            }
        }
        else if(strcmp(Funsion,"SALIR") == 0)
        {
            printf("Cerrando programa!!!!\n");
            return 1;
        }
    }



    return 0;
}

void EjecutarArchivo(char* ruta)
{
    char *comandos[100];
    int i = 0;

    FILE *F;

    F = fopen(strtok(ruta,"\n"),"r+");

    if(F != NULL)
    {
        char linea[500];
        while(fgets(linea, sizeof linea, F) != NULL)
        {
            comandos[i] = malloc(strlen(linea) + 1);
            strcpy(comandos[i],linea);
            i++;
        }

        fclose(F);

        for(int j = 0 ; j < i ; j++)
        {
            EjecutarComando(comandos[j], 0);
        }

    }
    else
    {
        printf("No se pudo abrir el archivo, es posible que no exista\n");
        perror(ruta);
    }
}

void Split(char Linea[], char *Destino1[10])
{
    printf("No sirve\n");
}

void ConvertirMayusculas(char cadena[])
{
    int i=0;
    for (i=0;cadena[i]!='\0';++i)
    {
        if(cadena[i]>='a' && cadena[i]<='z')
        {
            cadena[i] = cadena[i]- 32;
        }
    }
}
