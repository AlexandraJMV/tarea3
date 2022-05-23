#include <stdio.h>
#include <stdlib.h>
#include "funciones.h"
#include <locale.h>

void clean(void)
{
    char c;
    while((c = getchar()) != '\n' && c != EOF){}
}

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
            clean();
            break;
        case mostrar_ordenado:
            system("cls");
            mostrar_ord(librer);
            clean();
            break;
        case buscar_titulo:
            system("cls");
            buscar_tit(librer);
            clean();
            break;
        case mayor_frecuencia:
            system("cls");
            top_frecuencia(librer);
            clean();
            break;
        case relevancia_Palabra:
            system("cls");
            mostrar_relevancia(librer);
            clean();
            break;
        case buscar_palabra:
            system("cls");
            buscarPalabra(librer);
            clean();
            break;
        case cntxt:
            system("cls");
            contexto_palabra(librer);
            clean();
            break;
        case end:
            printf("Saliendo del programa...\n");
            return 0;
            break;
        default:
            printf("Opcion no valida!\n");
            clean();
            break;
        }
    }
}