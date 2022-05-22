#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
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
    float relevante;
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

            printf("%s -> %s\n", lib->titulo, min_title);

            insertTreeMap(Libreria->libros_ord, _strdup(min_title), lib);
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

int insert_frec(List * l, palabra * p)
{
    palabra * curr = (palabra*)firstList(l);
    while (curr != NULL)
    {
        if(curr->frecuencia == p->frecuencia)
        {
            pushCurrent(l, p);
            return 0 ;
        } 
        else if(curr->frecuencia < p->frecuencia)
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



List * find_top_frecuencia(libro * lib)
{
    List * top = createList();
    float limsup=0, liminf=0;
    int limit = 0;

    if(lib==NULL) return NULL;

    int cont = 0;

    TreePair * palpair = firstTreeMap(lib->pal_libro);
    if (palpair==NULL) printf("no hay palabras guardadas D: !!!!!\n");
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
            if (insert_frec(top, pal)) limit++;

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

/**********************************************************************
Encuentra las palabras mas relevantes, primero que nada hay que calcular 
la relevancia de cada una con la formula entreda, la cual es con las 
ocurrencias de las palabras en el documento dividido por la cantidad de 
palabras total del documento, multiplicado por el logaritmo  del numero
de documentos partido en los documentos que contienen dicha palabra, luego 
de esto se agrega a la lista  que contiene el libro de forma ordenada.
**********************************************************************/

void find_relev(libro * lib, libreria *  libreria)
{
    long en_doc, limit = 0;
    float limsup = 0, liminf = 0;

    List * relevantes = lib->pal_relevantes;
    if (relevantes == NULL) return;

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

        if(limit>=10)
        {
            liminf = new_lim(lib->pal_relevantes);
            popBack(lib->pal_relevantes);
            limit--;
        }
        par = nextTreeMap(lib->pal_libro);
    }
    return;
}
/**********************************************************************
Muestra la relevancia de la palabra que esta buscando, imprimiendo dicha 
palabra buscada y su relevancia dentro de los documentos. Si no se encuentra
la palabra indicara que no hay relevacia mayor que 0.
**********************************************************************/
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
    while (p != NULL)
    {
        if(p->relevancia > 0) {
            printf("Palabra: %s\nRelevancia: %f\n\n", p->palabra, p->relevancia);
            cont++;
        }
        p = (palabra*) nextList(lib->pal_relevantes);
    }
    if (cont == 0) printf("No hay ninguna palabra relevante (mayor que 0 !!)\n");
}

/**********************************************************************
Busca titulos de libros por palabras separas por espacio, imprimiendo
respectivamente el titulo del libro y su id, el titulo buscado debe tener
si o si todas las palabras buscadas, en cualquier orden pero todas, incluso 
puede tener mas palabras que las que se buscan. imprimiendolos
**********************************************************************/

void buscar_tit(libreria *l)
{
    char palabra[MAXCHAR];
    libro * lib;
    TreePair * par_palabra;
    
    TreeMap * libros_ord = l->libros_ord;
    TreePair * treepar = firstTreeMap(libros_ord);
    

    printf("Ingrese palabras para buscar titulos, separados por espacios\n");
    fgets(palabra, MAXCHAR, stdin);

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
            
        }
        treepar = nextTreeMap(libros_ord);

    }
    printf("-----------------------------------------------------------------\n");
}

/**********************************************************************
Muestra titulos e informacion sobre los libros en orden alfabetico, esta 
informacion consta de las palabras que contiene el libro, los caracteres de 
todo el texto y la id correspondiente. imprimiendolos
**********************************************************************/

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

void mostrarListaOrdenadaRelevancia(List * libros){
    printf("Libros ordenados Segun la relevancia de la palabra : \n");
    libro * lib=firstList(libros);
    while(lib != NULL){
        printf("-----------------------------------------------------------------\n");
        printf("-->Titulo: %-52s |\n",lib -> titulo);
        printf("Palabras: %-10ld Caracteres: %-10ld Id: %-14s  |\n",lib -> pal_tot, lib -> char_tot, lib->book_id);
        printf("                                                                |\n");
        printf("-----------------------------------------------------------------\n");
        lib = nextList(libros);
    }
}


void buscarPalabra(libreria * lib){

    char palabra[MAXCHAR];
    List * top_Relevancia=createList();
    TreeMap * MapLib =lib->libros_ord;

    TreePair * trepair = firstTreeMap(MapLib);
    if(trepair == NULL){
        printf("\nNo hay ningun libro guardado!,intenta agregar uno\n");
        return;
    }
    printf("Ingrese la palabra a buscar:");
    fgets(palabra, MAXCHAR, stdin);
    minusc(palabra);
    printf("-->%s",palabra);

    while(trepair != NULL){
        libro * libros=(libro *)trepair->value;
        TreePair * pair;
        int aux=1;
        int pos=0;

        while(1){
            char pal[MAXCHAR];
            get_pal(palabra, pal, &pos);
            minusc(pal);
            if (pal[0] =='\0' || pal[0] == '\n') break;
            pair = searchTreeMap(libros->pal_libro, pal);
            if (pair == NULL)
            {
                aux = 0;
                break;
            }
        }
        if(aux != 0){
            pushBack(top_Relevancia,libros);
        }

        trepair=nextTreeMap(MapLib);
    }
    if (firstList(top_Relevancia)==NULL){
        printf("La palabra que buscas no se encuentra en ninguno de los libros");
        return;
    }
    mostrarListaOrdenadaRelevancia(top_Relevancia);
    
}

void contexto_palabra(libreria * lib){
    
    
    return;
}