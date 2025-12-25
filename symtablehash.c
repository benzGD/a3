
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>


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

typedef void* data;

typedef struct Binding node;
struct Binding
{
    const char* key;
    data value;
    node* next;
    
};

struct SymTable
{
    size_t capacity;
    node** array;
    size_t bindings_count;  //returns #bindings

};
typedef struct SymTable symtable;

symtable* SymTable_new(void)
{
    symtable* ST = (symtable*)malloc(sizeof(symtable));
    if (ST == NULL)
    {
        return NULL;
    }
    ST->capacity = 509;
    ST->bindings_count = 0;
    ST->array = (node**)calloc(509, sizeof(node*));
    if (ST->array == NULL)
    {
        free(ST);
        return NULL;
    }
    return ST;
}

void SymTable_free(symtable* oSymTable)
{
    assert( oSymTable != NULL);
    node** p = oSymTable->array;

    for (size_t i = 0; i < oSymTable->capacity; i++)
    {
        while (p[i] != NULL)
        {
            node* q = p[i];
            free(p[i]->key);
            p[i] = p[i]->next;
            free(q);
            oSymTable->bindings_count--;
        }
    }
    
    assert(oSymTable->bindings_count == 0);
    free(oSymTable->array);
    free(oSymTable);
}

size_t SymTable_getLength(symtable* oSymTable)
{
    assert(oSymTable != NULL);
    return oSymTable->bindings_count;
    
}

int SymTable_contains(symtable* oSymTable, const char *pcKey)
{
    assert(oSymTable != NULL);
    size_t index = SymTable_hash(pcKey, oSymTable->capacity);
    node* curr = (oSymTable->array)[index];
    while (curr != NULL)
    {

        if (strcmp(curr->key, pcKey) == 0)
        {
            return 1;
        }
        curr = curr->next;
        
    }
    return 0;
}

void resize(symtable* oSymTable)
{

    const int primes[8] = {
    509, 1021, 2039, 4093, 8191, 16381, 32749, 65521 };

    for (size_t i = 0; i < 7; i++)
    {
        if (oSymTable->bindings_count == primes[i])
        {

        int n = primes[i+1];
        node** old_array = oSymTable->array;
        node** new_array = (node**)calloc(n, sizeof(node*));
        if (new_array == NULL)
        {
            return;
        }

        for (size_t i = 0; i < oSymTable->capacity; i++)
        {
            node* curr = (oSymTable->array)[i];
            while (curr != NULL)
            {
                node* q = curr;
                size_t index = SymTable_hash(q->key, n);
                node* p = new_array[index];
                new_array[index] = q;
                curr = curr->next;
                q->next = p;
            }
        }
        
        free(old_array);
        oSymTable->array = new_array;
        oSymTable->capacity = n;
        }
        
    }
 
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
        if (oSymTable->bindings_count == oSymTable->capacity && oSymTable->capacity != 65521)
        {
            resize(oSymTable);
        }
        
        N->key = (const char*)malloc(strlen(pcKey) + 1);
        strcpy(N->key, pcKey);
        N->value = (void*)pvValue;
        size_t index = SymTable_hash(pcKey, oSymTable->capacity);
        node* p = (oSymTable->array)[index];
        (oSymTable->array)[index] = N;
        N->next = p;
        oSymTable->bindings_count++;
        return 1;
        
    }

}


void *SymTable_replace(symtable* oSymTable,
     const char *pcKey, const void *pvValue)
{

    if (SymTable_contains(oSymTable, pcKey))
    {
    size_t index = SymTable_hash(pcKey, oSymTable->capacity);
    node* curr = (oSymTable->array)[index];
    while (curr != NULL)
    {
        if (strcmp(curr->key, pcKey) == 0)
        {
            void* oldValue = curr->value;
            curr->value = pvValue;
            return oldValue;
        }
        curr = curr->next;
        
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
    size_t index = SymTable_hash(pcKey, oSymTable->capacity);

    node* curr = (oSymTable->array)[index];
    while (curr != NULL)
    {
        if (strcmp(curr->key, pcKey) == 0)
        {
 
            return curr->value;
        }
        curr = curr->next;
        
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
    size_t index = SymTable_hash(pcKey, oSymTable->capacity);
    node** pp = &((oSymTable->array)[index]);
    while (*pp != NULL)
    {
        if (strcmp((*pp)->key, pcKey) == 0)
        {
            data old_value = (*pp)->value;
            node* next_node = (*pp)->next;
            free((*pp)->key);
            free((*pp));
            *pp = next_node;
            oSymTable->bindings_count--;
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
    for (size_t i = 0; i < oSymTable->capacity; i++)
    {
        node* curr = (oSymTable->array)[i];
        while (curr != NULL)
        {
            (*pfApply)(curr->key, curr->value, (void*) pvExtra);
            curr = curr->next;
          
        }
    }

}



typedef symtable* SymTable_T; //client

