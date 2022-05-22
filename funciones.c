#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "funciones.h"
#include "treemap.h"
#include "list.h"
#include "hashmap.h"

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
    List * pal_frecuentes;
    long pal_tot;
    long char_tot;
}libro;

typedef struct libreria{
    TreeMap * libros_ord;
    HashMap * bloqueo;
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

/* reserva de memoria e inicializacion de una variable tipo palabra */
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

/* guarda palabras a partir de una cadena con una o mas palabras separadas por espacios */
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
    lb->pal_frecuentes = createList();
    lb->pal_titulo = createTreeMap(lower_than_string);
    lb->pal_libro = createTreeMap(lower_than_string);
    lb->pal_tot = 0;
    lb->char_tot  = 0;

    strcpy(lb->book_id, id);
    return lb;
}

/* Funcion borra una cadena dentro de otra, asumiendo que existe y la continua un espacio en la cadena a borrar */
void delete_strstr(char * ret, char * del)
{
    int len = strlen(del)+ 1;
    int fin_str = strlen(ret) - len;
    
    char * p = strstr(ret, del);
    for(int i=(p-ret) ; ret[i] != '\n' && ret[i] != '\0' && len >0 ; i++)
    {
        ret[i] = ret[i+len];
    }
    
    ret[fin_str] = '\0';
}

/* Elimina espacios del inicio de una cadena pre-concatenacion */
void elim_edgespaces(char * str)
{
    int is_start = 0, new_ini = 0;
    
    for(int i=0 ; str[i] != '\0' && str[i] != '\n' ; i++)
    {
        if(is_start)
        {
            break;
        }
        else
        {
            if (str[i] != ' ') is_start = 1;
            else new_ini ++;
            continue;
        }
    }
    
    for(int i = 0; str[i] != '\0' && str[i] != '\n' ; i++)
    {
        str[i] = str[new_ini];
        new_ini++;
    }
}

/* lee un libro considerando el formato especifico */
libro* read_book(char * arch, FILE * file)
{
    libro * lb;
    char linea[MAXLIN];
    char aux_titl[MAXLIN] = "\0";
    int is_title = 0, start=0, cont = 0;

    lb = create_book(arch);

    while(fgets(linea, MAXLIN, file) != NULL)
    {
        /* Lee titulo ubicado antes del comienzo de la lectura */
        if(start != 1 && is_title == 0)
        {
            if(strstr(linea, TITLE_FORMAT) != NULL)
                is_title= 1;
            
            char * e = strstr(linea, TITLE_FORMAT);
            if (e)
                delete_strstr(linea, TITLE_FORMAT);
        }

        if(start != 1  && is_title == 1)
        {
            if(linea[0] == '\n' || linea[0] == '\0')
            {

                char * e = strstr(aux_titl, "\n");
                if(e)
                    aux_titl[e-aux_titl] = '\0';

                strcpy(lb->titulo, aux_titl);
                guardar_palabras(lb, lb->pal_titulo, lb->titulo);

                is_title ++;
                continue;
            }
            else
            {
                cont++;
                if(cont>1)
                {
                    elim_edgespaces(linea);
                    char * p = strstr(aux_titl, "\n");
                    if(p)
                        aux_titl[p-aux_titl] = ' ';
                }
                strcat(aux_titl, linea);
            }
                
        }


        /* Encuentra el simbolo *** paraempezar con la lectura de palabras del texto. */
        if (start != 1) 
        {
            if (strstr(linea, STRT_FORMAT) != NULL)
                start = 1;
            continue;
        }

        /* Encuentra *** para terminar la lectura */
        if (start == 1) 
            if (strstr(linea, END_FORMAT) != NULL)
                break;

        if(start) guardar_palabras(lb, lb->pal_libro, linea);
    }
    return lb;
}

/* importa un archivo, creando un libro en caso de que exista */
libro* importar(char * arch)
{
    FILE * entrada;
    libro * book;
    char path[MAXCHAR] = PATH;

    char * p = strstr(arch , TXT_FORMAT);
    if (p==NULL)
    {
        char * pnt = strstr(arch, ".");
        if(pnt)
            arch[pnt-arch] ='\0';
        strcat(arch, TXT_FORMAT);
    }

    strcat(path, arch);

    entrada = fopen(path, "rt");
    if(entrada==NULL) 
        return NULL;

    book = read_book(arch, entrada);
    fclose(entrada);
    free(entrada);
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
            char min_title[MAXCHAR];
            strcpy(min_title, lib->titulo);
            minusc(min_title);

            if(searchTreeMap(Libreria->libros_ord, min_title) == NULL)
                Libreria->libros_tot ++;

            insertTreeMap(Libreria->libros_ord, _strdup(min_title), lib);
            cont++;
        }
    }

    printf("Se han importado %d documentos de forma exitosa!\n", cont);
    return;
}

/* Guarda memoria e inicializa elementos pertinentes de una variable tipo libreria */
libreria * create_libreria()
{
    libreria * lib = (libreria *)malloc(sizeof(libreria));
    if(lib==NULL)
    {
        perror("!");
        return NULL;
    }

    lib->bloqueo = createMap(MAX_BLOQ);
    lib->libros_ord = createTreeMap(lower_than_string);
    lib->libros_tot=0;

    return lib;
}

