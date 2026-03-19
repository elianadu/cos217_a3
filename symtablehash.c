/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Eliana Du                                                  */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/
/* Global variable(s)*/

static const size_t auBucketCounts[] = {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};

/*--------------------------------------------------------------------*/

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

/*--------------------------------------------------------------------*/

/* Each binding is stored in a SymTableNode.  SymTableNodes are 
pointers to a linked list.  */

struct SymTableNode
{
   /* The key. */
   const char *pcKey;

   /* The value. */
   const void *pvValue;
   
   /* The address of the next SymTableNode. */
   struct SymTableNode *psNextNode;
};

/*--------------------------------------------------------------------*/

/* A SymTable is a pointer to an array of many first pointers. */

struct SymTable
{
   /* A pointer to an array of pointers */
   struct SymTableNode **ppsFirstNodes;

   /* The number of bindings */
   size_t uLength;

   /* The index of auBucketCounts that denotes the number of buckets */
   int iBucketIdx;
};

/*--------------------------------------------------------------------*/

/* Rehashes nodes in oSymTable, links nodes to an expanded array, and frees original array. */

static void SymTable_expansion(SymTable_T oSymTable) {
    size_t i;
    size_t j;
    struct SymTableNode **ppsTempFirstNodes;
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;

    /* Making a temporary pointer to new array of pointers for the expanded table */
    ppsTempFirstNodes = (struct SymTableNode **)malloc(sizeof(struct SymTableNode *) * auBucketCounts[oSymTable->iBucketIdx + 1]);
    if (ppsTempFirstNodes== NULL) {
        return NULL;
    }

    /* Initializing all nodes in new temp array to NULL */
    for (j = 0; j != auBucketCounts[oSymTable->iBucketIdx + 1]; j++) {
        ppsTempFirstNodes[j] = NULL;
    }
    
    /* Looping through through original table to rehash values and put them in the new table */
    for (j = 0;
        j != auBucketCounts[oSymTable->iBucketIdx];
        j++) {
        if (oSymTable->ppsFirstNodes[j] == NULL) continue;
        for (psCurrentNode = oSymTable->ppsFirstNodes[j];
        psCurrentNode != NULL;
        psCurrentNode = psNextNode) {
            psNextNode = psCurrentNode->psNextNode;
            i = SymTable_hash((char *) psCurrentNode->pcKey, auBucketCounts[oSymTable->iBucketIdx + 1]);
            psCurrentNode->psNextNode = ppsTempFirstNodes[i];
            ppsTempFirstNodes[i] = psCurrentNode;
        }
    }
    free(oSymTable->ppsFirstNodes);
    oSymTable->ppsFirstNodes = ppsTempFirstNodes;
    oSymTable->iBucketIdx++;
}


/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void)
{
   SymTable_T oSymTable;
   size_t i;

   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
      return NULL;

   oSymTable->uLength = 0;
    oSymTable->iBucketIdx = 0;

   oSymTable->ppsFirstNodes = (struct SymTableNode **)malloc(sizeof(struct SymTableNode *) * auBucketCounts[oSymTable->iBucketIdx]);
   if (oSymTable->ppsFirstNodes == NULL) {
        free(oSymTable);
        return NULL;
   }

   for (i = 0;
        i != auBucketCounts[oSymTable->iBucketIdx];
        i++) {
            oSymTable->ppsFirstNodes[i] = NULL;
        }
   
   return oSymTable;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable)
{
   struct SymTableNode *psCurrentNode;
   struct SymTableNode *psNextNode;
   size_t i;

   assert(oSymTable != NULL);

   for (i = 0;
        i != auBucketCounts[oSymTable->iBucketIdx];
        i++) {
            if (oSymTable->ppsFirstNodes[i] == NULL) continue;
            for (psCurrentNode = oSymTable->ppsFirstNodes[i];
            psCurrentNode != NULL;
            psCurrentNode = psNextNode) {
                psNextNode = psCurrentNode->psNextNode;
                free((char *) psCurrentNode->pcKey);
                free(psCurrentNode);
            }
        }
    free(oSymTable->ppsFirstNodes);
    free(oSymTable);
}

