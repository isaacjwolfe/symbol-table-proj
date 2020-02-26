/*-------------------------------------------------------------------*/
/* symtablehash.h                                                    */
/* Author: Isaac Wolfe                                               */
/* netid: iwolfe                                                     */
/*-------------------------------------------------------------------*/

#include <string.h>
#include "symtable.h"
#include <assert.h>
#include <stdlib.h>

/*-------------------------------------------------------------------*/

/* Size_t value to hold max number of buckets */
static size_t uSequenceMax = 65521;

/* Size_t array to hold incremented sizes for resized tables/arrays */
static size_t uSequence[8] = {509, 1021, 2039, 4093, 8191,
	16381, 32749, 65521};

/*-------------------------------------------------------------------*/

/* Special function to create size_t hash codes for hash table using 
   pcKey and and a count of the number of buckets in the current 
   hash table */
	static size_t SymTable_hash(const char *pcKey, size_t uPhysLength);

/* Special function to resize the current table to the next size in 
   the sequence */
	static void SymTable_resize(SymTable_T oSymTable);


/*-------------------------------------------------------------------*/

/* SymTable is a structure to maintain a counter for the number of 
   bindings in a Symbol Table and a pointer to the "head" Node. */

	struct SymTable 
	{

	/* Count of the binding elements contained in the hash array from 
	   user's point of view. */
		size_t uBindCount;

	/* Count of elements in the hash array that undrlies the
	   SymTable. */
		size_t uPhysLength;

	/* Variable to store which size in the sequence we are at. */
		size_t uSequenceIndex;

	/* The array/table that underlies the SymTable */
		struct Node **ppsTable;
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

	/* Allocate memory and return NULL if it is unsufficient. */
		size_t uSize = sizeof(struct SymTable);
		oSymTable = (SymTable_T)malloc(uSize);
		if (oSymTable == NULL) return NULL;

	/* Initiate SymTable variables. */
		oSymTable->uBindCount = 0;
		oSymTable->uSequenceIndex = 0;
		oSymTable->uPhysLength = uSequence[oSymTable->uSequenceIndex];
		oSymTable->ppsTable = 
		(struct Node**)calloc(oSymTable->uPhysLength, 
			sizeof(struct Node*));

	/* Return NULL and free mem if insufficient energy for ppsArray */
		if (oSymTable->ppsTable == NULL)
		{
			free(oSymTable);
			return NULL;
		}

		return oSymTable;
	}

/*-------------------------------------------------------------------*/

	void SymTable_free(SymTable_T oSymTable)
	{
		struct Node *psCurr;
		struct Node *psTemp;
		size_t uIndex;

		assert(oSymTable != NULL); 

	/* Traverse hash array and free the memory associated with 
	   each Key and Node. */
		for (uIndex = 0; uIndex < oSymTable->uPhysLength; uIndex++)
		{
			psCurr = oSymTable->ppsTable[uIndex];
			while (psCurr != NULL)
			{
				psTemp = psCurr->psNext;
				free((char *)psCurr->pcKey);
				free(psCurr);
				psCurr = psTemp;
			}
		}
		free(oSymTable->ppsTable);
		free(oSymTable);
	}

/*-------------------------------------------------------------------*/

	size_t SymTable_getLength(SymTable_T oSymTable)
	{
		assert(oSymTable != NULL);

	/* Return length count from SymTable structure. */
		return oSymTable->uBindCount;
	}

/*-------------------------------------------------------------------*/

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */

	static size_t SymTable_hash(const char *pcKey, size_t uPhysLength)
	{
		const size_t HASH_MULTIPLIER = 65599;
		size_t u;
		size_t uHash = 0;

		assert(pcKey != NULL);

		for (u = 0; pcKey[u] != '\0'; u++)
			uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

		return uHash % uPhysLength;
	}

/*-------------------------------------------------------------------*/

/* Take an oSymTable object and copies the elments over into an 
   expanded array that is the size of the next number in the 
   predetermined sequence */

	static void SymTable_resize(SymTable_T oSymTable)
	{
		size_t uIndex;
		size_t uHashedIndex;
		size_t uSequenceIndex;
		struct Node *psCurr;
		struct Node *psTemp;
		struct Node **ppsExpandedTable;

		assert(oSymTable != NULL);

		/* Increment index of size sequence and allocate memory 
		   for the exapnded array. */
		oSymTable->uSequenceIndex++;
		uSequenceIndex = oSymTable->uSequenceIndex;
		ppsExpandedTable = 
		(struct Node**)calloc(uSequence[uSequenceIndex],
			sizeof(struct Node*));

		/* Return if there is insufficient memory. 
		   I just return instead of returning NULL because this is a
		   static function. */
		if (ppsExpandedTable == NULL) return;

		/* Traverse through old array and hash each key into a bucket
		   in the new expanded array. */
		for (uIndex = 0; uIndex != oSymTable->uPhysLength; uIndex++)
		{
			for (psCurr = oSymTable->ppsTable[uIndex];
				psCurr != NULL; psCurr = psTemp)
			{
				psTemp = psCurr->psNext;
				uHashedIndex = SymTable_hash(psCurr->pcKey, 
					uSequence[uSequenceIndex]);
				psCurr->psNext = ppsExpandedTable[uHashedIndex];
				ppsExpandedTable[uHashedIndex] = psCurr;
			}
		}

		/* Free the old Array and update the Array pointer and 
		   PhysLength variable in oSymTable */
		free(oSymTable->ppsTable);
		oSymTable->ppsTable = ppsExpandedTable;
		oSymTable->uPhysLength = 
		uSequence[uSequenceIndex];
	}

