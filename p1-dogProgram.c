#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>

#define HASH_SIZE 10069
#define KEY_SIZE 33

FILE *current_file;
struct dogType *copyStruct();
void ingresar();
void ver();
void borrar();
void buscar();
void loadHash();

struct dogType{
  char nombre[KEY_SIZE];
  char tipo[KEY_SIZE];
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

void getPatient(int position){

  struct dogType *read_patient = (struct dogType*)malloc(sizeof(struct dogType));
  current_file = fopen("dataDogs.dat", "r");

  //recibir posición máxima
  fseek(current_file, 0L, SEEK_END);
  int eof = ftell(current_file);
  rewind(current_file);

  if(position >= eof){
    perror("El registro no existe.\n");
    printf("------------------------\n");
    exit(-1);
  }

  fseek(current_file, position, SEEK_SET);

  int read_result = fread(read_patient, 1, sizeof(struct dogType), current_file);

  if(read_result != sizeof(struct dogType)){
    perror("La lectura del registro fallo.\n");
    exit(-1);
  }

  printf("Nombre: %s\n",    read_patient -> nombre);
  printf("Tipo: %s\n",      read_patient -> tipo);
  printf("Edad: %i\n",      read_patient -> edad);
  printf("Raza: %s\n",      read_patient -> raza);
  printf("Estatura: %i\n",  read_patient -> estatura);
  printf("Peso: %f\n",      read_patient -> peso);
  printf("Sexo: %c\n",      read_patient -> sexo);

  free(read_patient);
  fclose(current_file);
}

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
    int count = 0;
    // try to find a valid slot
    entry_t *entry = hashtable->entries[slot];

    // no slot means no entry
    if (entry == NULL) {
        printf("No existen registros con el nombre especificado.\n" );
        return;
    }

    // walk through each entry in the slot, which could just be a single thing
    while (entry != NULL) {
        // print value if found
        if (strcmp(entry->key, key) == 0) {
            count++;

            printf("-> ID: %i\n", (entry -> value/sizeof(struct dogType))+1);
        }

        // proceed to next key if available
        entry = entry->next;
    }

