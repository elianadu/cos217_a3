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
   const void *pvKey;

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
};

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void)
{
   SymTable_T oSymTable;

   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
      return NULL;

   oSymTable->psFirstNode = NULL;
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
    return 0;
 }

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable,  const char *pcKey, const void *pvValue)
{
   struct SymTableNode *psNewNode;

   assert(oSymTable != NULL);

   psNewNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));
   if (psNewNode == NULL)
      return 0;

   psNewNode->pvItem = pvItem;
   psNewNode->psNextNode = oSymTable->psFirstNode;
   oSymTable->psFirstNode = psNewNode;
   return 1;
}

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue);
{
   const void *pvItem;
   struct SymTableNode *psNextNode;

   assert(oSymTable != NULL);
   assert(oSymTable->psFirstNode != NULL);

   pvItem = oSymTable->psFirstNode->pvItem;
   psNextNode = oSymTable->psFirstNode->psNextNode;
   free(oSymTable->psFirstNode);
   oSymTable->psFirstNode = psNextNode;
   return (void*)pvItem;
}

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    return 0;
}

/*--------------------------------------------------------------------*/


void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
    return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
    return 0;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
     void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
     const void *pvExtra);
{
   struct SymTableNode *psCurrentNode;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psCurrentNode->psNextNode)
      (*pfApply)((void*)psCurrentNode->pvItem, (void*)pvExtra);
}