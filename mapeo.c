#include <stdio.h>
#include <stdlib.h>
#include "lista.h"
#include "mapeo.h"

/**
    Retorna el mayor entre dos números parametrizados.
**/
int MAX(int num1, int num2)
{
   int to_return;

   if (num1 > num2)
      to_return = num1;
   else
      to_return = num2;

   return to_return;
}

//Se agregan prototipos para poder utilizarlos en los métodos.
void (*fEliminarClave)(void *);
void (*fEliminarValor)(void *);

/**
    Elimina del mapeo una entrada parametrizada.
**/
void fEliminarEntrada(tElemento e)
{
     tEntrada entrada = (tEntrada) e;
     if(entrada!=NULL)
    {
         fEliminarClave(entrada->clave);
         fEliminarValor(entrada->valor);
         free(entrada);
         entrada = NULL;
     }
}

/**
    Elimina un elemento parametrizado.
**/
void fEliminar(tElemento elemento)
{
    free(elemento);
    elemento=NULL;
}

/**
    Evalúa si el numero pasado por parametro es primo.
**/
int es_primo(int numero)
{
    int es_primo = 1;
	int divisor = 2;

	while(divisor<numero/2 && es_primo)
    {
		if(numero%divisor!=0)
			divisor++;
		else
			es_primo = 0;
    }

	return es_primo;
}

/**
    Obtiene el proximo primo al doble de un número pasado por parametro.
**/
int proximo_primo(int numero)
{
    int nro_primo = 0;
    numero++;

	while(!nro_primo)
    {
		if(es_primo(numero))
			nro_primo=1;
		else
			numero++;
	}

	return numero;
}

/**
    Redimensiona la tabla hash.
**/
void re_size(int longitud, tMapeo *m)
{
    int nuevo_size = proximo_primo(2*(*m)->longitud_tabla);
    int bucket;
    tEntrada entrada_vieja, entrada_nueva;
    tClave clave;
    tValor valor;
    tPosicion posicion_actual_lista, posicion_fin_lista;
    tLista lista_actual;
    tLista* lista = malloc(nuevo_size*sizeof(tLista));

    for(int i=0; i<nuevo_size; i++)
    {
        crear_lista(&(lista[i]));
    }

    for(int i=0; i<(*m)->longitud_tabla; i++) //Recorre la tabla existente.
    {
        lista_actual = (*m)->tabla_hash[i];
        posicion_actual_lista = l_primera(lista_actual);
        posicion_fin_lista = l_fin(lista_actual);
        while(posicion_actual_lista!=posicion_fin_lista)
        {
            entrada_vieja = l_recuperar(lista_actual, posicion_actual_lista);
            clave = entrada_vieja->clave;
            valor = entrada_vieja->valor;
            entrada_nueva = (tEntrada) malloc(sizeof(struct entrada));
            entrada_nueva->clave = clave;
            entrada_nueva->valor = valor;
            bucket = (*m)->hash_code(clave) % nuevo_size;
            l_insertar(lista[bucket], l_primera(lista[bucket]), entrada_nueva); //Almacena en la nueva tabla los datos anteriores.
            posicion_actual_lista = l_siguiente(lista_actual, posicion_actual_lista);
        }
        l_destruir(&lista_actual, &fEliminar); //Se llama a fEliminar para que no elimine las claves y valores.
    }

	(*m)->tabla_hash = 	lista;
    (*m)->longitud_tabla = nuevo_size;
}

/**
 Inicializa un mapeo vacío, con capacidad inicial igual al MAX(10, CI).
 Una referencia al mapeo creado es referenciada en *M.
 A todo efecto, el valor hash para las claves será computado mediante la función fHash.
 A todo efecto, la comparación de claves se realizará mediante la función fComparacion.
 Finaliza indicando MAP_ERROR_MEMORIA si no es posible reservar memoria correspondientemente.
**/
void crear_mapeo(tMapeo * m, int ci, int (*fHash)(void *), int (*fComparacion)(void *, void *))
{
    int capacidad = MAX(ci, 10);
    tLista* lista;

    *m = (tMapeo) malloc(sizeof(struct mapeo));
    if(*m==NULL)
        exit(MAP_ERROR_MEMORIA);

    lista = malloc(capacidad * sizeof(tLista));
    if(lista == NULL)
        exit(MAP_ERROR_MEMORIA);

    (*m)->longitud_tabla = capacidad;
    (*m)->cantidad_elementos = 0;
    (*m)->hash_code = fHash;
    (*m)->comparador = fComparacion;
    (*m)->tabla_hash = lista;

    for(int i=0; i<capacidad; i++) //Crea tantas listas como indique capacidad.
    {
        crear_lista(&((*m)->tabla_hash[i]));
    }
}