    // print number of coincidences
    printf("Encontradas %i coincidencias.\n", count);
    printf("------------------------\n");
    return;
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

ht_t *ht;

void ingresar(){
  current_file = fopen("dataDogs.dat", "a");
  char *pathname = malloc(100);
  struct dogType *animal;
  animal = (struct dogType*)malloc(sizeof(struct dogType));

  printf("Digite la informacion del paciente a registrar: ");

  printf("\nNombre: ");
  scanf(" %[^\t\n]s", animal -> nombre);

  printf("Tipo: ");
  scanf(" %[^\t\n]s", animal -> tipo);

  while(1){
    printf("Edad: ");
    scanf("%i", &animal -> edad);
    if(animal -> edad >= 0){
      break;
    }
  }

  printf("Raza: ");
  scanf(" %[^\t\n]s", animal -> raza);

  while(1){
    printf("Estatura: ");
    scanf("%i", &animal -> estatura);
    if(animal -> estatura > 0){
      break;
    }
  }

  while(1){
    printf("Peso: ");
    scanf("%f", &animal -> peso);
    if(animal -> peso > 0){
      break;
    }
  }

  while(1){
    printf("Sexo: ");
    scanf(" %[^\t\n]c", &animal -> sexo);
    if(animal -> sexo == 'h' || animal -> sexo == 'H' || animal -> sexo == 'm' || animal -> sexo == 'M'){
      break;
    }
  }


  int current_position = ftell(current_file);
  int write_result = fwrite(animal, sizeof(struct dogType), 1, current_file);

  if(write_result != 1){
    perror("No se pudo escribir correctamente");
    exit(-1);
  }

  //crear historia medica
  sprintf(pathname, "cd historias && touch %i.txt", current_position/sizeof(struct dogType));
  system(pathname);

  ht_set(ht, animal -> nombre, current_position);

  fclose(current_file);
  free(animal);
  free(pathname);

  ht_dump(ht);

  printf("Presione cualquier tecla...");
  getchar();
  getchar();
  printf("Hecho!\n");
  printf("--");
}

void ver(){
  int search, num;
  char choice;
  char *pathname;

  current_file = fopen("dataDogs.dat" , "r");
  //recibir tamaño del archivo
  fseek(current_file, 0L, SEEK_END);
  int sz = ftell(current_file);
  //rewind(current_file);

  fclose(current_file);
  int pet_amount = sz/sizeof(struct dogType);

  printf("\nEn el momento existen %d registros.\n", pet_amount);

  while(1){
    printf("Digite el numero del registro a revisar: ");
    scanf("%i", &search);

    if(search > 0 && search <= pet_amount){
      break;
    }
  }

  search--;
  num = search;

  printf("-------Registro %i-------\n", num + 1);
  getPatient(search * sizeof(struct dogType));

  while(1){
    printf("¿Abrir historia medica del paciente? S/N: ");
    scanf(" %[^\t\n]c", &choice);

    if(choice == 's' || choice == 'S'){
      pathname = malloc(100);
      //abrir historia
      sprintf(pathname, "nano historias/%i.txt", num);
      system(pathname);
      free(pathname);
      break;
    }
    else if(choice == 'n' || choice == 'N'){
      break;
    }
    else{
      continue;
    }
  }

  printf("-------------------------\n");

  printf("Hecho!\n");
  printf("Presione cualquier tecla...");
  getchar();
  getchar();
}

void borrar(){
  int search, num, c;
  FILE *new_file;
  char *pathname;

  current_file = fopen("dataDogs.dat" , "r");
  new_file = fopen("tempDogs.dat" , "w");

  //recibir tamaño del archivo fuente
  fseek(current_file, 0L, SEEK_END);
  int sz = ftell(current_file);
  rewind(current_file);

  printf("\nEn el momento existen %d registros.\n", sz/sizeof(struct dogType));

  printf("Digite el numero del registro a borrar: ");
  scanf("%i", &search);

  num = search;
  search--;

  search = search * sizeof(struct dogType);

  //leer todo lo que esta antes de la estructura
  for(int i = 0; i < search; i++){
    c = getc(current_file);
    putc(c, new_file);
  }

  //ignorar la estructura
  fseek(current_file, sizeof(struct dogType), SEEK_CUR);

  //borrar historia medica asociada
  pathname = malloc(100);
  sprintf(pathname, "cd historias && rm %i.txt &> /dev/null ", num - 1 );
  system(pathname);
  free(pathname);

  //renombrar todas las historias medicas para ser consistentes con la nueva tabla
  int remaining_bytes = (sz - ftell(current_file))/sizeof(struct dogType);

  for(int i = 0; i < remaining_bytes; i += sizeof(struct dogType)){
    pathname = malloc(100);
    sprintf(pathname, "cd historias && mv %i.txt %i.txt &> /dev/null", num, num-1 );
    system(pathname);

    free(pathname);
    num++;
  }

  //leer todo lo que esta despues de la estructura
  while(ftell(current_file) < sz){
    c = getc(current_file);
    putc(c, new_file);
  }

  fclose(new_file);
  fclose(current_file);

  //reemplazar el archivo viejo con el nuevo
  system("rm dataDogs.dat && mv tempDogs.dat dataDogs.dat");

  //rehacer tabla hash, dado que borrar une entrada en particular es complejo
  ht = ht_create();
  loadHash();

  fclose(current_file);

  printf("Hecho!\n");
  printf("Presione cualquier tecla...");
  getchar();
  getchar();
}

void buscar(){
  char *search = malloc(KEY_SIZE);
  printf("Digite el nombre del paciente a buscar: ");
  scanf(" %[^\t\n]s", search);

  ht_get(ht, search);

  free(search);
  printf("Hecho!\n");
  printf("Presione cualquier tecla...");
  getchar();
  getchar();
}

void loadHash(){
  char *read_key = malloc(KEY_SIZE);
  int read_result, read_position;
  current_file = fopen("dataDogs.dat" , "r");

  printf("\nCargando tabla hash...");

  //recibir posición del final del archivo
  fseek(current_file, 0L, SEEK_END);
  long eof = ftell(current_file);
  rewind(current_file);

  while(ftell(current_file) < eof){
    read_position = ftell(current_file);
    read_result = fread(read_key, 1, KEY_SIZE, current_file);

    if(read_result != KEY_SIZE){
      perror("La lectura de los registros fallo.\n");
      exit(-1);
    }

    fseek(current_file, sizeof(struct dogType)-KEY_SIZE, SEEK_CUR);

    ht_set(ht, read_key, read_position);
    //free(read_key);
  }

  printf("Listo.\n");

}

int main(){

  ht = ht_create();

  current_file = fopen("dataDogs.dat", "ab+");

  //recibir posición del final del archivo
  fseek(current_file, 0L, SEEK_END);
  long eof = ftell(current_file);
  rewind(current_file);

  if(ftell(current_file) < eof){
    loadHash();
  }

  fclose(current_file);

  while(1){
    printf("-------------------------\n");
    printf("Sistemas Operativos - Practica 1. Bienvenido.\n");
    printf("\n1. Ingresar paciente.\n");
    printf("2. Ver paciente por numero.\n");
    printf("3. Borrar paciente.\n");
    printf("4. Buscar paciente.\n");
    printf("5. Salir.\n");
    printf("Seleccione una opcion: ");
    switch(getchar()){
      case '1': ingresar(); break;
      case '2': ver();      break;
      case '3': borrar();   break;
      case '4': buscar();   break;
      case '5': exit(0);
    }
  }
    return 0;
}
