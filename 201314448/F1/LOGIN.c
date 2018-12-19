#include "LOGIN.h"

void EjecutarLogin(char Usuario[], char Pass[], char pID[])
{
    extern UsuarioLogeado *UsuarioActual;

    if(UsuarioActual->Estado == '1')
    {
        printf("Ya esta logeado con el usuario \"%s\" en este momento.\n\n",UsuarioActual->Nombre);
        return;
    }

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
            VerificarUsuario(DISCO, aux->inicio, Usuario, Pass, PathDisco);

            fclose(DISCO);
        }
        else
        {
            printf("El disco \"%s\" no se pudo abrir, problablemente no exista.\n\n",PathDisco);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar el login.\n", pID);
    }
}

void VerificarUsuario(FILE *DISCO, int InicioParticion,char Usuario[], char Pass[], char Path[])
{
    extern UsuarioLogeado *UsuarioActual;
    char Contenido[280320] = "\0";

    struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
    fseek(DISCO, InicioParticion, SEEK_SET);
    fread(SB, sizeof(SuperBloque), 1, DISCO);

    struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoTMP, 0, sizeof(Inodo));

    struct BloqueArchivo *BloqueTMP = (struct BloqueArchivo*)malloc(sizeof(struct BloqueArchivo));
    memset(BloqueTMP, 0, sizeof(BloqueArchivo));

    fseek(DISCO, SB->inicio_Inodos + sizeof(Inodo), SEEK_SET);
    fread(InodoTMP, sizeof(Inodo), 1, DISCO);



    for(int i = 0 ; i < 12 ; i++)
    {
        if(InodoTMP->ap_Bloques[i] != -1)
        {
            fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueArchivo), SEEK_SET);
            fread(BloqueTMP, sizeof(BloqueArchivo), 1, DISCO);

            strcat(Contenido, BloqueTMP->Contenido);
        }
    }

    /**En este punto ya se tiene el contenido del archivo users.txt en el char contenido***/

    char *Elemento = strtok(Contenido, "\n");

    int ID;
    char Tipo;
    char Grupo[10] = "\0";
    char NombreU[10] = "\0";
    char PassU[10] = "\0";

    char Lineas[100][20];



    int encontrado = 0;
    int incorrecto = 0;
    /**Leemos cada linea del archivo users.txt**/


    strcpy(Lineas[0],"\0");

    Elemento = strtok(NULL, "\n");

    int i = 1;

    while(Elemento != NULL)
    {
        strcpy(Lineas[i],Elemento);
        Elemento = strtok(NULL, "\n");
        i++;
    }

    for(i = 1 ; i < 100 ;i++)
    {
        Elemento = strtok(Lineas[i],",");
        while(Elemento != NULL)
        {
            ID = atoi(Elemento);
            Elemento = strtok(NULL,",");
            Tipo = Elemento[0];
            if(Tipo == 'U')
            {
                Elemento = strtok(NULL,",");
                strcpy(Grupo,Elemento);
                Elemento = strtok(NULL,",");
                strcpy(NombreU,Elemento);
                Elemento = strtok(NULL,",");
                strcpy(PassU,Elemento);

                if(strcmp(Usuario,NombreU) == 0)
                {
                    if(strcmp(Pass,PassU) == 0)
                    {
                        UsuarioActual->Estado = '1';
                        UsuarioActual->ID = ID;
                        strcpy(UsuarioActual->Grupo, Grupo);
                        strcpy(UsuarioActual->Nombre, NombreU);
                        strcpy(UsuarioActual->Pass, PassU);
                        strcpy(UsuarioActual->PathDisco,Path);
                        UsuarioActual->InicioParticion = InicioParticion;
                        printf("Se logeo correctamente con el usuario \"%s\".\n\n",Usuario);
                        encontrado = 1;
                        break;
                    }
                    else
                    {
                        printf("La contraseña para el usuario \"%s\" es incorrecta\n\n",NombreU);
                        incorrecto = 1;
                        break;
                    }
                }
                else
                {
                    break;
                }

            }
            else
            {
                break;
            }
        }

        if(encontrado == 1 || incorrecto == 1)
        {
            break;
        }
    }

    if(encontrado == 0 && incorrecto == 0)
    {
        printf("El usuario no existe.\n\n");
    }


    free(BloqueTMP);
    free(InodoTMP);
    free(SB);
}
