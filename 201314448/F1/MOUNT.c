#include "MOUNT.h"

char Letra[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
char LetraOcupada[26] = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};

void EjecutarMOUNT(char pPath[], char pName[])
{
    if(VerificarParticionMontada(pPath, pName) == 1)
    {
        printf("La particion \"%s\" del disco \"%s\" ya esta montada.\n\n", pName, pPath);
        return 0;
    }

    FILE *F;

    F = fopen(pPath,"r+b");
    if(F != NULL)
    {
        struct MBR *MBRtmp = (struct MBR*)malloc(sizeof(struct MBR));
        fseek(F, 0, SEEK_SET);
        fread(MBRtmp, sizeof(MBR), 1, F);

        int CooPart = -1;

        for(int i = 0 ; i < 4 ; i++)
        {
            if(MBRtmp->Particiones[i].Status == '1' && strcmp(MBRtmp->Particiones[i].Nombre, pName) == 0)
            {
                CooPart = i;
                break;
            }
        }

        if(CooPart != -1)
        {
            struct NodoParticion *nuevo = (struct NodoParticion*)malloc(sizeof(struct NodoParticion));

            int NumLetra = LetraParicion(pPath);
            int NumeroParticion = NumeroParticionMontada(pPath);;

            if(NumLetra == -1)
            {
                for(int i = 0 ; i < 26 ; i++)
                {
                    if(LetraOcupada[i] == '0')
                    {
                        NumLetra = i;
                        LetraOcupada[i] = Letra[i];
                        break;
                    }
                }
            }

            nuevo->inicio = MBRtmp->Particiones[CooPart].Inicio;
            nuevo->ID[0] = 'v';
            nuevo->ID[1] = 'd';
            nuevo->ID[2] = Letra[NumLetra];
            nuevo->ID[3] = NumeroParticion + '0';
            nuevo->ajuste = MBRtmp->Particiones[CooPart].Ajuste;
            strcpy(nuevo->Nombre, pName);
            strcpy(nuevo->Path, pPath);

            InsertarLista(nuevo);

            printf("Se monto la particion \"%s\" del disco \"%s\" con el ID \"%s\".\n\n", pName, pPath, nuevo->ID);
        }
        else
        {
            printf("La particion a montar \"%s\" no existe en el disco: %s\n\n", pName, pPath);
        }
        free(MBRtmp);
        fclose(F);
    }
    else
    {
        printf("No se puede montar la particion \"%s\" porque el disco no existe, disco: %s\n\n", pName, pPath);
    }
}

void EjecutarUNMOUNT(char pID[])
{
    extern ListaMomunt *LISTADO;
    int Desmonto = 0;
    struct NodoParticion *aux= LISTADO->primero;

    while(aux != NULL)
    {
        if(aux->ID[2] == pID[2] && aux->ID[3] == pID[3])
        {
            if(aux == LISTADO->primero)
            {
                aux->siguiente->anterior = NULL;
                LISTADO->primero = aux->siguiente;
            }
            else
            {
                if(aux == LISTADO->ultimo)
                {
                    aux->anterior->siguiente = NULL;
                    LISTADO->ultimo = aux->anterior;
                }
                else
                {
                    aux->siguiente->anterior = aux->anterior;
                    aux->anterior->siguiente = aux->siguiente;
                }
            }

            free(aux);
            Desmonto = 1;
            break;
        }

        aux = aux->siguiente;
    }

    if(Desmonto == 1)
    {
        printf("Se desmonto la particion \"%s\" con exito!!!\n", pID);
    }
    else
    {
        printf("No se desmonto la particion \"%s\", no esta montada.\n", pID);
    }

}

void InsertarLista(struct NodoParticion *nuevoaux)
{
    extern ListaMomunt *LISTADO;

    nuevoaux->anterior = NULL;
    nuevoaux->siguiente = NULL;

    if(LISTADO->primero == NULL)
    {
        LISTADO->ultimo = nuevoaux;
        LISTADO->primero = nuevoaux;
    }
    else
    {
        nuevoaux->anterior = LISTADO->ultimo;
        LISTADO->ultimo->siguiente = nuevoaux;
        LISTADO->ultimo = nuevoaux;
    }
    LISTADO->cantidad++;
}

int LetraParicion(char pPath[])
{
    extern ListaMomunt *LISTADO;

    if(LISTADO->cantidad != 0)
    {
        struct NodoParticion *aux = LISTADO->primero;

        while(aux != NULL)
        {
            if(strcmp(aux->Path, pPath) == 0)
            {
                for(int i = 0 ; i < 26 ; i++)
                {
                    if(Letra[i] == aux->ID[2])
                    {
                        return i;
                    }
                }
            }
            aux = aux->siguiente;
        }
    }
    return -1;
}

int NumeroParticionMontada(char pPath[])
{
    extern ListaMomunt *LISTADO;
    int r = 1;

    struct NodoParticion *aux = LISTADO->primero;

    while(aux != NULL)
    {
        if(strcmp(aux->Path, pPath) == 0)
        {
            r++;
        }
        aux = aux->siguiente;
    }

    return r;
}

void ImprimirLista()
{
    extern ListaMomunt *LISTADO;
    printf("=============LISTA DE PARTICIONES MONTADAS===========\n");

    if(LISTADO->cantidad > 0)
    {
        struct NodoParticion *aux = LISTADO->primero;

        while(aux != NULL)
        {
            printf("Path: %s\n", aux->Path);
            printf("Nombre: %s\n", aux->Nombre);
            printf("ID: %s\n", aux->ID);
            printf("Inicio: %i\n\n", aux->inicio);

            aux = aux->siguiente;
        }
    }
}

int VerificarParticionMontada(char pPath[], char pName[])
{
    extern ListaMomunt *LISTADO;

    if(LISTADO->cantidad > 0)
    {
        struct NodoParticion *aux = LISTADO->primero;

        while(aux != NULL)
        {
            if(strcmp(aux->Nombre, pName) == 0 && strcmp(aux->Path, pPath) == 0)
            {
                return 1;
            }

            aux = aux->siguiente;
        }
    }
    return 0;
}
