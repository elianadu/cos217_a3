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

struct *SymTableNode
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
   struct SymTableNode **psFirstNodes;

   /* The number of bindings */
   size_t uLength;

   /* The index of auBucketCounts that denotes the number of buckets */
   int iBucketIdx;
};

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void)
{
   SymTable_T oSymTable;
   int i;

   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
      return NULL;

   oSymTable->uLength = 0;
    oSymTable->iBucketIdx = 0;

   oSymTable->psFirstNodes = (struct SymTableNode **)malloc(sizeof(struct SymTableNode *) * auBucketCounts[oSymTable->iBucketIdx]);
   if (oSymTable->psFirstNodes == NULL) {
        free(oSymTable);
        return NULL;
   }

   for (i = 0;
        i != auBucketCounts[oSymTable->iBucketIdx];
        i++) {
            oSymTable->psFirstNodes[i] = NULL;
        }
   
   return oSymTable;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable)
{
   struct SymTableNode *psCurrentNode;
   struct SymTableNode *psNextNode;
   int i;

   assert(oSymTable != NULL);

   for (i = 0;
        i != auBucketCounts[oSymTable->iBucketIdx];
        i++) {
            if (oSymTable->psFirstNodes[i] == NULL) continue;
            for (psCurrentNode = oSymTable->psFirstNodes[i];
            psCurrentNode != NULL;
            psCurrentNode = psNextNode) {
                psNextNode = psCurrentNode->psNextNode;
                free((char *) psCurrentNode->pcKey);
                free(psCurrentNode);
            }
        }
    free(oSymTable->psFirstNodes);
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
   size_t uListIdx;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   uListIdx = SymTable_hash(const char *pcKey, auBucketCounts[oSymTable->iBucketIdx]);

    if (!SymTable_contains(oSymTable, pcKey)) {
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
        psNewNode->psNextNode = oSymTable->psFirstNode;
        oSymTable->psFirstNode = psNewNode;
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

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    for (psCurrentNode = oSymTable->psFirstNode;
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
    size_t uListIdx;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    uListIdx = SymTable_hash(const char *pcKey, auBucketCounts[oSymTable->iBucketIdx]);

    for (psCurrentNode = oSymTable->psFirstNode[uListIdx];
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

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    for (psCurrentNode = oSymTable->psFirstNode;
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

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    psPrevNode = oSymTable->psFirstNode;

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        psNextNode = psCurrentNode->psNextNode;
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            pvOldValue = (void*) psCurrentNode->pvValue;
            if (psCurrentNode == oSymTable->psFirstNode) {
                oSymTable->psFirstNode = psNextNode;
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

    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psCurrentNode->psNextNode) 
        (*pfApply)((char*) psCurrentNode->pcKey, (void*) psCurrentNode->pvValue, (void*) pvExtra);
}