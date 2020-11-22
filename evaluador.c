#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"
#include "mapeo.h"
#include "math.h"

#define ARCH_ERROR_APERTURA -1
#define ERROR_INVOCACION -2

//Se define la variable global para poder acceder al mapeo desde todo el fuente.
tMapeo mapeo;

/**
    Compara dos elementos parametrizados.
    Devuelve 0 si son iguales, cualquier otro número caso contrario.
**/
int fComparacion(void* s1, void* s2)
{
    char *string1 = s1;
    char *string2 = s2;
    int toReturn = strcmp(string1, string2);
    return toReturn;
}

/**
    Retorna la funcion hash de una clave recibida.
**/
int fHash(void* s)
{
    char* string = s;
    int hash = 0;
    for (int i = 0; i < strlen(string); i++)
        hash = 31 * hash + string[i];
    return abs(hash);
}

/**
    Elimina una clave parametrizada.
**/
void fEliminarC(tClave clave)
{
    free(clave);
    clave = NULL;
}

/**
    Elimina un valor parametrizado.
**/
void fEliminarV(tValor valor)
{
    free(valor);
    valor = NULL;
}

/**
    Muestra la cantidad de veces que la palabra parametrizada se encuentra en el archivo.
**/
int cant_apariciones(char* palabra)
{
    int to_return;
    tClave clave = palabra;
    tValor valor= m_recuperar(mapeo, clave);
    if(valor!=NULL)
        to_return = *(int*) valor;
    else
        to_return = 0;

    return to_return;
}

/**
    Finaliza el programa eliminando la memoria utilizada.
**/
void salir()
{
    m_destruir(&mapeo, &fEliminarC, &fEliminarV);
}

/**
    Lee el archivo y carga sus palabras al mapeo.
**/
void cargar_archivo(char *f)
{
    char linea[100]="\0";
    char delimitadores[] = ", ¿?¡!\n\r\0";
    char *contenido_separado, *palabra;
    char *puntero = (char*)malloc(100);
    tValor valor;
    FILE *file = fopen(f, "r");
    int* cant_apa;

    if(file==NULL)
    {
        printf("Ocurrió un error al intentar abrir el archivo.");
        exit(ARCH_ERROR_APERTURA);
    }

    crear_mapeo(&mapeo, 10, &fHash, &fComparacion);

    while(puntero!=NULL) //Recorre el archivo.
    {
        puntero = fgets(linea,100,file);
        contenido_separado = strtok(puntero, delimitadores); //Obtiene cada una las palabras de las líneas del archvio.
        while(contenido_separado!=NULL)
        {
            palabra = strdup(contenido_separado);
            if(palabra==NULL)
            {
                printf("\nOcurrió un error en la memoria");
                exit(-3);
            }
            valor = m_recuperar(mapeo, palabra);
            cant_apa= (int*) malloc(sizeof(int));
            if(valor == NULL)
                *cant_apa=1;
            else
            {
                (*cant_apa)= *((int*)valor);
                (*cant_apa)+=1;
            }
            //Esta asignación se debe a que m_insertar retorna un tValor
            cant_apa=(m_insertar(mapeo, palabra,cant_apa));

            free(cant_apa);
            contenido_separado = strtok(NULL, delimitadores);
        }
    }

    free(puntero);
    fclose(file);
}

/**
    Recorre el archivo parametrizado y computa la cantidad de apariciones de las palabras que lo componen.
    Ofrece un menú de operaciones que permita calcular la cantidad de apariciones de una palabra o salir del programa.
    Finaliza con ARCH_ERROR_APERTURA en caso de error al intentar la apertura del archivo parametrizado,
        ERROR_INVOCACION en caso de error ante la invocación del programa o 0 en caso de una finalización exitosa.
**/
void evaluador(char *ruta_archivo)
{
    char *buffer;
    int operacion, cantidad_apariciones, terminar=0;

    cargar_archivo(ruta_archivo);

    while(!terminar) //Permite realizar varias operaciones en una misma ejecución del programa.
    {
        printf("\nIngrese la operacion a realizar\n");
        printf("1: cantidad de apariciones, 2: salir >> ");
        scanf("%d", &operacion);
        fflush(stdin);

        if(operacion!=1 && operacion!=2)
        {
            printf("Ocurrio un error al intentar ejecutar el programa.");
            exit(ERROR_INVOCACION);
        }
        else
        {
            if(operacion==1)
            {
                printf("Ingrese la palabra que desea contabilizar >> ");
                buffer = (char*)malloc(250);
                scanf("%s", buffer);
                cantidad_apariciones = cant_apariciones(buffer);
                if(cantidad_apariciones>1)
                    printf("\nLa palabra %s aparece %i veces en el archivo.\n", buffer, cantidad_apariciones);
                else if(cantidad_apariciones==1)
                    printf("\nLa palabra %s aparece 1 vez en el archivo.\n", buffer);
                else
                    printf("\nLa palabra %s no aparece en el archivo.\n", buffer);
                free(buffer);
                fflush(stdin);
            }
            else
            {
                salir();
                terminar = 1;
            }
        }

        operacion = 0;
    }
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("\nLa cantidad de parametros esperada es incorrecta\n");
        exit(ERROR_INVOCACION);
    }
    evaluador(*(argv+1));

    return 0;
}
