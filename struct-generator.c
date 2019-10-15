#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define KEY_SIZE 33
#define RANGE 10000000

FILE *current_file;

// ESTRUCTURA dogType
struct dogType{
  char nombre[KEY_SIZE];
  char tipo[KEY_SIZE];
  int edad;
  char raza[17];
  int estatura;
  float peso;
  char sexo;
};

/* Returns a random line (w/o newline) from the file provided */
char* readRandom(const char *filename) {

    size_t lineno = 0;
    size_t selectlen;
    char selected[KEY_SIZE]; /* Arbitrary, make it whatever size makes sense */
    char current[KEY_SIZE];
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
    data->edad = (rand()%100)+1;

    // Generar raza
    randomNumber = rand()%5;
    switch (randomNumber){
    case 0:
        strcpy(data->raza, "Criollo");
        break;
    case 1:
        strcpy(data->raza, "Calvo");
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
    data->peso = random()%60 + 1;

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
    FILE *filePtr = fopen("dataDogs.dat", "ab");
    if (filePtr == NULL){
        perror("Archivo no existe \n");
        exit(-1);
    } else{

        fwrite(data, sizeof(struct dogType), 1, filePtr);

        fclose(filePtr);
        free(data);
        return;
    }
}

int main(){
   for(int i=0; i < RANGE; i++){
     assignation();
   }
   printf("Archivo creado y guardado \n");
   return 0;
}
