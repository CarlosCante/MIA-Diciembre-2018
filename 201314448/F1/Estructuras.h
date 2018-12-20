#ifndef ESTRUCTURAS_H_INCLUDED
#define ESTRUCTURAS_H_INCLUDED
#include <time.h>

typedef struct MBR MBR;
typedef struct Particion Particion;
typedef struct EBR EBR;
typedef struct NodoParticion NodoParticion;
typedef struct ListaMomunt ListaMomunt;
typedef struct SuperBloque SuperBloque;
typedef struct Journaling Journaling;
typedef struct Inodo Inodo;
typedef struct BloqueCarpeta BloqueCarpeta;
typedef struct content content;
typedef struct BloqueArchivo BloqueArchivo;
typedef struct BloqueApuntadores BloqueApuntadores;
typedef struct UsuarioLogeado UsuarioLogeado;

struct UsuarioLogeado
{
    char Estado;
    char Nombre[10];
    char Pass[10];
    int ID;
    char Grupo[10];
    char PathDisco[500];
    int InicioParticion;
    char ajuste;
};

struct BloqueApuntadores
{
    int Apuntadores[16];
};

struct BloqueArchivo
{
    char Contenido[64];
};

struct content
{
    char nombre[12];
    int ap_inodo;
};

struct BloqueCarpeta
{
    content b_content[4];
};

struct Inodo
{
    int ID_Usuario;
    int ID_Grupo;
    int Tamanio;
    char fecha_lectura[17];
    char fecha_creacion[17];
    char fecha_modificacion[17];
    int ap_Bloques[15];
    char tipo;
    int Permisos;
};


struct Journaling
{
    char Tipo_Operacion;
    char Tipo_Elemento;
    char nombre[12];
    char contenido[64];
    char fecha[17];
    char propietario[10];
    char permisos[3];
};

struct SuperBloque
{
    int Filesystem_Type;
    int Inodos_count;
    int Bloques_count;
    int free_Inodos_count;
    int free_Bloques_count;
    char fecha_mount[17];
    char fecha_unmount[17];
    int mount_count;
    int magic_number;
    int Inodo_size;
    int Bloque_size;
    int first_free_Inodo;
    int first_free_Bloque;
    int inicio_BM_Inodos;
    int inicio_BM_Bloques;
    int inicio_Inodos;
    int inicio_Bloques;
};

struct NodoParticion
{
    char Path[100];
    char Nombre[16];
    char ID[5];
    char ajuste;
    int inicio;
    NodoParticion *siguiente;
    NodoParticion *anterior;
};

struct ListaMomunt
{
    int cantidad;
    NodoParticion *primero;
    NodoParticion *ultimo;
};

struct EBR
{
    char Status;
    char Ajuste;
    int Inicio;
    int Tamanio;
    int Ap_Sig;
    char Nombre[16];
};

struct Particion
{
    char Status;
    char Tipo;
    char Ajuste;
    int Inicio;
    int Tamanio;
    char Nombre[16];
};

struct MBR
{
    int Tamanio;
    char FechaCreacion[17];
    int disk_asignature;
    char Ajuste;
    Particion Particiones[4];
};

/**Area de structs del comando mount, conta del nodo de la lista y la lista en si de las particiones montadas**/




#endif // ESTRUCTURAS_H_INCLUDED
