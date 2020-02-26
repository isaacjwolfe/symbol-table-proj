/*-------------------------------------------------------------------*/
/* symtablelist.c                                                    */
/* Author: Isaac Wolfe                                               */
/* netid: iwolfe                                                     */
/*-------------------------------------------------------------------*/

#include <string.h>
#include "symtable.h"
#include <assert.h>
#include <stdlib.h>

/*-------------------------------------------------------------------*/

/* SymTable is a structure to maintain a counter for the number of 
   bindings in a Symbol Table and a pointer to the "head" Node. */

struct SymTable 
{
	/* Node for head of the st */
	struct Node *psHead;

	/* variable to store size of st */
	size_t uCount;
};

/*-------------------------------------------------------------------*/

/* Node is a structure that stores and associates a char Key with 
   a void value and also maintains a pointer to the next node. */

struct Node
{
	/* Char pointer to hold the Key. */
	const char *pcKey;

	/* A void pointer to hold the Value. */
	const void *pvValue;

	/* Another Node to hold a pointer to the next Node. */
	struct Node *psNext;
};

/*-------------------------------------------------------------------*/

SymTable_T SymTable_new(void)
{
	SymTable_T oSymTable;

	/* Allocate memory and return if it is unsufficient. */
	size_t uSize = sizeof(struct SymTable);
	oSymTable = (SymTable_T)malloc(uSize);
	if (oSymTable == NULL) return NULL;

	/* Initiate uCount and psHead. */
	oSymTable->uCount = 0;
	oSymTable->psHead = NULL;

	return oSymTable;
}

/*-------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable)
{
	struct Node *psCurr;
	struct Node *psNext;

	assert(oSymTable != NULL); 

	psCurr = oSymTable->psHead;

	/* Traverse list and free the memory associated with 
	   each Key and Node. */
	while (psCurr != NULL)
	{
		psNext = psCurr->psNext;
		free((char *)psCurr->pcKey);
		free(psCurr);
		psCurr = psNext;
	}
	free(oSymTable);
}

/*-------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable)
{
	assert(oSymTable != NULL);

	/* Return length count from SymTable structure. */
	return oSymTable->uCount;
}

/*-------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
	const void *pvValue)
{
	struct Node *psNodePut;

	assert(oSymTable != NULL);
	assert(pcKey != NULL);

	/* Return 0 if oSymTable already contains pcKey... */
	if (SymTable_contains(oSymTable, pcKey) == 1) return 0;

	/* ...if not, allocate memory for the node and key in copies
	   and check to ensure there is suffcient memory. 
	   Return 0 if not */
	psNodePut = (struct Node*)malloc(sizeof(struct Node));
	if (psNodePut == NULL) return 0;
	psNodePut->pcKey = (const char*)malloc(strlen(pcKey) + 1);
	if (psNodePut->pcKey == NULL) return 0;

	/* Copy pcKey to the new  Node we are placing, then copy over 
	   the value and insert the node as the head with a next pointer 
	   to the old head. */
	strcpy((char*)psNodePut->pcKey, pcKey);
	psNodePut->pvValue = pvValue;
	psNodePut->psNext = oSymTable->psHead;
	oSymTable->psHead = psNodePut;

	/* Increment binding count of oSymTable. */
	oSymTable->uCount++;
	return 1;
}

/*-------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
	const void *pvValue)
{
	struct Node *psCurr;
	const void *pvOldValue;

	assert (oSymTable != NULL);
	assert (pcKey != NULL);

	psCurr = oSymTable->psHead;

	/* Traverse  oSymTable until finding the key that matchs pcKey. */
	while (psCurr != NULL)
	{
		if (strcmp(psCurr->pcKey, pcKey) == 0) 
		{
			/* Save & return old value, & overwrite with new value */
			pvOldValue = psCurr->pvValue;
			psCurr->pvValue = (void *)pvValue;
			return (void *)pvOldValue;
		}
		psCurr = psCurr->psNext;
	}
	return NULL;
}

/*-------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
	struct Node *psCurr;

	assert(oSymTable != NULL); 
	assert(pcKey != NULL);

	psCurr = oSymTable->psHead;

	/* Traverse list and return 1 if a key in the table matches the 
	   query key. */
	while (psCurr != NULL)
	{
		if (strcmp(pcKey, psCurr->pcKey) == 0) return 1; 
		psCurr = psCurr->psNext;
	}

	/* Return 0 otherwise */
	return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
	struct Node *psCurr;

	assert(oSymTable != NULL);
	assert(pcKey != NULL);

	psCurr = oSymTable->psHead;

	/* Traverse oSymbolTable and return a pointer to the value
	   connected to the query Key */
	while(psCurr != NULL)
	{
		if (strcmp(psCurr->pcKey, pcKey) == 0) 
			return (void *)psCurr->pvValue;
		psCurr = psCurr->psNext;
	}
	return NULL;
}

/*-------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
	struct Node *psCurr;
	struct Node *psPrev;
	const void *pvOldValue;

	assert(oSymTable != NULL);
	assert(pcKey != NULL);

	/* Return if oSymbolTable has no bindings */
	if (oSymTable->uCount == 0) return NULL;

	/* Special case if removed Node is the head of the list.
	   Retrun removed value. */
	if (strcmp(pcKey, oSymTable->psHead->pcKey) == 0)
	{
		psCurr = oSymTable->psHead;
		pvOldValue = psCurr->pvValue;
		oSymTable->psHead = oSymTable->psHead->psNext;
		free((char *)psCurr->pcKey);
		free(psCurr);
		oSymTable->uCount--;
		return (void *)pvOldValue;
	}

	psPrev = oSymTable->psHead;
	psCurr = psPrev->psNext;

	/* Traverse linked list after the head, remove the query key
	   and its associated Node and update connections/count.
	   Return removed value */
	while (psCurr != NULL)
	{
		if (strcmp(pcKey, psCurr->pcKey) == 0)
		{	
			pvOldValue = psCurr->pvValue;
			psPrev->psNext = psCurr->psNext;
			free((char *)psCurr->pcKey);
			free(psCurr);
			oSymTable->uCount--;
			return (void *) pvOldValue;
		}
		psPrev = psCurr;
		psCurr = psCurr->psNext;
	}
	return NULL;
}

/*-------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
	void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
	const void *pvExtra)
{
	struct Node *psCurr;

	assert(oSymTable != NULL);
	assert(pfApply != NULL);

	psCurr = oSymTable->psHead;

	/* Traverse linked list and apply function pfApply to each key,
	   value, and extra value if present */
	while (psCurr != NULL)
	{
		(*pfApply)(psCurr->pcKey, (void *)psCurr->pvValue, 
			(void *)pvExtra);
		psCurr = psCurr->psNext;
	}
}


