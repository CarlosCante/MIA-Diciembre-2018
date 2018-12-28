#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Analizador.h"
#include "Estructuras.h"

struct ListaMomunt *LISTADO;
struct UsuarioLogeado *UsuarioActual;
struct Journaling *OperacionActual;

int main()
{
    char Comando[150] = "\0";

    LISTADO = (struct ListaMomunt*)malloc(sizeof(struct ListaMomunt));
    UsuarioActual = (struct UsuarioLogeado*)malloc(sizeof(struct UsuarioLogeado));
    OperacionActual = (struct Journaling*)malloc(sizeof(struct Journaling));

    OperacionActual->Tipo_Operacion = '\0';
    OperacionActual->Tipo_Elemento = 'N';
    strcpy(OperacionActual->nombre, "\0");
    strcpy(OperacionActual->contenido, "\0");
    strcpy(OperacionActual->fecha, "\0");
    strcpy(OperacionActual->propietario, "\0");
    OperacionActual->permisos = 0;





    while(1)
    {
        printf("Comando->   ");
        fgets(Comando, 150, stdin);

        if(EjecutarComando(Comando, 0) == 1)
        {
            break;
        }

    }

    free(LISTADO);
    free(UsuarioActual);
    free(OperacionActual);
    return 0;
}
