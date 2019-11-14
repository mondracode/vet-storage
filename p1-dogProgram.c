#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<ctype.h>
#include <strings.h>


#define HASH_SIZE 1717
#define NAME_SIZE 32
#define TYPE_SIZE 32
#define BREED_SIZE 16

FILE *current_file;
void ingresar();
void ver();
void borrar();
void buscar();
char *lower();

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

void get_patient(int number){

  struct dogType *read_patient = (struct dogType*)malloc(sizeof(struct dogType));
  current_file = fopen("dataDogs.dat", "r");

  //recibir posición máxima
  fseek(current_file, 0L, SEEK_END);
  int eof = ftell(current_file);
  rewind(current_file);

  if(number*sizeof(struct dogType) >= eof){
    perror("El registro no existe.\n");
    printf("------------------------\n");
    exit(-1);
  }

  fseek(current_file, number * sizeof(struct dogType), SEEK_SET);

  int read_result = fread(read_patient, 1, sizeof(struct dogType), current_file);

  if(read_result != sizeof(struct dogType)){
    printf("Se leyeron %i bytes.", read_result);
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

void ingresar(){
  system("clear");
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

  int code = hash(animal -> nombre);

  printf("En la tabla está %i", hash_table[code]);

  animal -> previous = hash_table[code];
  hash_table[code] = current_position/sizeof(struct dogType);

  int write_result = fwrite(animal, sizeof(struct dogType), 1, current_file);

  if(write_result != 1){
    perror("No se pudo escribir correctamente");
    exit(-1);
  }

  //crear historia medica
  sprintf(pathname, "cd historias && touch %i.txt", current_position/sizeof(struct dogType));
  system(pathname);

  //Escribir tabla hash
  FILE *new_file = fopen("hashRegisters.dat", "wb");
  fwrite(hash_table, sizeof(int), HASH_SIZE, new_file);
  fclose(new_file);

  fclose(current_file);
  free(animal);
  free(pathname);

  printf("Presione cualquier tecla...");
  getchar();
  getchar();
  printf("Hecho!\n");
  printf("--");
}

void ver(){
  system("clear");
  int search, num;
  char choice;
  char *pathname;

  current_file = fopen("dataDogs.dat" , "r");

  //recibir tamaño del archivo
  fseek(current_file, 0L, SEEK_END);
  int sz = ftell(current_file);
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

  get_patient(search);

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
  int search, num, c, read_result;
  FILE *new_file;
  char *pathname;
  struct dogType *animal = (struct dogType*)malloc(sizeof(struct dogType));

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

  //fread la estructura
  read_result = fread(animal, 1, sizeof(struct dogType), current_file);

  printf("Borrando a %s\n", animal -> nombre);

  if(read_result != sizeof(struct dogType)){
    printf("Se leyeron %i bytes.", read_result);
    perror("La lectura del registro fallo.\n");
    exit(-1);
  }

  int code = hash(animal -> nombre);

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

  //rehacer tabla hash
    //poner nuevo previous
    hash_table[code] = animal -> previous;

    //Escribir tabla hash
    FILE *hash_file = fopen("hashRegisters.dat", "wb");
    fwrite(hash_table, sizeof(int), HASH_SIZE, hash_file);
    fclose(hash_file);

    printf("%i\n", hash_table[hash(animal -> nombre)]);

  //fclose(current_file);
  free(animal);

  printf("Hecho!\n");
  printf("Presione cualquier tecla...");
  getchar();
  getchar();
}

void buscar(){

  struct dogType *read_animal = (struct dogType*)malloc(sizeof(struct dogType));
  char *search = malloc(NAME_SIZE);
  int reg_pos, read_result;
  int flag = 0;
  int counter = 0;

  current_file = fopen("dataDogs.dat" , "r");
  printf("Digite el nombre del paciente a buscar: ");
  scanf(" %[^\t\n]s", search);

  int code = hash(search);

  fseek(current_file, hash_table[code]*sizeof(struct dogType), SEEK_SET);

  while(flag != 2){
    reg_pos = ftell(current_file);
    read_result = fread(read_animal, 1, sizeof(struct dogType), current_file);

    if(read_result != sizeof(struct dogType)){
      printf("Se leyeron %i bytes.", read_result);
      perror("La lectura del registro fallo.\n");
      exit(-1);
    }
    //printf("%s %i %s\n", look, strcmp(look,found), found);

    if(strcmp(look, found) == 0){
      printf("ID: %i\n", (reg_pos/sizeof(struct dogType))+1);
      counter++;
    }

    if(read_animal -> previous == 0){
      flag++;
    }

    rewind(current_file);
    fseek(current_file, read_animal -> previous*sizeof(struct dogType), SEEK_SET);

  }
  free(read_animal);
  free(search);
  printf("Hecho!\n");
  printf("Se encontraron %i registros.\n", counter);
  printf("Presione cualquier tecla...");
  getchar();
  getchar();
}

char *lower(char *str){
  char *mystr = (char*)malloc(32);
  for(int i = 0; str[i]; i++){
    mystr[i] = tolower(str[i]);
  }
  return mystr;
}

int main(){

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

  char *hey = (char*)malloc(10);

  while(1){
    system("clear");
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
      case '5': system("clear"); exit(0);
    }
  }
    return 0;
}
