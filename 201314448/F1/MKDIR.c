#include "MKDIR.h"

void EjecutarMKDIR(char Path[], char P)
{
    extern UsuarioLogeado *UsuarioActual;

    char PathAux[100] = "\0";
    strcpy(PathAux, Path);

    /**Primero se dividen los nombres de los directorios en distintas cadenas**/

    typedef struct Carpetas
    {
        char Nombre[50];
    }Carpetas;

    Carpetas ListadoCarpetas[100];
    int No_Carpetas = 0;

    char *Elemento = strtok(Path, "/");

    while(Elemento != NULL)
    {
        strcpy(ListadoCarpetas[No_Carpetas].Nombre, Elemento);
        Elemento = strtok(NULL, "/");
        No_Carpetas++;
    }


    /**Ahora ya separados los nombres se procede a verificar si ya existen y si esta definido p**/

    FILE *DISCO;
    DISCO = fopen(UsuarioActual->PathDisco, "r+b");

    struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
    fseek(DISCO, UsuarioActual->InicioParticion, SEEK_SET);
    fread(SB, sizeof(SuperBloque), 1, DISCO);

    int Padre = 0;
    int Hijo = 0;

    if(P == 'S')/**Se deben de crear todas las carpetas**/
    {
        for(int i = 0 ; i < No_Carpetas ; i++)
        {
            Padre = Hijo;
            Hijo = ExisteHijo(DISCO, SB, Padre, ListadoCarpetas[i].Nombre);
            if(Hijo == -1)/**La carpeta no existe entonces se crea**/
            {
                Hijo = CrearCarpeta(DISCO, SB, Padre, ListadoCarpetas[i].Nombre);
            }
        }

        printf("Se creo correctamente la ruta \"%s\".\n",PathAux);
    }
    else if(P == 'N')/**Solo se debe crear la ultima carpeta**/
    {
        for(int i = 0 ; i < No_Carpetas ; i++)
        {
            Padre = Hijo;
            Hijo = ExisteHijo(DISCO, SB, Padre, ListadoCarpetas[i].Nombre);
            if(Hijo == -1)/**La carpeta no existe entonces se crea**/
            {
                if(i + 1 == No_Carpetas)
                {
                    Hijo = CrearCarpeta(DISCO, SB, Padre, ListadoCarpetas[i].Nombre);
                    printf("Se creo correctamente la ruta \"%s\".\n",PathAux);
                    break;
                }
                else
                {
                    printf("Las no se puede crear el directorio debido a que faltan multiples carpetas de dicha ruta\n\n");
                    break;
                }
            }
        }
    }

    free(SB);
    fclose(DISCO);
}

int ExisteHijo(FILE *DISCO, SuperBloque *SB, int Padre, char Nombre[])
{
    int Respuesta = -1;

    struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoTMP, 0, sizeof(Inodo));

    struct BloqueCarpeta *BloqueTMP = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
    memset(BloqueTMP, 0, sizeof(BloqueCarpeta));

    fseek(DISCO, SB->inicio_Inodos + sizeof(Inodo)*Padre, SEEK_SET);
    fread(InodoTMP, sizeof(Inodo), 1, DISCO);

    for(int i = 0 ; i < 12 ; i++)
    {
        if(InodoTMP->ap_Bloques[i] != -1)
        {
            fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueCarpeta), SEEK_SET);
            fread(BloqueTMP, sizeof(BloqueCarpeta), 1, DISCO);

            for(int j = 0 ; j < 4 ; j++)
            {
                if(BloqueTMP->b_content[j].ap_inodo != -1)
                {
                    if(strcmp(BloqueTMP->b_content[j].nombre, Nombre) == 0)
                    {
                        Respuesta = BloqueTMP->b_content[j].ap_inodo;
                        break;
                    }
                }
            }
        }

        if(Respuesta != -1)
        {
            break;
        }
    }

    free(InodoTMP);
    free(BloqueTMP);

    return Respuesta;
}

