#include <stdio.h>
#include <stdlib.h>
#include "funciones.h"
#include "treemap.h"

typedef struct Palabra{
    char pal[MAXCHAR];
    int ocurrencia;
}Palabra;

/*1. Cargar documentos. El usuario coloca los nombres de una 
lista de archivos txt separados por espacio y la aplicación los carga.*/
void cargar_docs()
{
    char docs[MAXCHAR];
    fgets(docs, MAXCHAR, stdin);
    return;
}