#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "funciones.h"
#include "treemap.h"
#include "hashmap.h"

typedef struct palabra{
    char palabra[MAXCHAR];
    long ocurrencia;
    float frecuencia;
}palabra;

typedef struct libro{
    char titulo[MAXCHAR];
    char book_id[MAXCHAR];
    TreeMap * pal_titulo;
    TreeMap * pal_libro;
    long pal_tot;
    long char_tot;
}libro;

typedef struct libreria{
    TreeMap * libros_ord;
    int libros_tot;
}libreria;

int lower_than_string(void* key1, void* key2){
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}

// Impresion de menu de opciones para programa
void print_menu()
{
    printf( "Libreria\n\n"

            "1. Cargar archivos.\n"
            "7. Salir del programa"

            "\nIngrese lo que desea hacer:\n\n");
}

// transforma una cadena leida a una seleccion valida.
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

// A partir de un puntero a una posicion obtinene una palabra separada por espacios.
void get_pal(char * str, char * ret , int * prev_pos)
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

const char * extract_title(char * lin)
{
    char tit[MAXCHAR];
    int i=0;
    int ini = strlen(FIRSTLIN_FORMAT)+3;
    
    char * p = strstr(lin, ", by");
    if(p)
    {
        for(ini ; ini<p-lin ; ini++)
        {
            tit[i] = lin[ini];
            i++;
        }
        tit[i] = '\0';

        return _strdup(tit);
    }

    char * nextline = strstr(lin, "\n");
    if(nextline)
    {
        lin[nextline-lin] = '\0';
    }
    return _strdup(lin+ini+4); // +4 para compensar la basura :(
}

void minusc(char * str)
{
    for(int i=0 ; str[i] !='\0' ; i++)
        str[i]= tolower(str[i]);
}

char * clean_pal(char *str)
{
    int start;
    int ind=0, iret  = 0;
    char ret[MAXCHAR];

    minusc(str);

    start = 0;

    for (ind ; str[ind] != '\0' && str[ind] != '\n' ; ind++)
    {
        if(start == 0 && isalpha(str[ind]) != 0 && ispunct(str[ind]) == 0) start = 1;

        if(start==1 && (isalpha(str[ind]) == 0 || ispunct(str[ind]) != 0)) break;

        if(start)
        {
            ret[iret] = str[ind];
            iret++;
        }
    }
    ret[iret] = '\0';
    return _strdup(ret);
}

palabra * create_palabra(char * str)
{
    palabra * pal = (palabra *)malloc(sizeof(palabra));
    if(pal == NULL){
        perror("!");
        return NULL;
    }
    strcpy(pal->palabra, str);
    pal->ocurrencia = 0;
    pal->frecuencia = 0;

    return pal;
}

void guardar_palabras(libro * lib, TreeMap * palabras, char *str)
{
    int cont = 0;
    int pos=0;
    while (1)
    {
        cont++;
        char pal[MAXCHAR];
        char clean[MAXCHAR];
        palabra * p;

        get_pal(str, pal, &pos);
        if (pal[0] =='\0') break;
        
        strcpy(clean, clean_pal(pal));

        if( clean[0]!='\0' )
        {
            minusc(clean);
            TreePair * par = searchTreeMap(palabras, clean);
            if (par == NULL)
            {
                p = create_palabra(clean);
                p->ocurrencia = 1;
                if(lib->pal_tot != 0)
                    p->frecuencia = (float) 1 / (float) lib->pal_tot;
                
                insertTreeMap(palabras, clean, p);
            }
            else
            {
                p = (palabra *) par->value;
                p->ocurrencia++;
                p->frecuencia = (float) p->ocurrencia / (float) lib->pal_tot;
            }
            lib->pal_tot++;
            lib->char_tot = lib->char_tot + strlen(clean);
        }
    }
}

libro* create_book(char * id)
{
    libro * lb;
    lb = (libro *)malloc(sizeof(libro));
    if (lb == NULL)
    {
        perror("No se pudo reservar memoria para el libro! ");
        exit(1);
    }

    lb->pal_titulo = createTreeMap(lower_than_string);
    lb->pal_libro = createTreeMap(lower_than_string);
    lb->pal_tot = 0;
    lb->char_tot  = 0;

    strcpy(lb->book_id, id);
    return lb;
}

