#include "ABC_GU.h"

void EjecutarMKGRP(char name[])
{
    extern UsuarioLogeado *UsuarioActual;

    if(strcmp(UsuarioActual->Nombre, "root") == 0)
    {
        int Existe = VerificarSiGrupoExiste(name);
        if(Existe == 0)
        {
            CrearGrupo(name);
        }
        else
        {
            printf("EL grupo \"%s\" ya existe dentro de la particion.\n\n",name);
        }
    }
    else
    {
        printf("No puede crear un grupo porque no es el usurio root.\n\n");
    }
}

void CrearGrupo(char name[])
{
    extern UsuarioLogeado *UsuarioActual;
    char Contenido[280320] = "\0";
    char ContenidoAux[280320] = "\0";

    FILE *DISCO;
    DISCO = fopen(UsuarioActual->PathDisco, "r+b");

    struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
    fseek(DISCO, UsuarioActual->InicioParticion, SEEK_SET);
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

    strcpy(ContenidoAux,Contenido);

    /**Leemos cada linea del archivo users.txt**/

    char *Elemento = strtok(ContenidoAux, "\n");
    char Tipo;
    char Grupo[10] = "\0";
    typedef struct Linea
    {
        char Conten[20];
    }Linea;

    Linea Lineas[100];
    int j = 0;

    while(Elemento != NULL)
    {
        strcpy(Lineas[j].Conten,Elemento);
        Elemento = strtok(NULL, "\n");
        j++;
    }

    /**Ya teniendo el contenido del archivo users.txt procedemos a ver que id colocar**/

    int IDAux = 1;
    int IDGrupo = 1;

    for(int i = 0 ; i < j ;i++)
    {
        Elemento = strtok(Lineas[i].Conten,",");
        while(Elemento != NULL)
        {
            IDAux = atoi(Elemento);
            Elemento = strtok(NULL,",");
            Tipo = Elemento[0];
            if(Tipo == 'G')
            {
                IDGrupo = IDAux;
                break;
            }
            else
            {
                break;
            }
        }
    }

    IDGrupo++;

    /**Aca ya tenemos el id que colocaremos al nuevo grupo**/

    char buffer[3] = "\0";
    sprintf(buffer,"%d",IDGrupo);

    strcat(Contenido,buffer);
    strcat(Contenido,",G,");
    strcat(Contenido,name);
    strcat(Contenido,"\n");

    int conteo = 0;
    int bandera = 0;
    int BloquesNesesarios = 1;

    while(Contenido[conteo] != '\0')
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

    for(int i = 0 ; i < BloquesNesesarios ; i++)
    {
        for(int k = 0 ; k < 64 ; k++)
        {
            Bloques[i].Contenido[k] = Contenido[bandera];
            bandera++;
            if(bandera == conteo)
            {
                break;
            }
        }
        if(bandera == conteo)
        {
            break;
        }
    }

    for(int i = 0 ; i < BloquesNesesarios ; i++)
    {
        if(InodoTMP->ap_Bloques[i] != -1)
        {
            fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueArchivo), SEEK_SET);
            fwrite(&Bloques[i], sizeof(BloqueArchivo), 1, DISCO);
        }
        else
        {
            InodoTMP->ap_Bloques[i] = SB->first_free_Bloque;

            fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueArchivo), SEEK_SET);
            fwrite(&Bloques[i], sizeof(BloqueArchivo), 1, DISCO);

            fseek(DISCO, SB->inicio_BM_Bloques + SB->first_free_Bloque, SEEK_SET);
            fwrite("1", sizeof(char), 1, DISCO);

            SB->first_free_Bloque = ObtenerPrimerBloqueLibre(DISCO, SB);

        }
    }

    fseek(DISCO, UsuarioActual->InicioParticion, SEEK_SET);
    fwrite(SB, sizeof(SuperBloque), 1, DISCO);

    fseek(DISCO, SB->inicio_Inodos + sizeof(Inodo), SEEK_SET);
    fwrite(InodoTMP, sizeof(Inodo), 1, DISCO);



    free(BloqueTMP);
    free(InodoTMP);
    free(SB);

    fclose(DISCO);

    /*
    DISCO = fopen(UsuarioActual->PathDisco, "r+b");
    ComprobarUsersTXT(DISCO, UsuarioActual->InicioParticion);
    fclose(DISCO);
    */
}

int VerificarSiGrupoExiste(char name[])
{
    int resultado = 0;


    extern UsuarioLogeado *UsuarioActual;
    char Contenido[280320] = "\0";

    FILE *DISCO;
    DISCO = fopen(UsuarioActual->PathDisco, "r+b");

    struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
    fseek(DISCO, UsuarioActual->InicioParticion, SEEK_SET);
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

    /**Leemos cada linea del archivo users.txt**/

    char *Elemento = strtok(Contenido, "\n");
    char Tipo;
    char Grupo[10] = "\0";
    typedef struct Linea
    {
        char Conten[20];
    }Linea;

    Linea Lineas[100];
    int j = 0;

    while(Elemento != NULL)
    {
        strcpy(Lineas[j].Conten,Elemento);
        Elemento = strtok(NULL, "\n");
        j++;
    }

    for(int i = 0 ; i < j ;i++)
    {
        Elemento = strtok(Lineas[i].Conten,",");
        while(Elemento != NULL)
        {
            Elemento = strtok(NULL,",");
            Tipo = Elemento[0];
            if(Tipo == 'G')
            {
                Elemento = strtok(NULL,",");
                strcpy(Grupo,Elemento);
                if(strcmp(Grupo, name) == 0)
                {
                    resultado = 1;
                    break;
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
        if(resultado == 1)
        {
            break;
        }
    }


    free(BloqueTMP);
    free(InodoTMP);
    free(SB);

    fclose(DISCO);

    return resultado;
}

int ObtenerBloqueLibre(FILE *DISCO, SuperBloque *SB, int cantidad, char Ajuste)
{


    if(Ajuste == 'B')
    {

    }
    else if(Ajuste == 'F')
    {

    }
    else if(Ajuste == 'W')
    {

    }

    return -1;
}

int ObtenerPrimerBloqueLibre(FILE *DISCO, SuperBloque *SB)
{
    fseek(DISCO, SB->inicio_BM_Bloques, SEEK_SET);
    char C;

    for(int i = 0 ; i < SB->Bloques_count ; i++)
    {
        fread(&C, sizeof(char), 1, DISCO);
        if(C == '0')
        {
            return i;
        }
    }
    return -1;
}
