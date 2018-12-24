#include "MKFILE.h"

void EjecutarMKFILE(char Path[], char P, char pSize[], char Cont[])
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

    if(P == 'S')/**Se deben de crear todas las carpetas**/
    {
        for(int i = 0 ; i < No_Carpetas - 1 ; i++)
        {
            Padre = Hijo;
            Hijo = ExisteHijo(DISCO, SB, Padre, ListadoCarpetas[i].Nombre);
            if(Hijo == -1)/**La carpeta no existe entonces se crea**/
            {
                Hijo = CrearCarpeta(DISCO, SB, Padre, ListadoCarpetas[i].Nombre);
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

        CrearArchivo(DISCO, SB, Padre, ListadoCarpetas[No_Carpetas - 1].Nombre, pSize, Cont);
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

    if(Cont[0] != "\0")
    {
        FILE *F;

        F = fopen(strtok(cont,"\n"),"r+");
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

    /**Ya teniendo el contenido del archivo y su tamaño procedemos a crear el inodo y los bloques nesesarios**/





}
