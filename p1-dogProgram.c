#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>

#define HASH_SIZE 1069

FILE *current_file;
struct dogType *copyStruct();
void ingresar();
void ver();
void borrar();
void buscar();

struct dogType{
  char nombre[33];
  char tipo[33];
  int edad;
  char raza[17];
  int estatura;
  float peso;
  char sexo;
};

typedef struct entry_t {
    char *key;
    int value;
    struct entry_t *next;
} entry_t;

typedef struct {
    entry_t **entries;
} ht_t;

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

entry_t *ht_pair(const char *key, int value) {
    // allocate the entry
    entry_t *entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    //entry->value = malloc(sizeof(int));

    // copy the key and value in place
    strcpy(entry->key, key);
    entry->value = value;

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

void ht_set(ht_t *hashtable, const char *key, int value) {
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

        // walk to next
        prev = entry;
        entry = prev->next;
    }

    // end of chain reached without a match, add new
    prev->next = ht_pair(key, value);
}

void ht_get(ht_t *hashtable, const char *key) {
    unsigned int slot = hash(key);

    // try to find a valid slot
    entry_t *entry = hashtable->entries[slot];

    // no slot means no entry
    if (entry == NULL) {
        //print hey theres nothing
        return;
    }

    // walk through each entry in the slot, which could just be a single thing
    while (entry != NULL) {
        // print value if found
        if (strcmp(entry->key, key) == 0) {
            //print something
        }

        // proceed to next key if available
        entry = entry->next;
    }

    // print number of coincidences
    return;
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
            //free(entry->value);
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
            printf("%s=%i ", entry->key, entry->value);

            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }

        printf("\n");
    }
}

void ingresar(){
  current_file = fopen("dataDogs.dat", "a");
  struct dogType *animal;
  animal = (struct dogType*)malloc(sizeof(struct dogType));

  printf("Digite la informacion del paciente a registrar: ");

  printf("\nNombre: ");
  scanf(" %[^\t\n]s", animal -> nombre);

  printf("Tipo: ");
  scanf(" %[^\t\n]s", animal -> tipo);

  printf("Edad: ");
  scanf("%i", &animal -> edad);
  if(animal -> edad < 0){
    perror("Edad no valida");
    exit(-1);
  }

  printf("Raza: ");
  scanf(" %[^\t\n]s", animal -> raza);

  printf("Estatura: ");
  scanf("%i", &animal -> estatura);
  if(animal -> estatura < 0){
    perror("Estatura no valida");
    exit(-1);
  }

  printf("Peso: ");
  scanf("%f", &animal -> peso);
  if(animal -> peso < 0){
    perror("Peso no valido");
    exit(-1);
  }

  printf("Sexo: ");
  scanf(" %[^\t\n]c", &animal -> sexo);
  if(animal -> sexo != 'h' && animal -> sexo != 'H' && animal -> sexo != 'm' && animal -> sexo != 'M'){
    perror("Sexo no valido");
    exit(-1);
  }

  int write_result = fwrite(animal, sizeof(struct dogType), 1, current_file);

  if(write_result != 1){
    perror("No se pudo escribir correctamente");
    exit(-1);
  }

  fclose(current_file);
  free(animal);


  printf("Presione cualquier tecla...");
  getchar();
  getchar();
  printf("Hecho!\n");
  printf("--");
}

void ver(){

}

void borrar(){

}

void buscar(){
  char *search = malloc(33);
  printf("Digite el nombre del paciente a buscar: ");
  scanf(" %[^\t\n]s", search);



  printf("Hecho!\n");
  printf("Presione cualquier tecla...");
  getchar();
  getchar();
  printf("--");
}

void loadHash(){
  struct dogType *reader;
  int read_result;
  current_file = fopen("dataDogs.dat" , "r");

  printf("\nCargando tabla hash...");

  //recibir posición del final del archivo
  fseek(current_file, 0L, SEEK_END);
  long eof = ftell(current_file);
  rewind(current_file);

  while(ftell(current_file) < eof){


  }
  printf("Listo.\n");
  fclose(current_file);
}

int main(){

  printf("Sistemas Operativos - Practica 1. Bienvenido.\n");
  printf("%i\n", sizeof(struct dogType*));
  printf("%i\n", sizeof(struct dogType));
  printf("%i", sizeof(int));

  // while(1){
  //   printf("\n1. Ingresar paciente.\n");
  //   printf("2. Ver paciente por numero.\n");
  //   printf("3. Borrar paciente.\n");
  //   printf("4. Buscar paciente.\n");
  //   printf("5. Salir.\n");
  //   printf("Seleccione una opcion: ");
  //   switch(getchar()){
  //     case '1': ingresar(); break;
  //     case '2': ver();      break;
  //     case '3': borrar();   break;
  //     case '4': buscar();   break;
  //     case '5': exit(0);
  //   }
  // }

  ht_t *ht = ht_create();

    ht_set(ht, "name1", 1);
    ht_set(ht, "name2", 234);
    ht_set(ht, "name3", 567);
    ht_set(ht, "name4", 890);
    ht_set(ht, "name5", 69);
    ht_set(ht, "name6", 420);
    ht_set(ht, "Argusito", 666);
    ht_set(ht, "Argusito", 667);
    ht_set(ht, "Argusito", 676);
    ht_set(ht, "Argusito", 766);
    ht_set(ht, "Argusito", 668);
    ht_set(ht, "Argusito", 686);
    ht_set(ht, "Argusito", 866);

    ht_dump(ht);

    return 0;
}