/*--------------------------------------------------------------------*/

 size_t SymTable_getLength(SymTable_T oSymTable){
    assert(oSymTable != NULL);
    return (size_t) oSymTable->uLength;
 }

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable,  const char *pcKey, const void *pvValue)
{
   struct SymTableNode *psNewNode;
   char *pcNewKey;
   size_t i;
   struct SymTableNode **ppsTempFirstNodes;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

    if (!SymTable_contains(oSymTable, pcKey)) {
        /* Expand if the number of bindings is too large and if the max number of buckets hasn't already been reached */
        if (oSymTable->uLength + 1 == auBucketCounts[oSymTable->iBucketIdx] && (size_t) oSymTable->iBucketIdx < sizeof(auBucketCounts)/sizeof(auBucketCounts[0]) - 1) {
            SymTable_expansion(oSymTable);
        }

        /* Inserting new node into table, potentially after table expansion */

        i = SymTable_hash(pcKey, auBucketCounts[oSymTable->iBucketIdx]);

        psNewNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));    
        if (psNewNode == NULL) {
            return 0;
        }
        pcNewKey = (char*)malloc(strlen(pcKey)+1);
        if (pcNewKey == NULL) {
            free(psNewNode);
            return 0;
        }
        strcpy(pcNewKey, pcKey);

        psNewNode->pcKey = pcNewKey;
        psNewNode->pvValue = pvValue;
        psNewNode->psNextNode = oSymTable->ppsFirstNodes[i];
        oSymTable->ppsFirstNodes[i] = psNewNode;
        oSymTable->uLength += 1;
        return 1;
    }
    else {
        return 0;
    }
}

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;
    void *pvOldValue;
    size_t i;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    i = SymTable_hash(pcKey, auBucketCounts[oSymTable->iBucketIdx]);

    for (psCurrentNode = oSymTable->ppsFirstNodes[i];
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        psNextNode = psCurrentNode->psNextNode;
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            pvOldValue = (void *) psCurrentNode->pvValue;
            psCurrentNode->pvValue = pvValue;
            return pvOldValue;
        }
    }
    return NULL;
}

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;
    size_t i;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    i = SymTable_hash(pcKey, auBucketCounts[oSymTable->iBucketIdx]);

    for (psCurrentNode = oSymTable->ppsFirstNodes[i];
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        psNextNode = psCurrentNode->psNextNode;
        if (!strcmp(psCurrentNode->pcKey, pcKey)) return 1;
    }
    return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;
    size_t i;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    i = SymTable_hash(pcKey, auBucketCounts[oSymTable->iBucketIdx]);

    for (psCurrentNode = oSymTable->ppsFirstNodes[i];
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        psNextNode = psCurrentNode->psNextNode;
        if (!strcmp(psCurrentNode->pcKey, pcKey)) return (void*) psCurrentNode->pvValue;
    }
    return NULL;
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;
    struct SymTableNode *psPrevNode;
    void *pvOldValue;
    size_t i;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    i = SymTable_hash(pcKey, auBucketCounts[oSymTable->iBucketIdx]);
    psPrevNode = oSymTable->ppsFirstNodes[i];

    for (psCurrentNode = oSymTable->ppsFirstNodes[i];
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        psNextNode = psCurrentNode->psNextNode;
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            pvOldValue = (void*) psCurrentNode->pvValue;
            if (psCurrentNode == oSymTable->ppsFirstNodes[i]) {
                oSymTable->ppsFirstNodes[i] = psNextNode;
            }
            else {
                psPrevNode->psNextNode = psNextNode;
            }
            free((char *) psCurrentNode->pcKey);
            free(psCurrentNode);
            oSymTable->uLength -= 1;
            return pvOldValue;
        }
        psPrevNode = psCurrentNode;
    }
    return NULL;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
     void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
     const void *pvExtra)
{
    struct SymTableNode *psCurrentNode;
    size_t i;

    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    for (i = 0;
        i != auBucketCounts[oSymTable->iBucketIdx];
        i++) {
            if (oSymTable->ppsFirstNodes[i] == NULL) continue;
            for (psCurrentNode = oSymTable->ppsFirstNodes[i];
            psCurrentNode != NULL;
            psCurrentNode = psCurrentNode->psNextNode) {
                (*pfApply)((char*) psCurrentNode->pcKey, (void*) psCurrentNode->pvValue, (void*) pvExtra);
            }
        }
}