/**
 Inserta una entrada con clave C y valor V, en M.
 Si una entrada con clave C y valor Vi ya existe en M, modifica Vi por V.
 Retorna NULL si la clave C no existía en M, o Vi en caso contrario.
 Finaliza indicando MAP_ERROR_MEMORIA si no es posible reservar memoria correspondientemente.
**/
tValor m_insertar(tMapeo m, tClave c, tValor v)
{
    tLista mi_lista;
    tEntrada entrada;
    tValor to_return = NULL;
    tPosicion pos_actual, pos_fin;
    int encontre = 0;
    int bucket = m->hash_code(c) % m->longitud_tabla;

    mi_lista = *(m->tabla_hash+bucket);
    pos_actual = l_primera(mi_lista);
    pos_fin = l_fin(mi_lista);

    while(!encontre && pos_actual!=pos_fin) //Recorre la tabla en búsqueda de una entrada con la clave parametrizada.
    {
        entrada = (tEntrada) l_recuperar(mi_lista, pos_actual);
        if (m->comparador(c, entrada->clave) == 0) //Si se encuentra una clave igual, se actualiza su valor al parametrizado.
        {
            encontre = 1;
            to_return = entrada->valor;
            entrada->valor = v;
        }
        else
            pos_actual = l_siguiente(mi_lista, pos_actual);
    }
    if(!encontre) //Al no encontrarlo corresponde crear una nueva entrada con la clave y el valor parametrizados.
    {
        entrada = (tEntrada) malloc(sizeof(struct entrada));

        if(entrada==NULL)
            exit(MAP_ERROR_MEMORIA);

        entrada->clave = c;
        entrada->valor = v;
        l_insertar(mi_lista, l_primera(mi_lista), entrada);
        m->cantidad_elementos = m->cantidad_elementos+1;

        if((float)(m->cantidad_elementos)/(m->longitud_tabla) >= 0.75)
            re_size(m->cantidad_elementos, &m);
    }

    return to_return;
}

/**
 Elimina la entrada con clave C en M, si esta existe.
 La clave y el valor de la entrada son eliminados mediante las funciones fEliminarC y fEliminarV.
**/
void m_eliminar(tMapeo m, tClave c, void (*fEliminarC)(void *), void (*fEliminarV)(void *))
{
    int bucket = m->hash_code(c) % m->longitud_tabla, elimine = 0;
    tLista mi_lista = m->tabla_hash[bucket];
    tEntrada entrada;
    tPosicion pos_actual, pos_fin;
    fEliminarClave = fEliminarC;
    fEliminarValor = fEliminarV;

    pos_actual = l_primera(mi_lista);
    pos_fin = l_fin(mi_lista);
    while(!elimine && pos_actual!=pos_fin) //Recorre el bucket hasta encontrar la entrada con la clave parametrizada o el final.
    {
        entrada = (tEntrada) l_recuperar(mi_lista, pos_actual);
        if (m->comparador(c, entrada->clave) == 0)
        {
            elimine = 1;
            l_eliminar(mi_lista, pos_actual, &fEliminarEntrada);
            m->cantidad_elementos = m->cantidad_elementos-1;
        }
        else
            pos_actual = l_siguiente(mi_lista, pos_actual);
    }
}

/**
 Destruye el mapeo M, elimininando cada una de sus entradas.
 Las claves y valores almacenados en las entradas son eliminados mediante las funciones fEliminarC y fEliminarV.
**/
void m_destruir(tMapeo * m, void (*fEliminarC)(void *), void (*fEliminarV)(void *))
{
    fEliminarClave = fEliminarC;
    fEliminarValor = fEliminarV;

    for(int i=0; i<(*m)->longitud_tabla; i++) //Elimina una a una las listas asociadas a cada uno de los buckets de la tabla.
    {
        l_destruir((*m)->tabla_hash+i, &fEliminarEntrada);
    }
    free((*m)->tabla_hash);
    (*m)->tabla_hash=NULL;

    (*m)->cantidad_elementos=0;
    (*m)->comparador=NULL;
    (*m)->hash_code=NULL;
    (*m)->longitud_tabla=0;

    free(*m);
    (*m)=NULL;
}

/**
 Recupera el valor correspondiente a la entrada con clave C en M, si esta existe.
 Retorna el valor correspondiente, o NULL en caso contrario.
**/
tValor m_recuperar(tMapeo m, tClave c)
{
    int bucket = m->hash_code(c) % m->longitud_tabla;
    int encontre = 0;
    tLista mi_lista = m->tabla_hash[bucket];
    tValor to_return = NULL;
    tPosicion pos_actual, pos_fin;
    tEntrada entrada;

    pos_actual = l_primera(mi_lista);
    pos_fin = l_fin(mi_lista);
    while(!encontre && pos_actual!=pos_fin) //Recorre el bucket hasta encontrar la entrada con la clave parametrizada o el final.
    {
        entrada = (tEntrada) l_recuperar(mi_lista, pos_actual);
        if (m->comparador(c, entrada->clave) == 0)
        {
            encontre = 1;
            to_return = entrada->valor;
        }
        else
            pos_actual = l_siguiente(mi_lista, pos_actual);
    }

    return to_return;
}
