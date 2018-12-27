#include "JOURNAL.h"

void NuevoOperacionJournaling(FILE *DISCO, int InicioParticion)
{
    extern Journaling *OperacionActual;

    struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
    memset(SB,0, sizeof(struct SuperBloque));

    fseek(DISCO, InicioParticion, SEEK_SET);
    fread(SB, sizeof(SuperBloque), 1, DISCO);

    struct Journaling *JO = (struct Journaling*)malloc(sizeof(struct Journaling));
    memset(JO,0, sizeof(struct Journaling));

    fseek(DISCO, InicioParticion + sizeof(SuperBloque), SEEK_SET);
    fread(JO, sizeof(Journaling), 1, DISCO);

    for(int i = 0 ; i < SB->Inodos_count ; i++)
    {
        fseek(DISCO, InicioParticion + sizeof(SuperBloque) +i*sizeof(Journaling), SEEK_SET);
        fread(JO, sizeof(Journaling), 1, DISCO);

        if(JO->Tipo_Operacion == '0')
        {
            fseek(DISCO, InicioParticion + sizeof(SuperBloque) +i*sizeof(Journaling), SEEK_SET);
            fwrite(OperacionActual, sizeof(Journaling), 1, DISCO);
            free(SB);
            free(JO);
            return 0;
        }
    }

    free(SB);
    free(JO);

    printf("Ya no hay espacio para almacenar transacciones en el Journaling");
}

void CrearJournal(FILE *DISCO, int InicioParticion, int Cantidad_N)
{
    struct Journaling *JO = (struct Journaling*)malloc(sizeof(struct Journaling));
    memset(JO,0, sizeof(struct Journaling));

    JO->Tipo_Operacion = '0';
    JO->Tipo_Elemento = 'N';
    strcpy(JO->nombre, "\0");
    strcpy(JO->contenido, "\0");
    strcpy(JO->fecha, "\0");
    strcpy(JO->propietario, "\0");
    JO->permisos = 0;

    for(int i = 0 ; i < Cantidad_N ; i++)
    {
        fseek(DISCO, InicioParticion + sizeof(SuperBloque) + i*sizeof(Journaling), SEEK_SET);
        fwrite(JO, sizeof(Journaling), 1, DISCO);
    }

    free(JO);
}

void EjecutarLOSS(char pID[])
{
    extern ListaMomunt *LISTADO;
    char PathDisco[100] = "\0";
    int existe = 0;

    struct NodoParticion *aux= LISTADO->primero;

    while(aux != NULL)
    {
        if(aux->ID[2] == pID[2] && aux->ID[3] == pID[3])
        {
            strcpy(PathDisco, aux->Path);
            existe = 1;
            break;
        }
        aux = aux->siguiente;
    }

    if(existe == 1)
    {

        FILE * DISCO;
        DISCO = fopen(PathDisco, "r+b");

        if(DISCO != NULL)
        {
            struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
            fseek(DISCO, aux->inicio, SEEK_SET);
            fread(SB, sizeof(SuperBloque), 1, DISCO);

            for(int i = 0 ; i < SB->Bloques_count ; i++)
            {
                fseek(DISCO, SB->inicio_BM_Bloques + i, SEEK_SET);
                fwrite("0", sizeof(char), 1, DISCO);
            }

            for(int i = 0 ; i < SB->Inodos_count ; i++)
            {
                fseek(DISCO, SB->inicio_BM_Inodos + i, SEEK_SET);
                fwrite("0", sizeof(char), 1, DISCO);
            }

            for(int i = 0 ; i < SB->Bloques_count*sizeof(BloqueArchivo) ; i++)
            {
                fseek(DISCO, SB->inicio_Bloques + i, SEEK_SET);
                fwrite("\0", sizeof(char), 1, DISCO);
            }

            for(int i = 0 ; i < SB->Inodos_count*sizeof(Inodo) ; i++)
            {
                fseek(DISCO, SB->inicio_Inodos + i, SEEK_SET);
                fwrite("\0", sizeof(char), 1, DISCO);
            }

            free(SB);
            fclose(DISCO);
            printf("Se realizo la simulacion LOSS con exito.\n\n");

        }
        else
        {
            printf("EL disco de la particion \"%s\" no existe.\n", pID);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar la simulacion LOSS.\n", pID);
    }
}

void EjecutarRECOVERY(char pID[])
{
    extern ListaMomunt *LISTADO;
    extern UsuarioLogeado *UsuarioActual;

    char PathDisco[100] = "\0";
    int existe = 0;

    struct NodoParticion *aux= LISTADO->primero;

    while(aux != NULL)
    {
        if(aux->ID[2] == pID[2] && aux->ID[3] == pID[3])
        {
            strcpy(PathDisco, aux->Path);
            existe = 1;
            break;
        }
        aux = aux->siguiente;
    }

    if(existe == 1)
    {

        FILE * DISCO;
        DISCO = fopen(PathDisco, "r+b");

        if(DISCO != NULL)
        {
            UsuarioActual->Estado = '1';
            UsuarioActual->ID = 1;
            strcpy(UsuarioActual->Grupo, "root");
            strcpy(UsuarioActual->Nombre, "root");
            strcpy(UsuarioActual->Pass, "123");
            strcpy(UsuarioActual->PathDisco,aux->Path);
            UsuarioActual->InicioParticion = aux->inicio;
            UsuarioActual->ajuste = aux->ajuste;

            struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
            fseek(DISCO, aux->inicio, SEEK_SET);
            fread(SB, sizeof(SuperBloque), 1, DISCO);

            struct Journaling *JO = (struct Journaling*)malloc(sizeof(struct Journaling));
            memset(JO,0, sizeof(struct Journaling));

            /**Cargamos a memoria todo el journaling del disco**/
            typedef struct ListadoJournal
            {
                char Comando[150];
            }ListadoJournal;

            ListadoJournal ListadoJO[500];
            int limite = 0;

            for(int i = 0 ; i < SB->Inodos_count ; i++)
            {
                fseek(DISCO, aux->inicio + sizeof(SuperBloque) + i*sizeof(Journaling), SEEK_SET);
                fread(JO, sizeof(Journaling), 1, DISCO);

                if(JO->Tipo_Operacion != '0')
                {
                    strcpy(ListadoJO[i].Comando, JO->contenido);
                    limite++;
                }
            }

            for(int i = 0 ; i < limite ; i++)
            {
                EjecutarComando(ListadoJO[i].Comando, 1);
            }

            free(SB);
            free(JO);
            fclose(DISCO);
            printf("Se realizo el RECOVERY con exito.\n\n");

        }
        else
        {
            printf("EL disco de la particion \"%s\" no existe.\n", pID);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar la simulacion LOSS.\n", pID);
    }
}
