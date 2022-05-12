#ifndef Funciones_h
#define Funciones_h
#define MAXCHAR 400

enum op1{cargar=1, mo_ord, bu_tit, m_fr, bu_pl, ctx_pl, end};

void print_menu();
void cargar_docs();
int toselect(char * str);

#endif /*Funciones_h*/