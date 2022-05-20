#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "funciones.h"
#include "treemap.h"
#include "list.h"

typedef struct palabra{
    char palabra[MAXCHAR];
    long ocurrencia;
    float frecuencia;
    float relevancia;
}palabra;

typedef struct libro{
    char titulo[MAXCHAR];
    char book_id[MAXCHAR];
    TreeMap * pal_titulo;
    TreeMap * pal_libro;
    List * pal_relevantes;
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
            "2. Mostrar documentos ordenados\n"
            "3. Buscar libro por titulo\n"
            "4. Mostrar 10 palabras de mayor frecuencia\n"
            "5. Mostrar palabras relevantes de un libro\n"
            "6. Buscar por Palabra\n"
            "7. Mostrar contexto de una palabra\n"
            "8. Salir del programa\n"

            "\nIngrese lo que desea hacer: ");
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
    int ini = strlen(FIRSTLIN_FORMAT)+3; // +3 para compensar la basura :(
    
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
    return _strdup(lin+ini);
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

        if(start==1 && isalpha(str[ind]) == 0)
        {
            if(isalpha(str[ind+1])==0) break;
        }

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
    pal->relevancia = 0;

    return pal;
}

void guardar_palabras(libro * lib, TreeMap * obj, char *str)
{
    int cont = 0;
    int pos=0;
    while (1)
    {
        cont++;
        char pal[MAXCHAR];
        char clean[MAXCHAR];

        get_pal(str, pal, &pos);
        if (pal[0] =='\0') break;
        
        strcpy(clean, clean_pal(pal));

        if( clean[0]!='\0' )
        {
            minusc(clean);

            TreePair * par = searchTreeMap(obj, clean);
            if (par == NULL)
            {
                palabra * pp = create_palabra(clean);
                pp->ocurrencia = 1;
                insertTreeMap(obj , _strdup(clean), pp);
            }
            else
            {
                palabra * p = (palabra *) par->value;
                p->ocurrencia++;
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

    lb->pal_relevantes = createList();
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
                strcpy(lb->titulo, extract_title(aux_titl));
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
    char path[MAXCHAR] = PATH;

    strcat(path, arch);

    entrada = fopen(path, "rt");
    if(entrada==NULL) 
        return NULL;

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
    if(lib==NULL) return;
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
    if(treepar==NULL) return;
    libro* lib = (libro *)treepar->value;
    TreePair * par = firstTreeMap(lib->pal_libro);
    if(par==NULL) printf("La COSA. esta vacia\n");
    while(par!=NULL && cont<20)
    {
        palabra* pal = (palabra*) par->value;
        printf("palabra: %s\n", pal->palabra);
        printf("ocurrencia = %ld\n", pal->ocurrencia);
        printf("frecuencia = %f\n\n", pal->frecuencia);
        par = nextTreeMap(lib->pal_libro);
        cont++;
    }
}

float elim_liminf(List * l, float liminf)
{
    float auxlim = liminf;
    float new_lim;

    palabra * curr = (palabra*)firstList(l);
    while (curr != NULL)
    {
        if(curr->frecuencia == auxlim)
        {
            popCurrent(l);
        }
        else new_lim = curr->frecuencia;
        curr = (palabra*) nextList(l);
    }

    return new_lim;
}

void insert_frec(List * l, palabra * p, int * max)
{
    palabra * curr = (palabra*)firstList(l);
    while (curr != NULL)
    {
        if(curr->frecuencia == p->frecuencia)
        {
            pushCurrent(l, p);
            return;
        } 
        else if(curr->frecuencia < p->frecuencia)
        {
            prevList(l);
            pushCurrent(l, p);
            (*max)++;
            return;
        }
        curr = (palabra*) nextList(l);
    }
    (*max)++;
    pushBack(l, p);
}


List * find_top_frecuencia(libro * lib)
{
    List * top = createList();
    float limsup=0, liminf=0;
    int limit = 0;

    if(lib==NULL) return NULL;

    int cont = 0;

    TreePair * palpair = firstTreeMap(lib->pal_libro);
    if (palpair==NULL) printf("no hay palabras wtf\n");
    while(palpair != NULL)
    {
        cont++;
        palabra * pal = (palabra *) palpair->value;
        
        pal->frecuencia = (float)pal->ocurrencia/(float)lib->pal_tot;
        if(pal->frecuencia == liminf)
            pushBack(top, pal); 
        else if (pal->frecuencia == limsup)
            pushFront(top, pal);
            
        else if(pal->frecuencia > limsup)
        {
            limit++;
            pushFront(top, pal);
            limsup = pal->frecuencia;
        }
        else if(pal->frecuencia > liminf)
            insert_frec(top, pal, &limit);

        if(limit>=10)
        {
            liminf = elim_liminf(top, liminf);
            limit--;
        }
        palpair = nextTreeMap(lib->pal_libro);
    } 
    return top;
}

libro * search_id(TreeMap * libros, char * id)
{
    libro * currlib;
    TreePair * par = firstTreeMap(libros);
    while(par!=NULL)
    {
        currlib = (libro*)par->value;
        if(strcmp(currlib->book_id, id)==0)
            return currlib;
        par = nextTreeMap(libros);
    }
    return NULL;
}

void top_frecuencia(libreria * lib)
{
    char id[MAXCHAR], select[5];
    libro * book;
    List * top_f;
    palabra * currpal;

    printf("Ingrese id del libro que quiere buscar (id.txt): ");
    fgets(id, MAXCHAR, stdin);

    char * pos = strstr(id,"\n");
    id[pos-id] = '\0';

    book = search_id(lib->libros_ord, id);
    if(book)
    {
        top_f = find_top_frecuencia(book);
        
        currpal = (palabra*)firstList(top_f);
        if (currpal==NULL) printf("Lista vacia\n");
        while (currpal!=NULL)
        {
            printf("\n%s %f", currpal->palabra,currpal->frecuencia);
            currpal = (palabra*)nextList(top_f);
        }
    }
}

void find_relev(libro * lib, libreria *  libreria)
{
    return;
}

void mostrar_relevancia(libreria * libreria)
{
    char title[MAXCHAR];
    libro * lib;
    TreePair * par;
    palabra * p;

    printf("Ingrese id del libro que quiere buscar (id.txt): ");
    fgets(title, MAXCHAR, stdin);

    par = searchTreeMap(libreria->libros_ord, title);
    if (par == NULL){
        printf("Este libro no existe en la libreria!");
        return;
    } 
    else lib = (libro*) par->value;

    if (firstList(lib->pal_relevantes) == NULL){
        find_relev(lib, libreria);
    }

    p = (palabra*) firstList(lib->pal_relevantes);
    while (p != NULL)
    {
        printf("Palabra: %s\nRelevancia: %f\n\n", p->relevancia);
        p = (palabra*) nextList(lib->pal_relevantes);
    }
}

/*void buscar_tit(libreria *l){
    libro * lib;
    TreePair * par;
    palabra * p;

    par = searchTreeMap(l->libros_ord, lib->pal_titulo);
    printf("Ingrese palabras para buscar titulos, separados por espacios\n");
    fgets(p, MAXCHAR, stdin);


}*/

void mostrar_ord(libreria * l){
    TreeMap * libros_ord = l->libros_ord;

    TreePair * treepar = firstTreeMap(libros_ord);
    if(treepar == NULL){
        printf("\nNo hay ningun libro guardado!\n");
        return;
    }

    printf("Libros ordenados de manera alfabetica : \n");
    while(treepar != NULL){
        libro * lib = (libro*) treepar->value;
        printf("-----------------------------------------------------------------\n");
        printf("-->Titulo: %-52s |\n",lib -> titulo);
        printf("Palabras: %-10ld Caracteres: %-10ld Id: %-14s  |\n",lib -> pal_tot, lib -> char_tot, lib->book_id);
        printf("                                                                |\n");
        printf("-----------------------------------------------------------------\n");
        treepar = nextTreeMap(libros_ord);
    } 
    return;
}


/*
    6. Buscar por palabra. El usuario ingresa una palabra y la aplicación muestra los libros (id y título) 
    que tienen la palabra en su contenido. Los libros deben ser ordenados por la relevancia de la palabra buscada.  
    Por ejemplo, si busco “Jesús”, la biblia debería aparecer en primer lugar.
    Si busco “mancha”, el Quijote debería salir en primer lugar.
    
    7. Mostrar palabra en su contexto dentro del libro. 
    El usuario ingresa el título de un libro y el de una palabra a buscar. 
    La aplicación muestra las distintas apariciones de la palabra en el contexto del documento, es decir,
    para cada aparición, se muestran algunas palabras hacia atrás y hacia adelante de la palabra buscada
    (por ejemplo, la línea completa en la que aparece la palabra, o 5 palabras hacia atrás y 5 hacia adelante).

    solucion: 1)En buscar palabra , crear funcion que entregue los libros en el que estan presente la palabra y funcion
                relevancia_palabra para ordenar el mostrar lista de libro segun relevancia.(mayor el num = mayor relevancia)
              2)Por cada aparcion de una palabra mostrar la linea que lo contiene.


*/



void buscarPalabra(libreria * lib){

    char palabra[MAXCHAR];
    List * top_Relevancia=createList();

    printf("Ingrese la palabra a buscar:\n");
    scanf("%s",palabra);
    getchar;

    minusc(palabra);

    TreePair * trepair=firstTreeMap(lib->libros_ord);

    if (trepair == NULL){
        printf("No hay libros agregados.");
        return;
    }

    while(trepair != NULL){
        libro * libros=(libro *)trepair->value;
        if (searchTreeMap(libros->pal_libro,palabra)->value != NULL){
            pushBack(top_Relevancia,libros);
        }

        trepair=nextTreeMap(lib->libros_ord);
    }
    if(firstList(top_Relevancia) == NULL){
        printf("la palabra que buscas no esta presente en ningun libro");
    }
    else{
        //mostrarListaOrdenadaRelevancia(listaLib);
    }   
}

void contexto_palabra(libreria * lib){

}
