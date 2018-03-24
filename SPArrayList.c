//
// Created by user on 6/21/2017.
//
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "SPArrayList.h"
/**
 *  Creates an empty array list with the specified maximum capacity.
 *  @param maxSize - the maximum capacity of the target array list.
 *  @return
 *  NULL, if an allocation error occurred or maxSize  <= 0.
 *  An instant of an array list otherwise.
 */
SPArrayList* spArrayListCreate(int maxSize){

    SPArrayList* list = (SPArrayList*)malloc(sizeof(SPArrayList)) ;
    if (list==NULL||maxSize<=0)
        return NULL ;

    list->maxSize = maxSize ;
    list->actualSize = 0 ;
    list->elements = malloc(maxSize* sizeof(void*)) ;
    if (list->elements==NULL||maxSize<=0)
        return NULL ;

    return list ;
}

/**
 *	Creates an exact copy of the src array list. Elements in the new copy will
 *	be in the same order as they appeared in the source list.
 *	@param src - the source array list.
 *	@return
 *	NULL if either an allocation error occurs or src == NULL.
 *	A new copy of the source array list, otherwise.
 */
SPArrayList* spArrayListCopy(SPArrayList* src){
    SPArrayList* cpy = (SPArrayList*)malloc(sizeof(SPArrayList)) ;
    if (src==NULL||cpy==NULL)
        return NULL ;
    cpy->maxSize = src->maxSize ;
    cpy->actualSize = src->actualSize ;
    cpy->elements = malloc(src->maxSize*sizeof(src->elements[0])) ;
    if (cpy->elements==NULL)
        return NULL ;

    for (int i=0;i<src->actualSize;i++)
        cpy->elements[i] = src->elements[i] ;
    return cpy ;
}

/**
 * Frees all memory resources associated with the source array list. If the
 * source array is NULL, then the function does nothing.
 * @param src - the source array list
 */
void spArrayListDestroy(SPArrayList* src){
    if (src==NULL)
        return ;

    int size = spArrayListSize(src) ;
    for (int i=0;i<size;i++){
        free(src->elements[i]) ;
    }

    free(src->elements) ;
    free(src) ;
    return ;
}

/**
 * Clears all elements from the source array list. After invoking this function,
 * the size of the source list will be reduced to zero and maximum capacity is
 * not affected.
 * @param src - the source array list
 * @return
 * SP_ARRAY_LIST_INVALID_ARGUMENT if src == NULL
 * SP_ARRAY_LIST_SUCCESS otherwise
 */
SP_ARRAY_LIST_MESSAGE spArrayListClear(SPArrayList* src){
    if (src==NULL)
        return SP_ARRAY_LIST_INVALID_ARGUMENT ;
    src->actualSize = 0 ;
    return SP_ARRAY_LIST_SUCCESS ;
}


/**
* Inserts element at a specified index. The elements residing at and after the
* specified index will be shifted to make place for the new element. If the
* array list reached its maximum capacity and error message is returned and
* the source list is not affected
* @param src   - the source array list
* @param elem  - the new element to be inserted
* @param index - the index where the new element will be placed. The index is
*                0-based.
* @return
* SP_ARRAY_LIST_INVALID_ARGUMENT - if src == NULL or the index is out of bound
* SP_ARRAY_LIST_FULL - if the source array list reached its maximum capacity
* SP_ARRAY_LIST_SUCCESS - otherwise
*/
SP_ARRAY_LIST_MESSAGE spArrayListAddAt(SPArrayList* src, void* elem, int index){

    if (src==NULL||index>src->actualSize||index<0)
        return SP_ARRAY_LIST_INVALID_ARGUMENT ;

    if (src->actualSize==src->maxSize)
        return SP_ARRAY_LIST_FULL ;

    int lastIndex = src->actualSize ;
    while (lastIndex>index){
        src->elements[lastIndex] = src->elements[lastIndex-1] ;
        lastIndex -= 1 ;
    }
    src->elements[lastIndex] = elem ;
    src->actualSize += 1 ;
    return SP_ARRAY_LIST_SUCCESS ;
}

/**
 * Inserts element at a the beginning of the source element. The elements
 * will be shifted to make place for the new element. If the
 * array list reached its maximum capacity and error message is returned and
 * the source list is not affected
 * @param src   - the source array list
 * @param elem  - the new element to be inserted
 * @return
 * SP_ARRAY_LIST_INVALID_ARGUMENT - if src == NULL or the index is out of bound
 * SP_ARRAY_LIST_FULL - if the source array list reached its maximum capacity
 * SP_ARRAY_LIST_SUCCESS - otherwise
 */
SP_ARRAY_LIST_MESSAGE spArrayListAddFirst(SPArrayList* src, void* elem){

    return  spArrayListAddAt(src, elem, 0) ;
}

/**
 * Inserts element at a the end of the source element. If the array list
 * reached its maximum capacity and error message is returned and the source
 * list is not affected.
 * @param src   - the source array list
 * @param elem  - the new element to be inserted
 * @return
 * SP_ARRAY_LIST_INVALID_ARGUMENT - if src == NULL or the index is out of bound
 * SP_ARRAY_LIST_FULL - if the source array list reached its maximum capacity
 * SP_ARRAY_LIST_SUCCESS - otherwise
 */
SP_ARRAY_LIST_MESSAGE spArrayListAddLast(SPArrayList* src, void* elem){

    return  spArrayListAddAt(src, elem, src->actualSize) ;
}

