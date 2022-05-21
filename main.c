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
            getchar();
            break;
        case mostrar_ordenado:
            system("cls");
            mostrar_ord(librer);
            getchar();
            break;
        case buscar_titulo:
            system("cls");
            buscar_tit(librer);
            getchar();
            break;
        case mayor_frecuencia:
            top_frecuencia(librer);
            getchar();
            break;
        case relevancia_Palabra:
            mostrar_relevancia(librer);
            getchar();
            break;
        case buscar_palabra:
            printf("Peo");
            getchar();
            break;
        case cntxt:
            // contexto_palabra();
            break;
        case end:
            printf("Saliendo del programa...\n");
            getchar();
            return 0;
            break;
        default:
            printf("Opcion no valida!\n");
            getchar();
            break;
        }
    }
}