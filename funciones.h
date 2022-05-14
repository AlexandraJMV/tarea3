#ifndef Funciones_h
#define Funciones_h
#define MAXCHAR 400
#define MAXLIN 600
#define STRT_FORMAT "*** START OF THE PROJECT GUTENBERG EBOOK"
#define END_FORMAT "*** END OF THE PROJECT GUTENBERG EBOOK"
#define FIRSTLIN_FORMAT "The Project Gutenberg eBook of "
#define LASTLIN_FORMAT ", by"

enum op1{cargar=1, mo_ord, bu_tit, m_fr, bu_pl, ctx_pl, end };

typedef struct libreria libreria;

void print_menu();
void cargar_docs(libreria * lib);
int toselect(char * str);
libreria * create_libreria();
void printlibrostest(libreria * Libreria);
void printpaltest(libreria* libreria);

#endif /*Funciones_h*/