/**
 * Removes an element from a specified index. The elements residing after the
 * specified index will be shifted to make to keep the list continuous. If the
 * array list is empty then an error message is returned and the source list
 * is not affected
 * @param src   - The source array list
 * @param index - The index from where the element will be removed.
 *                The index is 0-based.
 * @return
 * SP_ARRAY_LIST_INVALID_ARGUMENT - if src == NULL or the index is out of bound
 * SP_ARRAY_LIST_EMPTY - if the source array list is empty
 * SP_ARRAY_LIST_SUCCESS - otherwise
 */
SP_ARRAY_LIST_MESSAGE spArrayListRemoveAt(SPArrayList* src, int index){

    if (src==NULL||index>=src->actualSize||index<0)
        return SP_ARRAY_LIST_INVALID_ARGUMENT ;

    if (src->actualSize==0)
        return SP_ARRAY_LIST_EMPTY ;

    void* current = spArrayListGetAt(src, index) ;

    int lastIndex = src->actualSize-1 ;
    int currentIndex = index ;
    while (lastIndex>currentIndex){
        src->elements[currentIndex] = src->elements[currentIndex+1] ;
        currentIndex += 1 ;
    }
    src->actualSize -= 1 ;
    free(current) ;
    return SP_ARRAY_LIST_SUCCESS ;
}

/**
 * Removes an element from a the beginning of the list.
 * The elements will be shifted to make to keep the list continuous. If the
 * array list is empty then an error message is returned and the source list
 * is not affected
 * @param src   - The source array list
 * @param elem  - The new element to be inserted
 * @return
 * SP_ARRAY_LIST_INVALID_ARGUMENT - if src == NULL
 * SP_ARRAY_LIST_EMPTY - if the source array list is empty
 * SP_ARRAY_LIST_SUCCESS - otherwise
 */
SP_ARRAY_LIST_MESSAGE spArrayListRemoveFirst(SPArrayList* src){

    return spArrayListRemoveAt(src, 0) ;
}

/**
 * Removes an element from a the end of the list.
 * The elements will be shifted to make to keep the list continuous. If the
 * array list is empty then an error message is returned and the source list
 * is not affected
 * @param src   - The source array list
 * @param elem  - The new element to be inserted
 * @return
 * SP_ARRAY_LIST_INVALID_ARGUMENT - if src == NULL
 * SP_ARRAY_LIST_EMPTY - if the source array list is empty
 * SP_ARRAY_LIST_SUCCESS - otherwise.
 */
SP_ARRAY_LIST_MESSAGE spArrayListRemoveLast(SPArrayList* src){

    return spArrayListRemoveAt(src, src->actualSize-1) ;
}

/**
 * Returns the element at the specified index. The function is called
 * with the assertion that all arguments are valid. If any of the arguments is
 * invalid then an undefined value is returned.
 * @param src - the source array list
 * @param index - the specified index, the index is 0-based.
 * @return
 * Undefined value if either src == NULL or index out of bound.
 * Otherwise, the element at the specified index is returned.
 */
void* spArrayListGetAt(SPArrayList* src, int index){

    if (src==NULL||index>=src->actualSize||index<0)
        return NULL ;
    void* element = src->elements[index] ;
    return element ;
}

/**
 * Returns the element at the beginning of the list. The function is called
 * with the assertion that all arguments are valid. If any of the arguments is
 * invalid then an undefined value is returned.
 * @param src - the source array list
 * @return
 * Undefined value if either src == NULL or the list is empty
 * Otherwise, the element at the beginning of the list is returned.
 */
void* spArrayListGetFirst(SPArrayList* src){

    return spArrayListGetAt(src, 0) ;
}

/**
 * Returns the element at the end of the list. The function is called
 * with the assertion that all arguments are valid. If any of the arguments is
 * invalid then an undefined value is returned.
 * @param src - the source array list
 * @return
 * Undefined value if either src == NULL or the list is empty
 * Otherwise, the element at the end of the list is returned.
 */
void* spArrayListGetLast(SPArrayList* src){
    void* element = spArrayListGetAt(src, src->actualSize-1) ;
    return element;
}

/**
 * Returns the maximum capacity of the list. The function is called
 * with the assertion that all arguments are valid. If any of the arguments is
 * invalid then an undefined value is returned.
 * @param src - the source array list
 * @return
 * Undefined value if either src == NULL
 * Otherwise, the maximum capacity of the list is returned.
 */
int spArrayListMaxCapacity(SPArrayList* src){

    if (src==NULL)
        return SP_ARRAY_LIST_INVALID_ARGUMENT ;

    return src->maxSize ;
}

/**
 * Returns the number of elements in the list. The function is called
 * with the assertion that all arguments are valid. If any of the arguments is
 * invalid then an undefined value is returned.
 * @param src - the source array list
 * @return
 * Undefined value if either src == NULL
 * Otherwise, the number of the elements in the list is returned.
 */
int spArrayListSize(SPArrayList* src){

    if (src==NULL)
        return SP_ARRAY_LIST_INVALID_ARGUMENT ;

    return src->actualSize ;
}

/**
 * Returns true if the list is full, that is the number of elements in the list
 * equals its maximum capacity.
 * @param src - the source array list
 * @return
 * false if either src == NULL or the number of elements in the list is less
 * than its maximum capacity.
 * Otherwise, true is returned.
 */
bool spArrayListIsFull(SPArrayList* src){

    if (src==NULL||src->actualSize<src->maxSize)
        return false ;
    return true ;
}

/**
 * Returns true if the list is empty, that is the number of elements in the list
 * equals to zero.
 * @param src - the source array list
 * @return
 * false if either src == NULL or the number of elements in the list is not zero.
 * Otherwise, true is returned.
 */
bool spArrayListIsEmpty(SPArrayList* src){

    if (src==NULL||src->actualSize>0)
        return false ;
    return true ;
}