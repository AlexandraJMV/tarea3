#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"
#include "treemap.h"

typedef struct Palabra{
    char pal[MAXCHAR];
    int ocurrencia;
}Palabra;

// Impresion de menu de opciones para programa
void print_menu()
{
    printf( "Libreria\n\n"

            "1. Cargar archivos.\n"
            "7. Salir del programa"

            "\nIngrese lo que desea hacer:\n\n");
}

// transforma una cadena leida a una seleccion valida.
// falla si lo primero que se ingresa es \n. Arreglar.
int toselect(char * str)
{
    for(int i=0 ; str[i] != '\0' ; i++)
    {
        if(str[i]=='\n') break;
        if(str[i]<'1' || str[i]>'9')
            return 0;
    }
    return atoi(str);
}

// A partir de un puntero a una posicion obtinene el siguiente nombre de archivo.
void get_doc(char * str, char * ret , int * prev_pos)
{
    int ispace = 0;
    int i = 0;

    if (str[*prev_pos] == ' ')
        ispace = 1;
    
    while(str[*prev_pos] != '\0' && str[*prev_pos] != '\n')
    {
        if (ispace == 0 && str[*prev_pos] == ' ')
            break;
        
        if (ispace == 1 && str[*prev_pos] != ' ')
            ispace = 0;

        if (ispace == 0)
        {
            ret[i] = str[*prev_pos];
            i++;
        }
        
        (*prev_pos)++;
    }
    ret[i] = '\0';
    return;
}

int importar(char * arch)
{
    FILE * entrada;
    char path[MAXCHAR] = "Libros/";
    char endpath[6] = ".txt";

    strcat(path, arch);
    strcat(path, endpath);

    entrada = fopen(path, "rt");
    if(entrada==NULL) 
    {
        return 0;
    }

    fclose(entrada);
    return 1;
}

// Imput de documentos, importacion individual documento a documento.
void cargar_docs()
{
    int pos = 0;
    int cont = 0;
    char docs[MAXCHAR];

    printf("Ingrese los archivos que desea importar, separados por espacios\n");
    fgets(docs, MAXCHAR, stdin);//
    
    
    while (1)
    {
        char doc[MAXCHAR];
        get_doc(docs, doc, &pos);
        if (doc[0] =='\0') break;

        if (importar(doc))
            cont++;
    }

    printf("Se han importado %d documentos de forma exitosa!\n", cont);
    return;
}