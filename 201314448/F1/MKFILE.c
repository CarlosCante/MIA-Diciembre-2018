#include "MKFILE.h"

void EjecutarMKFILE(char Path[], char P, char pSize[], char Cont[])
{
    extern UsuarioLogeado *UsuarioActual;
    extern Journaling *OperacionActual;

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

    int ExisteRuta = 1;

    if(P == 'S')/**Se deben de crear todas las carpetas**/
    {
        for(int i = 0 ; i < No_Carpetas - 1 ; i++)
        {
            Padre = Hijo;
            Hijo = ExisteHijo(DISCO, SB, Padre, ListadoCarpetas[i].Nombre);
            if(Hijo == -1)/**La carpeta no existe entonces se crea**/
            {
                if(VerificarPermisos(DISCO, SB, Padre, 'E') == 1)
                {
                    Hijo = CrearCarpeta(DISCO, SB, Padre, ListadoCarpetas[i].Nombre);
                }
                else
                {
                    printf("EL usuario no tiene permiso de escritura en la carpeta \"%s\".\n\n", ListadoCarpetas[i - 1].Nombre);
                    free(SB);
                    fclose(DISCO);
                    return;
                }
            }
        }
    }
    else if(P == 'N')/**Solo se debe crear la ultima carpeta**/
    {
        for(int i = 0 ; i < No_Carpetas - 1 ; i++)
        {
            Padre = Hijo;
            Hijo = ExisteHijo(DISCO, SB, Padre, ListadoCarpetas[i].Nombre);
            if(Hijo == -1)/**La carpeta no existe entonces se crea**/
            {
                ExisteRuta = 0;
                break;
            }
        }
    }

    /**Se verifica si la ruta para crear el archivo existe y de ser asi se procede a crear el archivo**/

    if(ExisteRuta == 1)
    {
        Padre = Hijo;

        if(VerificarPermisos(DISCO, SB, Padre, 'E') == 1)
        {
             CrearArchivo(DISCO, SB, Padre, ListadoCarpetas[No_Carpetas - 1].Nombre, pSize, Cont);

            OperacionActual->Tipo_Operacion = '1';
            OperacionActual->Tipo_Elemento = '1';
            strcpy(OperacionActual->nombre, PathAux);
            strcpy(OperacionActual->fecha, FechaYHoraActual());
            strcpy(OperacionActual->propietario, UsuarioActual->Nombre);
            OperacionActual->permisos = 664;

            NuevoOperacionJournaling(DISCO, UsuarioActual->InicioParticion);

            printf("Se creo el archivo \"%s\"\n\n",PathAux);
        }
        else
        {
            printf("EL usuario no tiene permiso de escritura en la carpeta \"%s\".\n\n", ListadoCarpetas[No_Carpetas - 2].Nombre);
            free(SB);
            fclose(DISCO);
            return;
        }
    }
    else
    {
        printf("No se puede crear el archivo porque faltan 1 o mas carpetas en la ruta.\n\n");
    }

    free(SB);
    fclose(DISCO);
}

void CrearArchivo(FILE *DISCO, SuperBloque *SB, int Padre, char Nombre[], char Size[], char Cont[])
{
    int TamanioArchivo = 0;
    char ContenidoSize[100] = "\0";

    if(Size[0] != '\0')
    {
        TamanioArchivo = atoi(Size);
        char j = '0';

        for(int i = 0 ; i < TamanioArchivo ; i++)
        {
            ContenidoSize[i] = j;
            j++;
            if(j == ':')
            {
                j = '0';
            }
        }
    }

    if(Cont[0] != '\0')
    {
        FILE *F;

        F = fopen(Cont,"r+");
        if(F != NULL)
        {
            char linea[500] = "\0";
            while(fgets(linea, sizeof linea, F) != NULL)
            {
                strcat(ContenidoSize, linea);
            }
            fclose(F);
        }
        else
        {
            printf("El archivo referenciado en el parametro Cont no se pudo abrir.\n\n");
        }
    }

    TamanioArchivo = 0;
    while(ContenidoSize[TamanioArchivo] != '\0')
    {
        TamanioArchivo++;
    }



    strcat(ContenidoSize, "\0");


    /**Ya teniendo el contenido del archivo y su tamaño procedemos a crear el inodo y los bloques nesesarios**/
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


    /**Se busca la aposicion del inodo y el bloque para el nuevo archivo y se marcan en los bitmap********************/

    int PosHijoInodo = BuscarPosicionInodo(DISCO, SB, 1);
    fseek(DISCO, SB->inicio_BM_Inodos + PosHijoInodo, SEEK_SET);
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
        PosBloquePadre = BuscarPosicionBloques(DISCO, SB, 1);
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
            if(InodoPadre->ap_Bloques[i] == -1)
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
    InodoHijo->ID_Grupo = UsuarioActual->ID_Grupo;
    InodoHijo->Tamanio = TamanioArchivo;

    strcpy(InodoHijo->fecha_creacion, FechaYHoraActual());
    strcpy(InodoHijo->fecha_lectura, FechaYHoraActual());
    strcpy(InodoHijo->fecha_modificacion, FechaYHoraActual());

    int conteo = 0;
    int bandera = 0;
    int BloquesNesesarios = 1;

    while(ContenidoSize[conteo] != '\0')
    {
        conteo++;
        bandera++;
        if(bandera == 63)
        {
            BloquesNesesarios++;
            bandera = 0;
        }
    }

    BloqueArchivo Bloques[12];
    bandera = 0;
    int bandera2 = 0;

    for(int i = 0 ; i < BloquesNesesarios ; i++)
    {
        for(int k = 0 ; k < 64 ; k++)
        {
            Bloques[i].Contenido[k] = ContenidoSize[bandera];
            bandera++;
            if(bandera == TamanioArchivo)
            {
                bandera2 = k + 1;
                break;
            }
        }
        if(bandera == TamanioArchivo)
        {
            for(int k = bandera2 ; k < 64 ; k++)
            {
                Bloques[i].Contenido[k] = '\0';
            }
            break;
        }
    }

    int PosHijoBloque = BuscarPosicionBloques(DISCO, SB, BloquesNesesarios);
    for(int i = 0 ; i < BloquesNesesarios ; i++)
    {
        fseek(DISCO, SB->inicio_BM_Bloques + PosHijoBloque + i, SEEK_SET);
        fwrite("1", sizeof(char), 1, DISCO);
    }

    for(int i = 0 ; i < BloquesNesesarios ; i++)
    {
        InodoHijo->ap_Bloques[i] = PosHijoBloque + i;

        fseek(DISCO, SB->inicio_Bloques + InodoHijo->ap_Bloques[i]*sizeof(BloqueArchivo), SEEK_SET);
        fwrite(&Bloques[i], sizeof(BloqueArchivo), 1, DISCO);

        SB->free_Bloques_count--;
    }

    for(int i = BloquesNesesarios ; i < 15 ; i++)
    {
        InodoHijo->ap_Bloques[i] = -1;
    }

    InodoHijo->tipo = '1';
    InodoHijo->Permisos = 664;

    /**Escribimos en el disco el nuevo inodo ***********************************************************/

    fseek(DISCO, SB->inicio_Inodos + PosHijoInodo*sizeof(Inodo), SEEK_SET);
    fwrite(InodoHijo, sizeof(Inodo), 1, DISCO);



    free(InodoPadre);
    free(BloquePadre);
    free(InodoHijo);

    return PosHijoInodo;
}