/*-------------------------------------------------------------------*/

	int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
		const void *pvValue)
	{
		struct Node *psNodePut;
		size_t uHashedIndex;

		assert(oSymTable != NULL);
		assert(pcKey != NULL);

	/* Return 0 if oSymTable already contains pcKey... */
		if (SymTable_contains(oSymTable, pcKey) == 1) return 0;

	/* Check if table needs to be expanded, and expand if so 
	   while ensuring that max expansion has not been 
	   reached (comment out for non-expanding) */
		if (oSymTable->uBindCount == oSymTable->uPhysLength)
		{
			if (oSymTable->uPhysLength != uSequenceMax)
			{
				SymTable_resize(oSymTable);
			}
		} 

	/* ...if not, allocate memory for the node and key in copies
	   and check to ensure there is suffcient memory. 
	   Return 0 if not */
		psNodePut = (struct Node*)malloc(sizeof(struct Node));
		if (psNodePut == NULL) return 0;
		psNodePut->pcKey = (char*)malloc(strlen(pcKey) + 1);
		if (psNodePut->pcKey == NULL) return 0;

	/* Copy over pvValue to psPutNode. */
		psNodePut->pvValue = pvValue;

	/* Get a hash code for the new Key, then copy pcKey and 
	   psNExt to  psPutNode. */
		uHashedIndex = SymTable_hash(pcKey, oSymTable->uPhysLength);
		strcpy((char*)psNodePut->pcKey, pcKey);
		psNodePut->psNext = oSymTable->ppsTable[uHashedIndex];
		oSymTable->ppsTable[uHashedIndex] = psNodePut;

		/* Increment binding count and return. */
		oSymTable->uBindCount++;
		return 1;
	}

/*-------------------------------------------------------------------*/

	void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
		const void *pvValue)
	{
		struct Node *psCurr;
		const void *pvOldValue;
		size_t uHashedIndex;

		assert (oSymTable != NULL);
		assert (pcKey != NULL);

		uHashedIndex = SymTable_hash(pcKey, oSymTable->uPhysLength);
		psCurr = oSymTable->ppsTable[uHashedIndex];

	/* Traverse nodes at hash location until finding the key that 
	   matches pcKey. */
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
		size_t uHashedIndex;

		assert(oSymTable != NULL); 
		assert(pcKey != NULL);

		uHashedIndex = SymTable_hash(pcKey, oSymTable->uPhysLength);
		psCurr = oSymTable->ppsTable[uHashedIndex];

	/* Traverse nodes in hash key location and return 1 if a key in 
	   the node key matches the query key. */
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
		size_t uHashedIndex;

		assert(oSymTable != NULL);
		assert(pcKey != NULL);

		uHashedIndex = SymTable_hash(pcKey, oSymTable->uPhysLength);
		psCurr = oSymTable->ppsTable[uHashedIndex];

	/* Traverse nodes in the correct bucket and return a pointer 
	   to the value connected to the query Key */
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
		size_t uHashedIndex;

		assert(oSymTable != NULL);
		assert(pcKey != NULL);

	/* Return NULL if oSymbolTable has no bindings */
		if (oSymTable->uBindCount == 0) return NULL;

	/* Get hash index and return if no nodes exist at this index. */
		uHashedIndex = SymTable_hash(pcKey, oSymTable->uPhysLength);
		if (oSymTable->ppsTable[uHashedIndex] == NULL) return NULL;

	/* Otherwise proceed with traversing nodes. First we check if
	   it is a special case of the query Node being the first in the
	   relavant bucket. If so, we remove it below and return the
	   removed value. */
		if (strcmp(pcKey, 
			oSymTable->ppsTable[uHashedIndex]->pcKey) == 0)
		{
			psCurr = oSymTable->ppsTable[uHashedIndex];
			pvOldValue = psCurr->pvValue;
			oSymTable->ppsTable[uHashedIndex] = 
			oSymTable->ppsTable[uHashedIndex]->psNext;
			free((char *)psCurr->pcKey);
			free(psCurr);
			oSymTable->uBindCount--;
			return (void *)pvOldValue;
		}

		psPrev = oSymTable->ppsTable[uHashedIndex];
		psCurr = psPrev->psNext;

	/* Traverse nodes after the first node in a bucket;
	   remove the query key and its associated Node and update 
	   connections/count. Then return removed value */
		while (psCurr != NULL)
		{
			if (strcmp(pcKey, psCurr->pcKey) == 0)
			{	
				pvOldValue = psCurr->pvValue;
				psPrev->psNext = psCurr->psNext;
				free((char *)psCurr->pcKey);
				free(psCurr);
				oSymTable->uBindCount--;
				return (void *)pvOldValue;
			}
			psPrev = psCurr;
			psCurr = psCurr->psNext;
		}
		return NULL;
	}

/*-------------------------------------------------------------------*/

	void SymTable_map(SymTable_T oSymTable,
		void (*pfApply)(const char *pcKey, void *pvValue, 
			void *pvExtra),const void *pvExtra)
	{
		struct Node *psCurr;
		size_t uIndex;

		assert(oSymTable != NULL);
		assert(pfApply != NULL);

	/* Traverse all the buckets and the traverse through each Node
	   in each bucket and apply function pfApply to each key,
	   value, and extra value if present */
		for (uIndex = 0; uIndex <oSymTable->uPhysLength; uIndex++)
		{
			psCurr = oSymTable->ppsTable[uIndex];
			while (psCurr != NULL)
			{
				(*pfApply)(psCurr->pcKey, (void *)psCurr->pvValue, 
					(void *)pvExtra);
				psCurr = psCurr->psNext;
			}
		}
	}
