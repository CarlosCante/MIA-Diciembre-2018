#include "MKFS.h"

void EjecutarMKFS(char pID[], char pType[])
{
    extern ListaMomunt *LISTADO;
    extern Journaling *OperacionActual;

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
            /**Se lee el MBR del disco donde esta la particion para obtener sus datos**/

            struct MBR *MBRDisco = (struct MBR*)malloc(sizeof(struct MBR));
            fseek(DISCO, 0, SEEK_SET);
            fread(MBRDisco, sizeof(MBR), 1, DISCO);

            /**Se calcula la cantidad de estructuras que puede albergar la particion**/

            int n = floor((MBRDisco->Tamanio - sizeof(SuperBloque)) / (4 + sizeof(Journaling) + sizeof(Inodo) + 3 * sizeof(BloqueArchivo)));

            /**Se crean el superbloque que se escribiran al disco**/

            struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
            memset(SB,0, sizeof(struct SuperBloque));

            /**Se agregan los atributos al super bloque**/

            SB->Filesystem_Type = MBRDisco->disk_asignature;
            SB->Inodos_count = n;
            SB->Bloques_count = 3*n;
            SB->free_Inodos_count = n;
            SB->free_Bloques_count = 3*n;
            strcpy(SB->fecha_mount, FechaYHoraActual());
            strcpy(SB->fecha_unmount, FechaYHoraActual());
            SB->mount_count = 0;
            SB->magic_number = 61267;
            SB->Inodo_size = sizeof(Inodo);
            SB->Bloque_size = sizeof(BloqueArchivo);
            SB->first_free_Inodo = 0;
            SB->first_free_Bloque = 0;
            SB->inicio_BM_Inodos = aux->inicio + sizeof(SuperBloque) + n*sizeof(Journaling);
            SB->inicio_BM_Bloques = aux->inicio + sizeof(SuperBloque) + n*sizeof(Journaling) + n;
            SB->inicio_Inodos = aux->inicio + sizeof(SuperBloque) + n*sizeof(Journaling) + n + 3 * n;
            SB->inicio_Bloques = aux->inicio + sizeof(SuperBloque) + n*sizeof(Journaling) + n + 3 * n + n * sizeof(Inodo);

            fseek(DISCO, aux->inicio, SEEK_SET);
            fwrite(SB, sizeof(struct SuperBloque), 1, DISCO);

            CrearJournal(DISCO, aux->inicio, n);

            free(MBRDisco);
            free(SB);


            ReiniciarBitMaps(DISCO, aux->inicio);

            if(strcmp(pType,"FULL") == 0)
            {
                for(int i = SB->inicio_Inodos ; i < SB->inicio_Bloques + n*sizeof(BloqueArchivo) ; i++)
                {
                    fseek(DISCO, i, SEEK_SET);
                    fwrite("\0", 1, 1, DISCO);
                }
            }

            CrearUsersTXT(DISCO, aux->inicio);

            //ComprobarUsersTXT(DISCO, aux->inicio);

            /**Guardamos la operacion en el journaling de la particion**/
            OperacionActual->Tipo_Operacion = '1';
            OperacionActual->Tipo_Elemento = '1';
            strcpy(OperacionActual->nombre, "/users.txt");
            strcpy(OperacionActual->fecha, FechaYHoraActual());
            strcpy(OperacionActual->propietario, "root");
            OperacionActual->permisos = 007;

            NuevoOperacionJournaling(DISCO, aux->inicio);

            fclose(DISCO);
        }
        else
        {
            printf("El disco \"%s\" no se pudo abrir, problablemente no exista.\n\n",PathDisco);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar el formato.\n", pID);
    }
}

void ReiniciarBitMaps(FILE *DISCO, int InicioParticion)
{
    struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
    fseek(DISCO, InicioParticion, SEEK_SET);
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

    free(SB);
}