libro* read_book(char * arch, FILE * file)
{
    libro * lb;
    char linea[MAXLIN];
    char aux_titl[MAXLIN];
    int lin = 0, start=0;

    lb = create_book(arch);

    while(fgets(linea, MAXLIN, file) != NULL)
    {
        if (lin == 0) { // Extrae titulo desde la primera linea
            if(linea[0] == '\n' || linea[0] == '\0')
            {
                strcpy(lb->titulo, extract_title(aux_titl)); //guardar_palabras(lb->pal_titulo, lb->pal_titulo);
                guardar_palabras(lb, lb->pal_titulo, lb->titulo);
                lin = 1;
                continue;
            }

            char * p = strstr(aux_titl,"\n");
            if(p)
            {
                aux_titl[p-aux_titl] = ' ';
            }
            strcat(aux_titl, linea);
        }

        if (start != 1) // Encuentra el simbolo *** paraempezar con la lectura de palabras del texto.
        {
            if (strstr(linea, STRT_FORMAT) != NULL)
                start = 1;
            continue;
        }

        if (start == 1) // Encuentra *** para terminar la lectura
            if (strstr(linea, END_FORMAT) != NULL)
                break;

        guardar_palabras(lb, lb->pal_libro, linea);
    }
    return lb;
}

libro* importar(char * arch)
{
    FILE * entrada;
    libro * book;
    char path[MAXCHAR] = "Libros/";

    strcat(path, arch);

    entrada = fopen(path, "rt");
    if(entrada==NULL) 
    {
        return NULL;
    }

    book = read_book(arch, entrada);
    fclose(entrada);
    return book;
}

// Imput de documentos, importacion individual documento a documento.
void cargar_docs(libreria * Libreria)
{
    int pos = 0;
    int cont = 0;
    char docs[MAXCHAR];

    printf("Ingrese los archivos que desea importar, separados por espacios\n");
    fgets(docs, MAXCHAR, stdin);
    
    while (1)
    {
        char doc[MAXCHAR];
        get_pal(docs, doc, &pos);
        if (doc[0] =='\0') break;

        libro * lib = importar(doc);
        if (lib != NULL)
        {
            insertTreeMap(Libreria->libros_ord, lib->titulo, lib);
            Libreria->libros_tot ++;
            cont++;
        }
    }

    printf("Se han importado %d documentos de forma exitosa!\n", cont);
    return;
}

libreria * create_libreria()
{
    libreria * lib = (libreria *)malloc(sizeof(libreria));
    if(lib==NULL)
    {
        perror("!");
        return NULL;
    }

    lib->libros_ord = createTreeMap(lower_than_string);
    lib->libros_tot=0;

    return lib;
}

void printlibrostest(libreria * Libreria)
{
    TreePair * lib = firstTreeMap(Libreria->libros_ord);
    while(lib != NULL)
    {
        libro * book = (libro*) lib->value;
        printf("valor es %s\n", book->titulo);
        printf("palabras y caracteres son %ld %ld\n\n",book->pal_tot, book->char_tot);
        lib = nextTreeMap(Libreria->libros_ord);
    }
}

void printpaltest(libreria* libreria)
{
    int cont = 0;
    TreePair * treepar = firstTreeMap(libreria->libros_ord);
    libro* lib = (libro *)treepar->value;
    printf("NWE VERSION!!!!!!1   %s\n\n",lib->titulo);
    TreePair * par = firstTreeMap(lib->pal_libro);
    if(par==NULL) printf("La COSA. esta vacia\n");
    while(par!=NULL && cont<100)
    {
        palabra* pal = (palabra*) par->value;
        printf("palabra: %s\n", pal->palabra);
        printf("ocurrencia = %ld\n\n", pal->ocurrencia);
        par = nextTreeMap(lib->pal_libro);
        cont++;
    }
}