int CrearCarpeta(FILE *DISCO, SuperBloque *SB, int Padre, char Nombre[])
{
    /**Declaramos las estructuras a usar*****************************************************************************/
    extern UsuarioLogeado *UsuarioActual;

    //Carpeta Padre
    struct Inodo *InodoPadre = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoPadre, 0, sizeof(Inodo));

    struct BloqueCarpeta *BloquePadre = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
    memset(BloquePadre, 0, sizeof(BloqueCarpeta));

    //Carpeta Hijo

    struct Inodo *InodoHijo = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoHijo, 0, sizeof(Inodo));

    struct BloqueCarpeta *BloqueHijo = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
    memset(BloqueHijo, 0, sizeof(BloqueCarpeta));


    /**Se busca la aposicion del inodo y el bloque para la nueva carpeta y se marcan en los bitmap********************/

    int PosHijoInodo = BuscarPosicionInodo(DISCO, SB, 1);
    fseek(DISCO, SB->inicio_BM_Inodos + PosHijoInodo, SEEK_SET);
    fwrite("1", sizeof(char), 1, DISCO);

    int PosHijoBloque = BuscarPosicionBloques(DISCO, SB, 1);
    fseek(DISCO, SB->inicio_BM_Bloques + PosHijoBloque, SEEK_SET);
    fwrite("1", sizeof(char), 1, DISCO);



    /**Actualizamos los datos del inodo y bloque del padre para que apunten a la nueva carpeta***********************/

    int EncontroEspacio = 0;
    int PosBloquePadre = -1;

    fseek(DISCO, SB->inicio_Inodos + Padre*sizeof(Inodo), SEEK_SET);
    fread(InodoPadre, sizeof(Inodo), 1, DISCO);

    for(int i = 0 ; i < 12 ; i++)
    {
        if(InodoPadre->ap_Bloques[i] != -1)
        {
            fseek(DISCO, SB->inicio_Bloques + InodoPadre->ap_Bloques[i]*sizeof(BloqueCarpeta), SEEK_SET);
            fread(BloquePadre, sizeof(BloqueCarpeta), 1, DISCO);

            for(int j = 0 ; j < 4 ; j++)
            {
                if(BloquePadre->b_content[j].ap_inodo == -1)
                {
                    strcpy(BloquePadre->b_content[j].nombre, Nombre);
                    BloquePadre->b_content[j].ap_inodo = PosHijoInodo;
                    EncontroEspacio = 1;
                    PosBloquePadre = InodoPadre->ap_Bloques[i];
                    break;
                }
            }
        }

        if(EncontroEspacio == 1)
        {
            break;
        }
    }

    if(EncontroEspacio == 0)
    {
        PosBloquePadre = ObtenerBloqueLibre(DISCO, SB, 1, UsuarioActual->ajuste);
        fseek(DISCO, SB->inicio_BM_Bloques + PosBloquePadre, SEEK_SET);
        fwrite("1", sizeof(char), 1, DISCO);


        strcpy(BloquePadre->b_content[0].nombre, Nombre);
        BloquePadre->b_content[0].ap_inodo = PosHijoInodo;

        strcpy(BloquePadre->b_content[1].nombre, "");
        BloquePadre->b_content[1].ap_inodo = -1;

        strcpy(BloquePadre->b_content[2].nombre, "");
        BloquePadre->b_content[2].ap_inodo = -1;

        strcpy(BloquePadre->b_content[3].nombre, "");
        BloquePadre->b_content[3].ap_inodo = -1;

        for(int i = 0 ; i < 12 ; i++)
        {
            if(InodoPadre->ap_Bloques[i] != -1)
            {
                InodoPadre->ap_Bloques[i] = PosBloquePadre;
                break;
            }
        }
    }

    //Se escriben en el disco los datos del inodo y el bloque actualizados

    strcpy(InodoPadre->fecha_modificacion, FechaYHoraActual());
    strcpy(InodoPadre->fecha_lectura, FechaYHoraActual());

    fseek(DISCO, SB->inicio_Inodos + Padre*sizeof(Inodo), SEEK_SET);
    fwrite(InodoPadre, sizeof(Inodo), 1, DISCO);

    fseek(DISCO, SB->inicio_Bloques + PosBloquePadre*sizeof(BloqueCarpeta), SEEK_SET);
    fwrite(BloquePadre, sizeof(BloqueCarpeta), 1, DISCO);












     /**Creamos el inodo de la carpeta y llenamos sus atributos***************************************************/

    InodoHijo->ID_Usuario = UsuarioActual->ID;
    InodoHijo->ID_Grupo = UsuarioActual->ID;
    InodoHijo->Tamanio = 0;

    strcpy(InodoHijo->fecha_creacion, FechaYHoraActual());
    strcpy(InodoHijo->fecha_lectura, FechaYHoraActual());
    strcpy(InodoHijo->fecha_modificacion, FechaYHoraActual());

    InodoHijo->ap_Bloques[0] = PosHijoBloque;

    for(int i = 1 ; i < 15 ; i++)
    {
        InodoHijo->ap_Bloques[i] = -1;
    }

    InodoHijo->tipo = '0';
    InodoHijo->Permisos = 664;

    /**Creamos el bloque asociado a la carpeta **************************************************************/

    strcpy(BloqueHijo->b_content[0].nombre, Nombre);
    BloqueHijo->b_content[0].ap_inodo = PosHijoInodo;

    strcpy(BloqueHijo->b_content[1].nombre, "");
    BloqueHijo->b_content[1].ap_inodo = Padre;

    strcpy(BloqueHijo->b_content[2].nombre, "");
    BloqueHijo->b_content[2].ap_inodo = -1;

    strcpy(BloqueHijo->b_content[3].nombre, "");
    BloqueHijo->b_content[3].ap_inodo = -1;

    /**Escribimos en el disco el nuevo inodo y el nuevo bloque de la carpeta********************************/

    fseek(DISCO, SB->inicio_Inodos + PosHijoInodo*sizeof(Inodo), SEEK_SET);
    fwrite(InodoHijo, sizeof(Inodo), 1, DISCO);

    fseek(DISCO, SB->inicio_Bloques + PosHijoBloque*sizeof(BloqueCarpeta), SEEK_SET);
    fwrite(BloqueHijo, sizeof(BloqueCarpeta), 1, DISCO);



    free(InodoPadre);
    free(BloquePadre);
    free(InodoHijo);
    free(BloqueHijo);

    return PosHijoInodo;
}

