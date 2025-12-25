#include <stdlib.h>
#include <assert.h>
#include <string.h>




typedef void* data;

struct Node
{
    const char* key;
    data value;
    struct Node* next;
    
};

typedef struct Node node;

struct SymTable
{
    struct Node* first;
    size_t length;  //returns #bindings

};

typedef struct SymTable symtable;


symtable* SymTable_new(void)
{
    symtable* L = (symtable*)malloc(sizeof(symtable));
    if (L == NULL)
    {
        return NULL;
    }
    L->first = NULL;
    L->length = 0;
    assert(L != NULL);
    return L;
}

void SymTable_free(symtable* oSymTable)
{
    assert( oSymTable != NULL);
    node* p = oSymTable->first;
    while (p != NULL)
    {
        node* q = p;
        free(p->key);
        p = p->next;
        free(q);
        oSymTable->length--;
    }
    assert(oSymTable->length == 0);
    free(oSymTable);
}


size_t SymTable_getLength(symtable* oSymTable)
{
    assert(oSymTable != NULL);
    return oSymTable->length;
    
}

int SymTable_contains(symtable* oSymTable, const char *pcKey)
{
    assert(oSymTable != NULL);
    node* p = oSymTable->first;
    while (p != NULL)
    {
        if (strcmp(p->key, pcKey) == 0)
        {
            return 1;
        }
        p = p->next;
        
    }
    return 0;
}



int SymTable_put(symtable* oSymTable,
     const char *pcKey, const void *pvValue)
{
    assert(oSymTable != NULL);
    if (SymTable_contains(oSymTable, pcKey))
    {
        return 0;
    }
    else
    {
        node* N = (node*)malloc(sizeof(node));
        if (N == NULL)
        {
            return 0;
        }
        N->key = (const char*)malloc(strlen(pcKey) + 1);
        strcpy(N->key, pcKey);
        N->value = (void*)pvValue;
        node* p = oSymTable->first;
        oSymTable->first = N;
        N->next = p;
        oSymTable->length++;
        return 1;
        
    }

}

void *SymTable_replace(symtable* oSymTable,
     const char *pcKey, const void *pvValue)
{

    if (SymTable_contains(oSymTable, pcKey))
    {
    node* p = oSymTable->first;
    while (p != NULL)
    {
        if (strcmp(p->key, pcKey) == 0)
        {
            void* oldValue = p->value;
            p->value = pvValue;
            return oldValue;
        }
        p = p->next;
        
    }
        
    }
    else
    {
        return NULL;
    }
}

void *SymTable_get(symtable* oSymTable,
     const char *pcKey)
{

    if (SymTable_contains(oSymTable, pcKey))
    {
    node* p = oSymTable->first;
    while (p != NULL)
    {
        if (strcmp(p->key, pcKey) == 0)
        {
            return p->value;
        }
        p = p->next;
        
    }
        
    }
    else
    {
        return NULL;
    }
}

void *SymTable_remove(symtable* oSymTable,
     const char *pcKey)
{
    node** pp = &(oSymTable->first);
    while (*pp != NULL)
    {
        if (strcmp((*pp)->key, pcKey) == 0)
        {
            data old_value = (*pp)->value;
            node* next_node = (*pp)->next;
            free((*pp)->key);
            free((*pp));
            *pp = next_node;
            oSymTable->length--;
            return old_value;
        }
        pp = &((*pp)->next);
        
    }
    return NULL;
}

void SymTable_map(symtable* oSymTable,
     void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
     const void *pvExtra)
{
    assert(oSymTable != NULL);
    if (pfApply == NULL)
    {
        return;
    }
    node* p = oSymTable->first;
    while (p != NULL)
    {
        (*pfApply)(p->key, p->value, (void*) pvExtra);
        p = p->next;
        
    }

}





typedef symtable* SymTable_T; //client

