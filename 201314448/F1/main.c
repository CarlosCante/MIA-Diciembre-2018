#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Analizador.h"
#include "Estructuras.h"

struct ListaMomunt *LISTADO;
struct UsuarioLogeado *UsuarioActual;

int main()
{
    char Comando[150] = "\0";

    LISTADO = (struct ListaMomunt*)malloc(sizeof(struct ListaMomunt));
    UsuarioActual = (struct UsuarioLogeado*)malloc(sizeof(struct UsuarioLogeado));

    while(1)
    {
        printf("Comando->   ");
        fgets(Comando, 150, stdin);

        if(EjecutarComando(Comando) == 1)
        {
            break;
        }

    }

    return 0;
}
