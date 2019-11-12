#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define HASH_SIZE 1717
#define NAME_SIZE 32
#define TYPE_SIZE 32
#define BREED_SIZE 16
#define RANGE 1000

FILE *current_file;
void ingresar();
void ver();
void borrar();
void buscar();

int *hash_table;

struct dogType{
  char nombre[NAME_SIZE];
  char tipo[TYPE_SIZE];
  int edad;
  char raza[BREED_SIZE];
  int estatura;
  float peso;
  char sexo;
  int previous;
};

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

int getEOF(){

  current_file = fopen("dataDogs.dat", "r");
  if(current_file = NULL){
      return 0;
  }

  fseek(current_file, 0L, SEEK_END); //ir hasta el final del archivo
  int eof = ftell(current_file); //guardar dirección
  fclose(current_file);

  return eof;
}

/* Returns a random line (w/o newline) from the file provided */
char* readRandom(const char *filename) {

    size_t lineno = 0;
    size_t selectlen;
    char selected[NAME_SIZE]; /* Arbitrary, make it whatever size makes sense */
    char current[NAME_SIZE];
    selected[0] = '\0'; /* Don't crash if file is empty */

    current_file = fopen(filename, "r"); /* Add your own error checking */
    while (fgets(current, sizeof(current), current_file)) {
        if (drand48() < 1.0 / ++lineno) {
            strcpy(selected, current);
        }
    }
    fclose(current_file);
    selectlen = strlen(selected);
    if (selectlen > 0 && selected[selectlen-1] == '\n') {
        selected[selectlen-1] = '\0';
    }
    return strdup(selected);
}

void assignation(void){
    system("clear");
    int randomNumber;
    struct dogType *data;
    data = (struct dogType*)malloc(sizeof(struct dogType));
    memset(data, 0, sizeof(struct dogType));
    // Generar nombre a partir de archivo
    strcpy(data->nombre, readRandom("nombresMascotas.txt"));

    // Generar tipo
    randomNumber = rand() % 5;
    switch (randomNumber){
    case 0:
        strcpy(data->tipo, "Perro");
        break;
    case 1:
        strcpy(data->tipo, "Gato");
        break;
    case 2:
        strcpy(data->tipo, "Pato");
        break;
    case 3:
        strcpy(data->tipo, "Erizo");
        break;
    case 4:
        strcpy(data->tipo, "Unicornio");
        break;
    default:
        break;
    }

    // Generar edad
    data->edad = (rand()%12)+1;

    // Generar raza
    randomNumber = rand()%5;
    switch (randomNumber){
    case 0:
        strcpy(data->raza, "Criollo");
        break;
    case 1:
        strcpy(data->raza, "Exótico");
        break;
    case 2:
        strcpy(data->raza, "Manchado");
        break;
    case 3:
        strcpy(data->raza, "Rayado");
        break;
    case 4:
        strcpy(data->raza, "Colorido");
        break;
    default:
        break;
    }

    // Generar estatura
    data->estatura = (rand()%100)+10;

    // Generar peso
    data->peso = ((float)rand()/(float)(RAND_MAX/60)) + 1;

    // Generar sexo
    randomNumber = rand()%2;
    switch (randomNumber){
    case 0:
        strcpy(&data->sexo, "M");
        break;
    case 1:
        strcpy(&data->sexo, "F");
        break;
    default:
        break;
    }

    // Guardar archivo
    current_file = fopen("dataDogs.dat", "a");
    if (current_file == NULL){
        perror("Archivo no existe \n");
        exit(-1);
    }
    else{

        fseek(current_file, 0L, SEEK_END); //ir hasta el final del archivo
        int eof = ftell(current_file); //guardar dirección
        rewind(current_file);

        int position = (eof/sizeof(struct dogType));

        int code = hash(data -> nombre);

        data -> previous = hash_table[code];
        hash_table[code] = position;

        printf("Nombre: %s\n", data -> nombre);
        printf("Tipo: %s\n", data -> tipo);
        printf("Raza: %s\n", data -> raza);
        printf("Edad: %i\n", data -> edad);
        printf("Peso: %f\n", data -> peso);
        printf("Posición: %i\n", hash_table[code]);
        printf("Posicion del anterior: %i\n", data -> previous);

        fwrite(data, sizeof(struct dogType), 1, current_file);
        fclose(current_file);
        free(data);

        return;
    }
}

int main(){
  system("touch hashRegisters.dat");

  hash_table = (int*)malloc(HASH_SIZE*sizeof(int));
  bzero(hash_table, HASH_SIZE * sizeof(int));
  FILE *hash_file = fopen("hashRegisters.dat", "rb");

  if(!hash_file){
    perror("Por favor crear estructuras con generator");
    exit(-1);
  }

  //leer tabla hash
  fread(hash_table, sizeof(int), HASH_SIZE, hash_file);
  fclose(hash_file);

   for(int i=0; i < RANGE; i++){
     assignation();
   }

   //Escribir tabla hash
   FILE *new_file = fopen("hashRegisters.dat", "wb");
   fwrite(hash_table, sizeof(int), HASH_SIZE, new_file);
   fclose(new_file);

   printf("Archivo creado y guardado %i \n", sizeof(struct dogType));
   return 0;
}
