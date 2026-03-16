/*--------------------------------------------------------------------*/
/* symtable.h                                                              */
/* Author: Eliana Du                                                  */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
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
   int iLength;
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
      free(psCurrentNode);
   }

   free(oSymTable);
}

/*--------------------------------------------------------------------*/

 size_t SymTable_getLength(SymTable_T oSymTable){
    return oSymTable->iLength;
 }

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable,  const char *pcKey, const void *pvValue)
{
   struct SymTableNode *psNewNode;
   char *pcNewKey;

   assert(oSymTable != NULL);

   psNewNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));    
   if (psNewNode == NULL)
      return 0;

    if (SymTable_contains(oSymTable, pcKey)) {
        pcNewKey = (char*)malloc(sizeof(pcKey));
        if (pcNewKey == NULL)
            return 0;
        *pcNewKey = *pcKey;

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

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        psNextNode = psCurrentNode->psNextNode;
        if (*(psCurrentNode->pcKey) == *pcKey){
            pvOldValue = psCurrentNode->pvValue;
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

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        psNextNode = psCurrentNode->psNextNode;
        if (*(psCurrentNode->pcKey) == *pcKey) return 1;
    }
    return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
    return pcKey;
}

/*--------------------------------------------------------------------*/

/* TODO: change iLength */

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
   /*  const void *pvItem;
   struct SymTableNode *psNextNode;

   assert(oSymTable != NULL);
   assert(oSymTable->psFirstNode != NULL);

   pvItem = oSymTable->psFirstNode->pvItem;
   psNextNode = oSymTable->psFirstNode->psNextNode;
   free(oSymTable->psFirstNode);
   oSymTable->psFirstNode = psNextNode;
   return (void*)pvItem; */
   return pcKey;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
     void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
     const void *pvExtra)
{
   /* struct SymTableNode *psCurrentNode;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psCurrentNode->psNextNode)
      (*pfApply)((void*)psCurrentNode->pvItem, (void*)pvExtra); */
}