void EJecutarCAT(char Path[], int reporte, char Ruta[])
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

    int ExisteRuta = 1;

    for(int i = 0 ; i < No_Carpetas; i++)
    {
        Padre = Hijo;
        Hijo = ExisteHijo(DISCO, SB, Padre, ListadoCarpetas[i].Nombre);
        if(Hijo == -1)/**La carpeta no existe entonces se crea**/
        {
            ExisteRuta = 0;
            break;
        }
    }


    /**Se verifica si la ruta para crear el archivo existe y de ser asi se procede a crear el archivo**/

    if(ExisteRuta == 1)
    {
        Padre = Hijo;
        if(reporte == 0)
        {
            MostrarContenidoArchivo(DISCO, SB, Padre, ListadoCarpetas[No_Carpetas - 1].Nombre);
        }
        else if (reporte == 1)
        {
            GenerarReporteFile(DISCO, SB, Padre, ListadoCarpetas[No_Carpetas - 1].Nombre, Ruta);
        }

    }
    else
    {
        printf("No se puede leer el archivo porque faltan 1 o mas carpetas en la ruta.\n\n");
    }

    free(SB);
    fclose(DISCO);


}

void MostrarContenidoArchivo(FILE *DISCO, SuperBloque *SB, int Padre, char Nombre[])
{
        struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
        memset(InodoTMP, 0, sizeof(Inodo));

        struct BloqueArchivo *BloqueTMP = (struct BloqueArchivo*)malloc(sizeof(struct BloqueArchivo));
        memset(BloqueTMP, 0, sizeof(BloqueArchivo));

        fseek(DISCO, SB->inicio_Inodos + Padre*sizeof(Inodo), SEEK_SET);
        fread(InodoTMP, sizeof(Inodo), 1, DISCO);


        for(int i = 0 ; i < 12 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {
                fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueArchivo), SEEK_SET);
                fread(BloqueTMP, sizeof(BloqueArchivo), 1, DISCO);

                printf("%s",BloqueTMP->Contenido);
            }
        }

        printf("\n");

        free(BloqueTMP);
        free(InodoTMP);
}

void GenerarReporteFile(FILE *DISCO, SuperBloque *SB, int Padre, char Nombre[], char Destino[])
{
        struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
        memset(InodoTMP, 0, sizeof(Inodo));

        struct BloqueArchivo *BloqueTMP = (struct BloqueArchivo*)malloc(sizeof(struct BloqueArchivo));
        memset(BloqueTMP, 0, sizeof(BloqueArchivo));

        fseek(DISCO, SB->inicio_Inodos + Padre*sizeof(Inodo), SEEK_SET);
        fread(InodoTMP, sizeof(Inodo), 1, DISCO);

        FILE *TXT;
        TXT = fopen(Destino, "w");
        if(TXT != NULL)
        {
            for(int i = 0 ; i < 12 ; i++)
            {
                if(InodoTMP->ap_Bloques[i] != -1)
                {
                    fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueArchivo), SEEK_SET);
                    fread(BloqueTMP, sizeof(BloqueArchivo), 1, DISCO);

                    fprintf(TXT, "%s",BloqueTMP->Contenido);
                }
            }

            fclose(TXT);
            printf("Se genero el reporte FILE correctamente.\n\n");
        }

        free(BloqueTMP);
        free(InodoTMP);
}
