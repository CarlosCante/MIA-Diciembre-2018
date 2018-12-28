#include "REM.h"

void EjecutarREM(char Path[])
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
        /**Se elimina la referencia en el padre del hijo**/
        EliminarRefernciaPadre(DISCO, SB, Padre, Hijo);

        /**Se eliminan los archivos y carpetas del hijo**/
        EliminarInodoYBloques(DISCO, SB, Hijo);

        OperacionActual->Tipo_Operacion = '4';
        OperacionActual->Tipo_Elemento = '1';
        strcpy(OperacionActual->nombre, PathAux);
        strcpy(OperacionActual->fecha, FechaYHoraActual());
        strcpy(OperacionActual->propietario, UsuarioActual->Nombre);
        OperacionActual->permisos = 664;

        NuevoOperacionJournaling(DISCO, UsuarioActual->InicioParticion);

        printf("Se elimino con exito el directorio o carpeta \"%s\".\n\n", PathAux);
    }
    else
    {
        printf("No se puede leer el archivo porque faltan 1 o mas carpetas en la ruta.\n\n");
    }

    free(SB);
    fclose(DISCO);
}

void EliminarInodoYBloques(FILE *DISCO, SuperBloque *SB, int AP_Inodo)
{
    extern UsuarioLogeado *UsuarioActual;

    struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoTMP, 0, sizeof(Inodo));

    struct BloqueArchivo *BloqueA_TMP = (struct BloqueArchivo*)malloc(sizeof(struct BloqueArchivo));
    memset(BloqueA_TMP, 0, sizeof(BloqueArchivo));

    struct BloqueCarpeta *BloqueC_TMP = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
    memset(BloqueC_TMP, 0, sizeof(BloqueCarpeta));

    fseek(DISCO, SB->inicio_Inodos + AP_Inodo*sizeof(Inodo), SEEK_SET);
    fread(InodoTMP, sizeof(Inodo), 1, DISCO);
    fseek(DISCO, SB->inicio_BM_Inodos + AP_Inodo, SEEK_SET);
    fwrite("0", sizeof(char), 1, DISCO);
    SB->free_Inodos_count++;


    if(InodoTMP->tipo == '0')
    {
        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {
                fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueCarpeta), SEEK_SET);
                fread(BloqueC_TMP, sizeof(BloqueCarpeta), 1, DISCO);
                fseek(DISCO, SB->inicio_BM_Bloques + InodoTMP->ap_Bloques[i], SEEK_SET);
                fwrite("0", sizeof(char), 1, DISCO);
                SB->free_Bloques_count++;
                SB->first_free_Bloque = ObtenerPrimerBloqueLibre(DISCO, SB);

                for(int k = 0 ; k < 4 ; k++)
                {
                    if(BloqueC_TMP->b_content[k].ap_inodo != -1)
                    {
                        if(i != 0 || (i==0 && k != 0 && k != 1))
                        {
                            EliminarInodoYBloques(DISCO, SB, BloqueC_TMP->b_content[k].ap_inodo);
                        }
                    }
                }
            }
        }
    }
    else if(InodoTMP->tipo == '1')
    {
        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {
                fseek(DISCO, SB->inicio_BM_Bloques + InodoTMP->ap_Bloques[i], SEEK_SET);
                fwrite("0", sizeof(char), 1, DISCO);
                SB->free_Bloques_count++;
                SB->first_free_Bloque = ObtenerPrimerBloqueLibre(DISCO, SB);
            }
        }
    }

    fseek(DISCO, UsuarioActual->InicioParticion, SEEK_SET);
    fwrite(SB, sizeof(SuperBloque), 1, DISCO);

    free(InodoTMP);
    free(BloqueA_TMP);
    free(BloqueC_TMP);
}


void EliminarRefernciaPadre(FILE *DISCO, SuperBloque *SB, int Padre, int Hijo)
{
    extern UsuarioLogeado *UsuarioActual;

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
                    if(BloqueTMP->b_content[j].ap_inodo == Hijo)
                    {
                        Respuesta = BloqueTMP->b_content[j].ap_inodo;
                        BloqueTMP->b_content[j].ap_inodo = -1;
                        strcpy(BloqueTMP->b_content[j].nombre, "AP");

                        fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueCarpeta), SEEK_SET);
                        fwrite(BloqueTMP, sizeof(BloqueCarpeta), 1, DISCO);

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

    strcpy(InodoTMP->fecha_lectura, FechaYHoraActual());
    fseek(DISCO, SB->inicio_Inodos + sizeof(Inodo)*Padre, SEEK_SET);
    fwrite(InodoTMP, sizeof(Inodo), 1, DISCO);

    free(InodoTMP);
    free(BloqueTMP);

    return Respuesta;
}
