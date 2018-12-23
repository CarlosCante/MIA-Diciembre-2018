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

        if(JO->Tipo_Operacion[0] == '0')
        {
            fseek(DISCO, InicioParticion + sizeof(SuperBloque) +i*sizeof(Journaling), SEEK_SET);
            fwrite(OperacionActual, sizeof(Journaling), 1, DISCO);
            free(SB);
            free(JO);
            return;
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

    strcpy(JO->Tipo_Operacion, "\0");
    JO->Tipo_Elemento = 'N';
    strcpy(JO->nombre, "\0");
    strcpy(JO->contenido, "\0");
    strcpy(JO->fecha, "\0");
    strcpy(JO->propietario, "\0");
    strcpy(JO->permisos, "\0");

    for(int i = 0 ; i < Cantidad_N ; i++)
    {
        fseek(DISCO, InicioParticion + sizeof(SuperBloque) + i*sizeof(Journaling), SEEK_SET);
        fwrite(JO, sizeof(Journaling), 1, DISCO);
    }

    free(JO);
}