float liminf_frec(List * l)
{
    float new_lim;

    palabra * curr = (palabra*)firstList(l);
    while (curr != NULL)
    {
        new_lim = curr->frecuencia;
        curr = (palabra*) nextList(l);
    }

    return new_lim;
}

/* busca nuevo limite inferior de relevancia. 11 iteraciones MAX */
float new_lim(List * l)
{
    float new_lim;

    palabra * curr = (palabra*)firstList(l);
    while (curr != NULL)
    {
        new_lim = curr->relevancia;
        curr = (palabra*) nextList(l);
    }

    return new_lim;
}

// Inserta un lemento en una lista de forma ordenada segun frecuencia */
void insert_frec(List * l, palabra * p)
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
            return;
        }
        curr = (palabra*) nextList(l);
    }
    pushBack(l, p);
    return;
}

// Inserta un lemento en una lista de forma ordenada segun relevancia */
int insert_relv(List * l, palabra * p)
{
    palabra * curr = (palabra*)firstList(l);
    while (curr != NULL)
    {
        if(curr->relevancia == p->relevancia)
        {
            pushCurrent(l, p);
            return 1 ;
        } 
        else if(curr->relevancia < p->relevancia)
        {
            prevList(l);
            pushCurrent(l, p);
            return 1;
        }
        curr = (palabra*) nextList(l);
    }
    pushBack(l, p);
    return 1;
}

long pal_en_doc(palabra * p, libreria * libreria);

/* Retorna lista de mayores frecuencias y actualiza la variable dentro de struct de palabra*/
void find_top_frecuencia(libro * lib, libreria * libreria)
{
    List * top = createList();
    float limsup=0, liminf=0;
    int limit = 0;

    if(lib==NULL) return;

    int cont = 0;

    TreePair * palpair = firstTreeMap(lib->pal_libro);

    while(palpair != NULL)
    {
        cont++;
        palabra * pal = (palabra *) palpair->value;
        
        pal->frecuencia = (float)pal->ocurrencia/(float)lib->pal_tot;

        float is_bloq = (float) pal_en_doc(pal, libreria) /
                        (float) libreria->libros_tot;
        
        if(libreria->libros_tot < 10)
        {   
            if (pal->frecuencia >= limsup)
            {
                limsup = pal->frecuencia;
                pushFront(lib->pal_frecuentes, pal);
                limit++;
            }
            else if(pal->frecuencia >=  liminf)
            {
                insert_frec(lib->pal_frecuentes, pal);
                limit ++;
            }
        }
        else if(is_bloq <= 0.8)
        {
            if (pal->frecuencia >= limsup)
            {
                limsup = pal->frecuencia;
                pushFront(lib->pal_frecuentes, pal);
                limit++;
            }
            else if(pal->frecuencia >=  liminf)
            {
                insert_frec(lib->pal_frecuentes, pal);
                limit ++;
            }
        }

        if(limit>10)
        {
            liminf = liminf_frec(lib->pal_frecuentes);
            popBack(lib->pal_frecuentes);
            limit--;
        }
        palpair = nextTreeMap(lib->pal_libro);
    } 
}

/* Busqueda de libro en arbol de libros ordenados segun su ID */
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

/* Imput para busqueda y llamada a funciones para encontrar la frecuencia del libro solicitado */
void top_frecuencia(libreria * lib)
{
    char id[MAXCHAR], select[5];
    libro * book;
    List * top_f;
    palabra * currpal;
    int cont = 1;

    printf("Ingrese id del libro que quiere buscar (id.txt): ");
    fgets(id, MAXCHAR, stdin);

    if(id[0]=='\n' || id[0]=='\0' || id[0]==EOF)
    {
        printf("No ha ingresado un id valido !\n");
        return;
    }

    char * pos = strstr(id,"\n");
    id[pos-id] = '\0';

    book = search_id(lib->libros_ord, id);
    if(book)
    {
        cleanList(book->pal_frecuentes);
        find_top_frecuencia(book, lib);
        
        currpal = (palabra*)firstList(book->pal_frecuentes);
        if (currpal==NULL) 
        {
            printf("No se han guardado palabras !?\n");
            return;
        }
        else printf("\n--++ | Palabras mas frecuentes en el texto | ++--\n");

        if(lib->libros_tot > 10) printf("-- sin considerar aquellas que aparecen en el 80%% de los libros! --\n\n");
        else printf("\n");
        while (currpal!=NULL)
        {
            printf("-----------------------------+\n");
            printf("%-2d.- Palabra : %-14s|\nOcurrencia :%-17ld|\n", cont, currpal->palabra,currpal->ocurrencia);
            cont++;
            currpal = (palabra*)nextList(book->pal_frecuentes);
        }
        printf("-----------------------------\n");
    }
}

