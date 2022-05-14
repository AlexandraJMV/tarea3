// fuck fuck fuck fuck fuck fuck fuck fuck fuck fuck
#include <stdio.h>
#include <stdlib.h>
#include "funciones.h"
#include <locale.h>

int main(void)
{
    libreria * librer = create_libreria();
    while(1)
    {
        system("cls");
        char str[MAXCHAR];
        int select;

        print_menu();
        fgets(str, MAXCHAR, stdin);
        select = toselect(str);

        switch (select)
        {
        case cargar:
            cargar_docs(librer);
            printlibrostest(librer);
            getchar();
            break;
        case mo_ord:
            /*
                m_orden();
            */
            break;
        case bu_tit:
            //busc
            break;
        case m_fr:
            //mayor frecuencia
            break;
        case bu_pl:
            // buscar por palabra
            break;
        case ctx_pl:
            // palabra en contexto
            break;
        case end:
            printf("Saliendo del programa...\n");
            getchar();
            return 0;
            break;
        default:
            printf("Opción no valida!\n");
            getchar();
            break;
        }
    }
}