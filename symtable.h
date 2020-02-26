/*-------------------------------------------------------------------*/
/* symtable.h.                                                       */
/* Author: Isaac Wolfe                                               */
/* netid: iwolfe                                                     */
/*-------------------------------------------------------------------*/

#include <stddef.h>

/*-------------------------------------------------------------------*/

#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SymTable_T: An object that is a chain of structures (called       *
 *             nodes) with each node pointing to the next node in    *
 *             the chain. The nodes also contain char-type key       *
 *             pointers and untyped (void) value pointers.           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct SymTable *SymTable_T;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SymTable_new: Returns a new SymTable object that contains no      *
 *               bindings, or NULL if insufficient memory is         *
 *               available.                                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

SymTable_T SymTable_new(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SymTable_free: Frees all memory occupied by SymTable_T argument   *
 *                oSymTable.                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SymTable_free(SymTable_T oSymTable);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SymTable_getLength: Returns the number of bindings in SymTable_T  *
 *                     argument oSymTable.                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

size_t SymTable_getLength(SymTable_T oSymTable);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SymTable_put: If argument SymTable_T oSymTable does not contain   *
 *               a binding with argument key pcKey, then this        *
 *               function adds a new binding to oSymTable consisting *
 *               of key pcKey and value pvValue and return 1.        *
 *               Otherwise the function leaves oSymTable unchanged   *
 *               and return 0. If insufficient memory is available,  *
 *               then the function leaves oSymTable unchanged and    *
 *               returns 0.                                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SymTable_replace: If afgumnet SymbleTable_t oSymTable contains a  *
 *                   binding with key pcKey, then this function      *
 *                   replaces the binding's value with pvValue and   *
 *                   returns the old value. Otherwise it leaves      *
 *                   oSymTable unchanged and returns NULL.           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
   const void *pvValue);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SymTable_contains: Returns 1 if SymbTable_T oSymTable contains a  *
 *                    binding whose key is pcKey, and 0 otherwise.   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SymTable_get: return the value of the binding within SymTable_T   *
 *               argument oSymTable whose key is pcKey, or NULL if   *
 *               no such binding exists.                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SymTable_remove: If SymTable_T argument oSymTable contains a      *
 *                  binding with key char-pointer pcKey, then        *
 *                  then this function rmeoves that binding from     *
 *                  oSymTable and returns the binding's value.       *
 *                  Otherwise the function does not change oSymTable *
 *                  and returns NULL.                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SymTable_map: Applies function *pfApply to each binding in        *
 *               Symtable_T object oSymTable, passing pvExtra as     *
 *               an extra parameter.                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra);

#endif