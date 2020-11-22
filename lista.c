#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

/**
 Inicializa una lista vacía.
 Una referencia a la lista creada es referenciada en *L.
 Finaliza indicando LST_ERROR_MEMORIA si no es posible reservar memoria correspondientemente.
**/
void crear_lista(tLista * l)
{
    *l = (tLista)malloc(sizeof(struct celda));

    if(*l==NULL)
        exit(LST_ERROR_MEMORIA);

    (*l)->elemento = NULL;
    (*l)->siguiente = NULL;
}

/**
 Inserta el elemento E, en la posición P, en L.
 Con L = A,B,C,D y la posición P direccionando C, luego:
 L' = A,B,E,C,D.
 Finaliza indicando LST_ERROR_MEMORIA si no es posible reservar memoria correspondientemente.
**/
void l_insertar(tLista l, tPosicion p, tElemento e)
{
    struct celda* nueva_celda;
    nueva_celda = (struct celda*) malloc(sizeof(struct celda));

    if(nueva_celda == NULL)
        exit(LST_ERROR_MEMORIA);

    if (l->siguiente == NULL) //Si la lista es vacía, inserta en la primer posición.
    {
        l->siguiente = nueva_celda;
        nueva_celda->siguiente = NULL;
    }
    else
    {
        nueva_celda->siguiente = p->siguiente;
        p->siguiente = nueva_celda;
    }

    nueva_celda->elemento = e;
}

/**
 Elimina la celda P de L.
 El elemento almacenado en la posición P es eliminado mediante la función fEliminar.
 Finaliza indicando LST_POSICION_INVALIDA si P es fin(L).
**/
void l_eliminar(tLista l, tPosicion p, void (*fEliminar)(tElemento))
{
    tPosicion pos_aux;

    if(p->siguiente == NULL)
        exit(LST_POSICION_INVALIDA);

    fEliminar(p->siguiente->elemento);
    pos_aux = p->siguiente->siguiente; //Se guarda el puntero al siguiente de la posición para liberar el espacio en memoria.
    free(p->siguiente);

    if(pos_aux == NULL)
        p->siguiente = NULL;
    else
        p->siguiente = pos_aux;
}

/**
 Destruye la lista L, elimininando cada una de sus celdas.
 Los elementos almacenados en las celdas son eliminados mediante la función fEliminar.
**/
void l_destruir(tLista * l, void (*fEliminar)(tElemento))
{
    tPosicion pos_actual = (*l);
    tPosicion pos_aux;

    while(pos_actual!=NULL)
    {
        fEliminar(pos_actual->elemento);
        pos_aux = pos_actual;
        pos_actual = pos_actual->siguiente;
        free(pos_aux);
        pos_aux=NULL;
    }

    (*l) = NULL;
}

 /**
 Recupera y retorna el elemento en la posición P.
 Finaliza indicando LST_POSICION_INVALIDA si P es fin(L).
**/
tElemento l_recuperar(tLista l, tPosicion p)
{
    if(p->siguiente == NULL)
        exit(LST_POSICION_INVALIDA);

    return p->siguiente->elemento;
}

/**
 Recupera y retorna la primera posición de L.
 Si L es vacía, primera(L) = ultima(L) = fin(L).
**/
tPosicion l_primera(tLista l)
{
    return l;
}

/**
 Recupera y retorna la posición siguiente a P en L.
 Finaliza indicando LST_NO_EXISTE_SIGUIENTE si P es fin(L).
**/
tPosicion l_siguiente(tLista l, tPosicion p)
{
    if(p->siguiente == NULL)
        exit(LST_NO_EXISTE_SIGUIENTE);

    return p->siguiente;
}

/**
 Recupera y retorna la posición anterior a P en L.
 Finaliza indicando LST_NO_EXISTE_ANTERIOR si P es primera(L).
**/
tPosicion l_anterior(tLista l, tPosicion p)
{
    tPosicion puntero;

    if(p==l)
        exit(LST_NO_EXISTE_ANTERIOR);

    puntero = l;
    while(puntero->siguiente != NULL && puntero->siguiente != p)
    {
        puntero = puntero->siguiente;
    }

    return puntero;
}

/**
 Recupera y retorna la última posición de L.
 Si L es vacía, primera(L) = ultima(L) = fin(L).
**/
tPosicion l_ultima(tLista l)
{
    tPosicion puntero = l;

    if (l->siguiente != NULL)
        while(puntero->siguiente->siguiente != NULL)
            puntero = puntero->siguiente;

    return puntero;
}

/**
 Recupera y retorna la posición fin de L.
 Si L es vacía, primera(L) = ultima(L) = fin(L).
**/
tPosicion l_fin(tLista l)
{
    tPosicion puntero = l;

    while(l->siguiente!=NULL && puntero->siguiente!=NULL)
        puntero = puntero->siguiente;
    return puntero;
}

/**
 Retorna la longitud actual de la lista.
**/
int l_longitud(tLista l)
{
    tPosicion puntero;
    int longitud = 0;

    if(l->siguiente != NULL)
    {
        puntero = l;
        while(puntero->siguiente != NULL)
        {
            longitud = longitud+1;
            puntero = puntero->siguiente;
        }
    }

    return longitud;
}