/* Retorna cantidad de apariciones de una palabra respecto a todos los libros */
long pal_en_doc(palabra * p, libreria * libreria)
{
    long cont = 0;

    TreePair * par = firstTreeMap(libreria->libros_ord);
    while(par != NULL)
    {
        libro * book = (libro *) par->value;

        if(searchTreeMap(book->pal_libro, p->palabra) != NULL)
            cont++;

        par = nextTreeMap(libreria->libros_ord);
    }

    return cont;
}


/* Calculo de relevancia e insercion en lista de palabras mas relevantes del libro */
void find_relev(libro * lib, libreria *  libreria)
{
    long en_doc, limit = 0;
    float limsup = 0, liminf = 0;

    cleanList(lib->pal_relevantes);

    TreePair * par = firstTreeMap(lib->pal_libro);

    while (par != NULL)
    {
        palabra * pal = (palabra *)par->value;

        /* Calcular la relevancia */
        en_doc = pal_en_doc(pal, libreria);

        if (en_doc != 0)
            pal->relevancia = (float) pal->ocurrencia / (float) lib->pal_tot
            * log(libreria->libros_tot/en_doc);           
        else 
            pal->relevancia = 0;

        /* Guardar en lista */
        if(pal->relevancia >= limsup)
        {
            pushFront(lib->pal_relevantes,pal);
            limit++;
            limsup = pal->relevancia;
        }
        else if (pal->relevancia >= liminf)
            if(insert_relv(lib->pal_relevantes, pal)) limit ++;

        if(limit>10)
        {
            liminf = new_lim(lib->pal_relevantes);
            popBack(lib->pal_relevantes);
            limit--;
        }
        par = nextTreeMap(lib->pal_libro);
    }
    return;
}

/* Imput titulo de libro para la  busqueda y calculo de relevancia, llamado a funcion e impresion de lista */
void mostrar_relevancia(libreria * libreria)
{
    char title[MAXCHAR];
    libro * lib;
    TreePair * par;
    palabra * p;
    int cont= 0;

    
    printf("Ingrese titulo del libro que quiere buscar : ");
    fgets(title, MAXCHAR, stdin);
    char * pos = strstr(title, "\n");
    if(pos) title[pos-title] = '\0';

    minusc(title);
    char * aux = strstr(title, "\n");
    if (aux)
        title[aux-title] = '\0';
    
    par = searchTreeMap(libreria->libros_ord, title);
    if (par == NULL){
        printf("Este libro no existe en la libreria!");
        return;
    } 
    
    lib = (libro*) par->value;

    find_relev(lib, libreria);

    p = (palabra*) firstList(lib->pal_relevantes);
    if (p) printf("\n--++ Palabras mas relevantes del texto ++--\n\n");
    while (p != NULL)
    {
        if(p->relevancia > 0) {
            cont++;
            printf("----------------------------+\n");
            printf("%-2d.- Palabra: %-14s|\nRelevancia: %-16f|\n", cont, p->palabra, p->relevancia);
        }
        p = (palabra*) nextList(lib->pal_relevantes);
    }
    if (cont == 0) printf("No hay ninguna palabra relevante (mayor que 0 !!)\n");
    else printf("-----------------------------\n");
}

void buscar_tit(libreria *l)
{
    int cont = 0;
    char palabra[MAXCHAR];
    libro * lib;
    TreePair * par_palabra;
    
    TreeMap * libros_ord = l->libros_ord;
    TreePair * treepar = firstTreeMap(libros_ord);
    

    printf("Ingrese palabras para buscar titulos, separados por espacios\n");
    fgets(palabra, MAXCHAR, stdin);

    if(palabra[0]=='\n' || palabra[0]=='\0' || palabra[0]==EOF) 
    {
        printf("No ha ingresado una palabra valida !\n");
        return;
    }

    while(treepar != NULL)
    {
        int aux = 1;
        int pos = 0;
        lib = (libro*) treepar->value;
        
        while(1){
            char pal[MAXCHAR];
            get_pal(palabra, pal, &pos);
            minusc(pal);
            if (pal[0] =='\0' || pal[0] == '\n') break;
            par_palabra = searchTreeMap(lib -> pal_titulo, pal);
            if (par_palabra == NULL)
            {
                aux = 0;
                break;
            }
        }

        if(aux != 0){
            printf("-----------------------------------------------------------------\n");
            printf("-->Titulo: %-52s\n Id: %-57s  |\n",lib -> titulo, lib -> book_id);
            printf("                                                                |\n");
            printf("-----------------------------------------------------------------\n");
            cont++;
        }
        treepar = nextTreeMap(libros_ord);

    }
    if(cont==0) printf("No se ha encontrado ningun libro con esta palabra!\n");
}

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
        printf("-->Titulo: %-53s|\n",lib -> titulo);
        printf("Palabras: %-10ld Caracteres: %-10ld Id: %-14s  |\n",lib -> pal_tot, lib -> char_tot, lib->book_id);
        printf("                                                                |\n");
        printf("-----------------------------------------------------------------\n");
        treepar = nextTreeMap(libros_ord);
    } 

    printf("Hay un total de %ld libros\n", l->libros_tot);
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
    return;
}
