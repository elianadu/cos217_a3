/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Eliana Du                                                  */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

/* Each binding is stored in a SymTableNode.  SymTableNodes are linked 
to form a list.  */

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

/* A SymTable is a "dummy" node that points to the first SymTableNode. */

struct SymTable
{
   /* The address of the first SymTableNode. */
   struct SymTableNode *psFirstNode;

   /* The number of bindings */
   size_t iLength;
};

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void)
{
   SymTable_T oSymTable;

   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
      return NULL;

   oSymTable->psFirstNode = NULL;
   oSymTable->iLength = 0;
   
   return oSymTable;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable)
{
   struct SymTableNode *psCurrentNode;
   struct SymTableNode *psNextNode;

   assert(oSymTable != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
   {
      psNextNode = psCurrentNode->psNextNode;
      free((char *) psCurrentNode->pcKey);
      free(psCurrentNode);
   }

   free(oSymTable);
}

/*--------------------------------------------------------------------*/

 size_t SymTable_getLength(SymTable_T oSymTable){
    assert(oSymTable != NULL);
    return (size_t) oSymTable->iLength;
 }

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable,  const char *pcKey, const void *pvValue)
{
   struct SymTableNode *psNewNode;
   char *pcNewKey;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   assert(pvValue != NULL);

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
        oSymTable->iLength += 1;
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
    assert(pvValue != NULL);

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

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    for (psCurrentNode = oSymTable->psFirstNode;
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

    psPrevNode = psCurrentNode;

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
            oSymTable->iLength -= 1;
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
    assert(pvExtra != NULL);

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psCurrentNode->psNextNode) 
        (*pfApply)((char*) psCurrentNode->pcKey, (void*) psCurrentNode->pvValue, (void*) pvExtra);
}