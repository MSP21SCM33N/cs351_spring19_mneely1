#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/* Daniel J. Bernstein's "times 33" string hash function, from comp.lang.C;
   See https://groups.google.com/forum/#!topic/comp.lang.c/lSKWXiuNOAk */
unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

hashtable_t *make_hashtable(unsigned long size) {
  hashtable_t *ht = malloc(sizeof(hashtable_t));
  ht->size = size;
  ht->buckets = calloc(sizeof(bucket_t *), size);
  return ht;
}

void ht_put(hashtable_t *ht, char *key, void *val) {
  /* FIXME: the current implementation doesn't update existing entries */
  unsigned int idx = hash(key) % ht->size;
  bucket_t *node = ht->buckets[idx]; // Created a bucket pointer nodeto get the element in the hashtable
  
  while(node != NULL){
     if(strcmp(key, node->key) == 0){
         free(key); // Freeing the given key to use
         free(node->val);
         node->val = val;
         return;
     }
     node = node->next;
  }
   
  bucket_t *b = malloc(sizeof(bucket_t));// implements a new node bucket
  b->key = key;
  b->val = val;
  b->next = ht->buckets[idx];
  ht->buckets[idx] = b;
  
}

void *ht_get(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx];
  while (b) {
    if (strcmp(b->key, key) == 0) {
      return b->val;
    }
    b = b->next;
  }
  return NULL;
}

void ht_iter(hashtable_t *ht, int (*f)(char *, void *)) {
  bucket_t *b;
  unsigned long i;
  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      if (!f(b->key, b->val)) {
        return ; // abort iteration
      }
      b = b->next;
    }
  }
}

void free_hashtable(hashtable_t *ht) {
  bucket_t *node;
  bucket_t *temp; // Created to point to the same memory space as the node pointer
  for(int i = 0; i <ht->size; i++){
      node = ht->buckets[i];
      while (node != NULL){
        temp = node; // Allows us to free the memory space as the node while still being able to access the next node
        node = node->next;
        free(temp->key);
        free(temp->val);
        free(temp);
      }
  }
  free(ht->buckets);
  free(ht) // FIXME: must free all substructures!
}

/* TODO */
void  ht_del(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *node = ht->buckets[idx];
  bucket_t *temp;
  
  if(strcmp(node->key, key)==0){
      ht->buckets[idx] = node->next; // Sets the current head node to the next node
      free(node->val);
      free(node->key);
      free(node);
      return;
  }

   while(node != NULL){
       if(strcmp(node->next->key, key)==0){
           if(node->next == node->next->next){// the last item in the last points to itself
               free(node->next->val);
               free(node->next->key);
               free(node->next);
               node->next = node;
               return;
           }
           else{
               temp= node->next->next;// Stores the node after the one we are trying to delete
               free(node->next->val);
               free(node->next->key);
               free(node->next);
               node->next = temp;// Now the node points to the next node after the deleted one
               return;

           }
       }
       node = node->next;
   }


}

void  ht_rehash(hashtable_t *ht, unsigned long newsize) {
  hashtable_t *new_hashtable = make_hashtable(newsize);
  bucket_t *node;
  bucket_t *temp;
  for(int i = 0; i < ht->size; i++){
    node = ht->buckets[i];
    while(node != NULL){
      temp = node;
      ht_put(new_hashtable,node->key,node->val);
      node = node->next;
      free(temp);
    }
  }
  free(ht->buckets);
  ht->size = newsize;
  ht->buckets = new_hashtable->buckets;
  free(new_hashtable);
}
