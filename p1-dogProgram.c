#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>

#define HASH_SIZE 1069

FILE *current_file;
struct dogType *copyStruct();
void escribir();
void ver();
void borrar();
void buscar();

struct dogType{
  char nombre[33];
  char tipo[17];
  int edad;
  char raza[17];
  int estatura;
  float peso;
  char sexo;
};

typedef struct entry_t {
    char *key;
    struct dogType *value;
    struct entry_t *next;
} entry_t;

typedef struct {
    entry_t **entries;
} ht_t;

ht_t *ht;

unsigned int hash(const char *key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    // do several rounds of multiplication
    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }

    // make sure value is 0 <= value < HASH_SIZE
    value = value % HASH_SIZE;

    return value;
}

entry_t *ht_pair(const char *key, struct dogType *value) {
    // allocate the entry
    entry_t *entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(sizeof(struct dogType));

    // copy the key and value in place
    strcpy(entry->key, key);
    entry -> value = value;

    // next starts out null but may be set later on
    entry->next = NULL;

    return entry;
}

ht_t *ht_create(void) {
    // allocate table
    ht_t *hashtable = malloc(sizeof(ht_t) * 1);

    // allocate table entries
    hashtable->entries = malloc(sizeof(entry_t*) * HASH_SIZE);

    // set each to null (needed for proper operation)
    int i = 0;
    for (; i < HASH_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_set(ht_t *hashtable, const char *key, struct dogType *value) {
    unsigned int slot = hash(key);

    // try to look up an entry set
    entry_t *entry = hashtable->entries[slot];

    // no entry means slot empty, insert immediately
    if (entry == NULL) {
        hashtable->entries[slot] = ht_pair(key, value);
        return;
    }

    entry_t *prev;

    // walk through each entry until either the end is
    // reached or a matching key is found
    while (entry != NULL) {
        // check key
        // if (strcmp(entry->key, key) == 0) {
        //     // match found, replace value
        //     free(entry->value);
        //     entry->value = malloc(sizeof(struct dogType) + 1);
        //     entry -> value = value;
        //     return;
        // }

        // walk to next
        prev = entry;
        entry = prev->next;
    }

    // end of chain reached without a match, add new
    prev->next = ht_pair(key, value);
}

struct dogType *ht_get(ht_t *hashtable, const char *key) {
    unsigned int slot = hash(key);

    // try to find a valid slot
    entry_t *entry = hashtable->entries[slot];

    // no slot means no entry
    if (entry == NULL) {
        return NULL;
    }

    // walk through each entry in the slot, which could just be a single thing
    while (entry != NULL) {
        // return value if found
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }

        // proceed to next key if available
        entry = entry->next;
    }

    // reaching here means there were >= 1 entries but no key match
    return NULL;
}

void ht_del(ht_t *hashtable, const char *key) {
    unsigned int bucket = hash(key);

    // try to find a valid bucket
    entry_t *entry = hashtable->entries[bucket];

    // no bucket means no entry
    if (entry == NULL) {
        return;
    }

    entry_t *prev;
    int idx = 0;

    // walk through each entry until either the end is reached or a matching key is found
    while (entry != NULL) {
        // check key
        if (strcmp(entry->key, key) == 0) {
            // first item and no next entry
            if (entry->next == NULL && idx == 0) {
                hashtable->entries[bucket] = NULL;
            }

            // first item with a next entry
            if (entry->next != NULL && idx == 0) {
                hashtable->entries[bucket] = entry->next;
            }

            // last item
            if (entry->next == NULL && idx != 0) {
                prev->next = NULL;
            }

            // middle item
            if (entry->next != NULL && idx != 0) {
                prev->next = entry->next;
            }

            // free the deleted entry
            free(entry->key);
            free(entry->value);
            free(entry);

            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;

        ++idx;
    }
}

void ht_dump(ht_t *hashtable) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        entry_t *entry = hashtable->entries[i];

        if (entry == NULL) {
            continue;
        }

        printf("slot[%4d]: ", i);

        for(;;) {
            printf("%s=%s ", entry->key, entry->value->raza);

            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }

        printf("\n");
    }
}

struct dogType *copyStruct(struct dogType *src){
  struct dogType *new_dog = (struct dogType*)malloc(sizeof(struct dogType));

  strcpy(new_dog -> nombre, src -> nombre);
  strcpy(new_dog -> tipo , src -> tipo);
  new_dog -> edad = src -> edad;
  strcpy(new_dog -> raza , src -> raza);
  new_dog -> estatura = src -> estatura;
  new_dog -> peso = src -> peso;
  new_dog -> sexo = src -> sexo;

  return new_dog;
}

int main(){
  ht_t *ht = ht_create();

  struct dogType *dog = (struct dogType*)malloc(sizeof(struct dogType));

  strcpy(dog -> nombre, "Argus");
  strcpy(dog -> tipo, "FatCat");
  dog -> edad = 8;
  strcpy(dog -> raza , "No idea");
  dog -> estatura = 34;
  dog -> peso  = 6.55;
  dog -> sexo = 'h';

  ht_set(ht, dog  -> nombre, dog);

  struct dogType *new_dog = copyStruct(dog);

  strcpy(new_dog -> raza , "Gatote");

  ht_set(ht, new_dog -> nombre, new_dog);
  //

  new_dog = copyStruct(dog);

  strcpy(new_dog -> raza , "Shonc");

  ht_set(ht, new_dog -> nombre, new_dog);


  ht_dump(ht);

  printf("Hecho!");
}
