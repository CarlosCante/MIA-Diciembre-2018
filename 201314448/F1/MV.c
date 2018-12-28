#include "MV.h"



void EjecutarMV(char Path[], char Dest[])
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


    /**Se verifica si la ruta para crear el archivo existe y de ser asi se procede a la eliminacion del archivo**/

    if(ExisteRuta == 1)
    {
        int a = MoverInodo(DISCO, SB, Hijo, Dest);

        if(a == 1)
        {

            /**Se elimina la referencia en el padre del hijo**/
            EliminarRefernciaPadre(DISCO, SB, Padre, Hijo);

            /**Se eliminan los archivos y carpetas del hijo**/

            OperacionActual->Tipo_Operacion = '4';
            OperacionActual->Tipo_Elemento = '1';
            strcpy(OperacionActual->nombre, PathAux);
            strcpy(OperacionActual->fecha, FechaYHoraActual());
            strcpy(OperacionActual->propietario, UsuarioActual->Nombre);
            OperacionActual->permisos = 664;

            NuevoOperacionJournaling(DISCO, UsuarioActual->InicioParticion);

            printf("Se movio con exito el directorio o carpeta \"%s\".\n\n", PathAux);
        }
        else if(a == 0)
        {
            printf("No se pudo mover el directorio o archivo porque el destino probablemente no exista.\n\n");
        }
    }
    else
    {
        printf("No se puede leer el archivo porque faltan 1 o mas carpetas en la ruta.\n\n");
    }

    free(SB);
    fclose(DISCO);
}

int MoverInodo(FILE *DISCO, SuperBloque *SB, int Hijo, char Dest[])
{
    int Movio = 0;

    extern UsuarioLogeado *UsuarioActual;
    extern Journaling *OperacionActual;

    char PathAux[100] = "\0";
    strcpy(PathAux, Dest);

    /**Primero se dividen los nombres de los directorios en distintas cadenas**/

    typedef struct Carpetas
    {
        char Nombre[50];
    }Carpetas;

    Carpetas ListadoCarpetas[100];
    int No_Carpetas = 0;

    char *Elemento = strtok(Dest, "/");

    while(Elemento != NULL)
    {
        strcpy(ListadoCarpetas[No_Carpetas].Nombre, Elemento);
        Elemento = strtok(NULL, "/");
        No_Carpetas++;
    }

    int Padre = 0;
    int HijoT = 0;

    int ExisteRuta = 1;

    for(int i = 0 ; i < No_Carpetas; i++)
    {
        Padre = HijoT;
        HijoT = ExisteHijo(DISCO, SB, Padre, ListadoCarpetas[i].Nombre);
        if(HijoT == -1)/**La carpeta no existe entonces se crea**/
        {
            ExisteRuta = 0;
            Movio = 0;
            break;
        }
    }

    if(ExisteRuta == 1)
    {
        Padre = HijoT;
        AgregarAPadre(DISCO, SB, Padre, Hijo);
        Movio = 1;
    }


   return Movio;
}

void AgregarAPadre(FILE *DISCO, SuperBloque *SB, int Padre, int Hijo)
{
    extern UsuarioLogeado *UsuarioActual;
    //Carpeta Padre
    struct Inodo *InodoPadre = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoPadre, 0, sizeof(Inodo));

    struct BloqueCarpeta *BloquePadre = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
    memset(BloquePadre, 0, sizeof(BloqueCarpeta));

    fseek(DISCO, SB->inicio_Inodos + Padre*sizeof(Inodo), SEEK_SET);
    fread(InodoPadre, sizeof(Inodo), 1, DISCO);


    int EncontroEspacio = 0;
    int PosBloquePadre = -1;

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
                    strcpy(BloquePadre->b_content[j].nombre, "");
                    BloquePadre->b_content[j].ap_inodo = Hijo;
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


        strcpy(BloquePadre->b_content[0].nombre, "");
        BloquePadre->b_content[0].ap_inodo = Hijo;

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

}


