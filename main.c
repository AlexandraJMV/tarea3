// fuck fuck fuck fuck fuck fuck fuck fuck fuck fuck
#include <stdio.h>
#include <stdlib.h>
#include "funciones.h"

int main(void)
{
    while(1)
    {
        system("cls");
        char str[MAXCHAR];
        int select;

        imprimir_menu();
        fgets(str, MAXCHAR, stdin);
        select = toselect(str);

        switch (select)
        {
        case cargar:
            /*
                Realizar lectura línea, identificar textos que existan e importar.
                cargar_docs();
            */
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
        default:
            printf("Opción no valida!\n");
            break;
        }
    }
    return 0;
}