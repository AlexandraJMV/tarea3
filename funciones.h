#ifndef Funciones_h
#define Funciones_h
#define MAXCHAR 400
#define MAXLIN 600
#define STRT_FORMAT "*** START OF"
#define END_FORMAT "*** END OF"
#define TITLE_FORMAT "Title:"
#define LASTLIN_FORMAT ", by"
#define PATH "Libros/"

enum op1{cargar=1, mostrar_ordenado, buscar_titulo, mayor_frecuencia,relevancia_Palabra, buscar_palabra, cntxt, end };

typedef struct libreria libreria;

void print_menu();

void cargar_docs(libreria * lib);

int toselect(char * str);

libreria * create_libreria();

void printlibrostest(libreria * Libreria);

void printpaltest(libreria* libreria);

void top_frecuencia(libreria * lib);

void buscarPalabra(libreria * lib);

float relevancia_palabra(libreria * lib);

void contexto_palabra(libreria * lib);

void mostrar_ord(libreria * lib);

void mostrar_relevancia(libreria * lib);

void buscar_tit(libreria * lib);

#endif /*Funciones_h*/