int BuscarPosicionInodo(FILE *DISCO, SuperBloque *SB, int Cantidad)
{
    extern UsuarioLogeado *UsuarioActual;
    int TamMin = SB->Inodos_count;
    int Conteo = 0;
    int InicioAux = 0;
    int Inicio = -1;
    char C;

    if(UsuarioActual->ajuste == 'B')
    {
        int i = 0;
        while(i < SB->Inodos_count)
        {
            fseek(DISCO, SB->inicio_BM_Inodos + i, SEEK_SET);
            fread(&C, sizeof(char), 1, DISCO);
            if(C == '0')
            {
                InicioAux = i;
            }

            while(C == '0')
            {
                Conteo++;
                i++;
                fseek(DISCO, SB->inicio_BM_Inodos + i, SEEK_SET);
                fread(&C, sizeof(char), 1, DISCO);
            }

            if(Conteo < TamMin && Conteo >= Cantidad)
            {
                TamMin = Conteo;
                Inicio = InicioAux;
                Conteo = 0;
            }

            i++;
        }

    }
    else if(UsuarioActual->ajuste == 'F')
    {
        fseek(DISCO, SB->inicio_BM_Inodos, SEEK_SET);
        char C;

        for(int i = 0 ; i < SB->Inodos_count ; i++)
        {
            fseek(DISCO, SB->inicio_BM_Inodos + i, SEEK_SET);
            fread(&C, sizeof(char), 1, DISCO);
            if(C == '0')
            {
                Inicio = i;
                break;
            }
        }
    }
    else if(UsuarioActual->ajuste == 'W')
    {
        TamMin = 0;
        int i = 0;
        while(i < SB->Inodos_count)
        {
            fseek(DISCO, SB->inicio_BM_Inodos + i, SEEK_SET);
            fread(&C, sizeof(char), 1, DISCO);
            if(C == '0')
            {
                InicioAux = i;
            }

            while(C == '0')
            {
                Conteo++;
                i++;
                fseek(DISCO, SB->inicio_BM_Inodos + i, SEEK_SET);
                fread(&C, sizeof(char), 1, DISCO);
            }

            if(Conteo > TamMin && Conteo >= Cantidad)
            {
                TamMin = Conteo;
                Inicio = InicioAux;
                Conteo = 0;
            }

            i++;
        }
    }

    return Inicio;
}

int BuscarPosicionBloques(FILE *DISCO, SuperBloque *SB, int Cantidad)
{
    extern UsuarioLogeado *UsuarioActual;
    int TamMin = SB->Bloques_count;
    int Conteo = 0;
    int InicioAux = 0;
    int Inicio = -1;
    char C;

    if(UsuarioActual->ajuste == 'B')
    {
        int i = 0;
        while(i < SB->Bloques_count)
        {
            fseek(DISCO, SB->inicio_BM_Bloques + i, SEEK_SET);
            fread(&C, sizeof(char), 1, DISCO);
            if(C == '0')
            {
                InicioAux = i;
            }

            while(C == '0')
            {
                Conteo++;
                i++;
                fseek(DISCO, SB->inicio_BM_Bloques + i, SEEK_SET);
                fread(&C, sizeof(char), 1, DISCO);
            }

            if(Conteo < TamMin && Conteo >= Cantidad)
            {
                TamMin = Conteo;
                Inicio = InicioAux;
                Conteo = 0;
            }

            i++;
        }

    }
    else if(UsuarioActual->ajuste == 'F')
    {
        fseek(DISCO, SB->inicio_BM_Bloques, SEEK_SET);
        char C;

        for(int i = 0 ; i < SB->Bloques_count ; i++)
        {
            fseek(DISCO, SB->inicio_BM_Bloques + i, SEEK_SET);
            fread(&C, sizeof(char), 1, DISCO);
            if(C == '0')
            {
                Inicio = i;
                break;
            }
        }
    }
    else if(UsuarioActual->ajuste == 'W')
    {
        TamMin = 0;
        int i = 0;
        while(i < SB->Bloques_count)
        {
            fseek(DISCO, SB->inicio_BM_Bloques + i, SEEK_SET);
            fread(&C, sizeof(char), 1, DISCO);
            if(C == '0')
            {
                InicioAux = i;
            }

            while(C == '0')
            {
                Conteo++;
                i++;
                fseek(DISCO, SB->inicio_BM_Bloques + i, SEEK_SET);
                fread(&C, sizeof(char), 1, DISCO);
            }

            if(Conteo > TamMin && Conteo >= Cantidad)
            {
                TamMin = Conteo;
                Inicio = InicioAux;
                Conteo = 0;
            }

            i++;
        }
    }

    return Inicio;
}
