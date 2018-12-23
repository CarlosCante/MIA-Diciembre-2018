#include "MKDIR.h"

void EjecutarMKDIR(char Path[], char P)
{
    extern UsuarioLogeado *UsuarioActual;

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
    }
    else if(P == 'N')/**Solo se debe crear la ultima carpeta**/
    {

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
    extern UsuarioLogeado *UsuarioActual;
    int PosHijoInodo = BuscarPosicionInodo(DISCO, SB);
    int PosHijoBloque = BuscarPosicionBloques(DISCO, SB, 1);

    int PosPadreInodo = Padre;
    int PosPadreBloque = -1;


    /**Obtenemos las estructuras pertenecientes al padre**/

    struct Inodo *InodoPadre = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoPadre, 0, sizeof(Inodo));
    fseek(DISCO, SB->inicio_Inodos + Padre*sizeof(Inodo), SEEK_SET);

    struct BloqueCarpeta *BloquePadre = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
    memset(BloquePadre, 0, sizeof(BloqueCarpeta));

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
                    BloquePadre->
                }
            }
        }
    }



     /**Creamos el inodo de la carpeta y llenamos sus atributos***************************************************/

    fseek(DISCO, SB->inicio_BM_Inodos + PosHijoInodo, SEEK_SET);
    fwrite("1", sizeof(char), 1, DISCO);

    struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoTMP, 0, sizeof(Inodo));

    InodoTMP->ID_Usuario = UsuarioActual->ID;
    InodoTMP->ID_Grupo = UsuarioActual->ID;
    InodoTMP->Tamanio = 0;

    strcpy(InodoTMP->fecha_creacion, FechaYHoraActual());
    strcpy(InodoTMP->fecha_lectura, FechaYHoraActual());
    strcpy(InodoTMP->fecha_modificacion, FechaYHoraActual());

    InodoTMP->ap_Bloques[0] = PosHijoBloque;

    for(int i = 1 ; i < 15 ; i++)
    {
        InodoTMP->ap_Bloques[i] = -1;
    }

    InodoTMP->tipo = '0';
    InodoTMP->Permisos = 664;

    /**Creamos el bloque asociado a la carpeta **************************************************************/

    fseek(DISCO, SB->inicio_BM_Bloques + PosHijoBloque, SEEK_SET);
    fwrite("1", sizeof(char), 1, DISCO);

    struct BloqueCarpeta *Bloque = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
    memset(Bloque, 0, sizeof(BloqueCarpeta));

    strcpy(Bloque->b_content[0].nombre, Nombre);
    Bloque->b_content[0].ap_inodo = PosHijoInodo;

    strcpy(Bloque->b_content[1].nombre, "");
    Bloque->b_content[1].ap_inodo = Padre;

    strcpy(Bloque->b_content[2].nombre, "");
    Bloque->b_content[2].ap_inodo = -1;

    strcpy(Bloque->b_content[3].nombre, "");
    Bloque->b_content[3].ap_inodo = -1;



    free(InodoPadre);
    free(InodoTMP);
    free(Bloque);

}

int BuscarPosicionInodo(FILE *DISCO, SuperBloque *SB)
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

            if(Conteo < TamMin)
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

            if(Conteo > TamMin)
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