void CrearUsersTXT(FILE *DISCO, int InicioParticion)
{
    struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
    fseek(DISCO, InicioParticion, SEEK_SET);
    fread(SB, sizeof(SuperBloque), 1, DISCO);


    /**Creamos el inodo de la carpeta raiz y llenamos sus atributos***************************************************/

    fseek(DISCO, SB->inicio_BM_Inodos, SEEK_SET);
    fwrite("1", sizeof(char), 1, DISCO);

    struct Inodo *InodoRaiz = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoRaiz, 0, sizeof(Inodo));

    InodoRaiz->ID_Usuario = 1;
    InodoRaiz->ID_Grupo = 1;
    InodoRaiz->Tamanio = 0;

    strcpy(InodoRaiz->fecha_creacion, FechaYHoraActual());
    strcpy(InodoRaiz->fecha_lectura, FechaYHoraActual());
    strcpy(InodoRaiz->fecha_modificacion, FechaYHoraActual());

    InodoRaiz->ap_Bloques[0] = 0;

    for(int i = 1 ; i < 15 ; i++)
    {
        InodoRaiz->ap_Bloques[i] = -1;
    }

    InodoRaiz->tipo = '0';
    InodoRaiz->Permisos = 777;

    /**Creamos el inodo del archivo users.txt llenamos sus atributos******************************************************/

    fseek(DISCO, SB->inicio_BM_Inodos + 1, SEEK_SET);
    fwrite("1", sizeof(char), 1, DISCO);

    struct Inodo *InodoUsers = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoUsers, 0, sizeof(Inodo));

    InodoUsers->ID_Usuario = 1;
    InodoUsers->ID_Grupo = 1;
    InodoUsers->Tamanio = 9;

    strcpy(InodoUsers->fecha_creacion, FechaYHoraActual());
    strcpy(InodoUsers->fecha_lectura, FechaYHoraActual());
    strcpy(InodoUsers->fecha_modificacion, FechaYHoraActual());

    InodoUsers->ap_Bloques[0] = 1;

    for(int i = 1 ; i < 15 ; i++)
    {
        InodoUsers->ap_Bloques[i] = -1;
    }

    InodoUsers->tipo = '1';
    InodoUsers->Permisos = 777;

    /**Creamos el bloque asociado a la carpeta raiz**********************************************************/

    fseek(DISCO, SB->inicio_BM_Bloques, SEEK_SET);
    fwrite("1", sizeof(char), 1, DISCO);

    struct BloqueCarpeta *Bloque = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
    memset(Bloque, 0, sizeof(BloqueCarpeta));

    strcpy(Bloque->b_content[0].nombre, "/");
    Bloque->b_content[0].ap_inodo = 0;

    strcpy(Bloque->b_content[1].nombre, "/");
    Bloque->b_content[1].ap_inodo = 0;

    strcpy(Bloque->b_content[2].nombre, "users.txt");
    Bloque->b_content[2].ap_inodo = 1;

    strcpy(Bloque->b_content[3].nombre, "");
    Bloque->b_content[3].ap_inodo = -1;

    /**Creamos el bloque asociado al archivo users.txt********************************************************/

    fseek(DISCO, SB->inicio_BM_Bloques + 1, SEEK_SET);
    fwrite("1", sizeof(char), 1, DISCO);

    struct BloqueArchivo *BloqueA = (struct BloqueArchivo*)malloc(sizeof(struct BloqueArchivo));
    memset(BloqueA, 0, sizeof(BloqueArchivo));

    strcpy(BloqueA->Contenido,"1,G,root\n1,U,root,root,123\n");

    /**Escribimos en el disco los inodos y los abloques que creamos******************************************/

    fseek(DISCO, SB->inicio_Inodos, SEEK_SET);
    fwrite(InodoRaiz, sizeof(Inodo), 1, DISCO);

    fseek(DISCO, SB->inicio_Inodos + sizeof(Inodo), SEEK_SET);
    fwrite(InodoUsers, sizeof(Inodo), 1, DISCO);

    fseek(DISCO, SB->inicio_Bloques, SEEK_SET);
    fwrite(Bloque, sizeof(BloqueCarpeta), 1, DISCO);

    fseek(DISCO, SB->inicio_Bloques + sizeof(BloqueArchivo), SEEK_SET);
    fwrite(BloqueA, sizeof(BloqueArchivo), 1, DISCO);

    free(InodoUsers);
    free(InodoRaiz);
    free(Bloque);
    free(BloqueA);
    free(SB);
}

void ComprobarUsersTXT(FILE *DISCO, int InicioParticion)
{
    struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
    fseek(DISCO, InicioParticion, SEEK_SET);
    fread(SB, sizeof(SuperBloque), 1, DISCO);

    struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoTMP, 0, sizeof(Inodo));

    struct BloqueArchivo *BloqueTMP = (struct BloqueArchivo*)malloc(sizeof(struct BloqueArchivo));
    memset(BloqueTMP, 0, sizeof(BloqueArchivo));

    fseek(DISCO, SB->inicio_Inodos + sizeof(Inodo), SEEK_SET);
    fread(InodoTMP, sizeof(Inodo), 1, DISCO);

    printf("\nInformacion del inodo:\n\n");

    printf("ID Usuario: %i\n",InodoTMP->ID_Usuario);
    printf("ID Grupo: %i\n",InodoTMP->ID_Grupo);
    printf("Tamaño: %i\n",InodoTMP->Tamanio);

    printf("Fecha creacion: %s\n",InodoTMP->fecha_creacion);
    printf("Fecha lectura: %s\n",InodoTMP->fecha_lectura);
    printf("Fecha modificacion: %s\n",InodoTMP->fecha_modificacion);

    printf("\nApuntadores:\n");
    for(int i = 0 ; i < 15 ; i++)
    {
        printf("Apuntador %i: %i\n",i,InodoTMP->ap_Bloques[i]);
    }

    printf("\nTipo: %c\n",InodoTMP->tipo);
    printf("Permisos: %i\n", InodoTMP->Permisos);

    for(int i = 0 ; i < 12 ; i++)
    {
        if(InodoTMP->ap_Bloques[i] != -1)
        {
            fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueArchivo), SEEK_SET);
            fread(BloqueTMP, sizeof(BloqueArchivo), 1, DISCO);

            printf("%s",BloqueTMP->Contenido);
        }
    }

    free(BloqueTMP);
    free(InodoTMP);
    free(SB);
}



