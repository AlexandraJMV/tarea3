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
            printtitulo(librer);
            getchar();
            break;
        case mostrar_ordenado:
            /*
                m_orden();
            */
            break;
        case buscar_titulo:
            //busc
            break;
        case mayor_frecuencia:
            top_frecuencia(librer);
            getchar();
            break;
        case relevancia_Palabra:
            //Relevancia_Palabra();
            getchar;
            break;
        case buscar_palabra:
            // buscar_palabra(librer);
            getchar;
            break;
        case contexto_palabra:
            // contexto_palabra();
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