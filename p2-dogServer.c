#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include "myqueue.h"

#define PORT 3535
#define BACKLOG 32
#define MAX_PROCESOS 1

#define HASH_SIZE 1717
#define NAME_SIZE 32
#define TYPE_SIZE 32
#define BREED_SIZE 16

//thread pool
pthread_t threads[BACKLOG];
//mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

FILE *current_file;
FILE *hash_file;

void *thread_handler(void *arg);
void *connection_handler(void *clientdesc);
void ingresar();
void ver();
void borrar();
void buscar(int clientdesc);
char *lower();

pid_t x;
int *hash_table;
time_t rawtime;
struct tm * timeinfo;
char buffer [80];
char logg [100];
struct sockaddr_in server, client;

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

struct dogType *get_patient(int number){

  struct dogType *read_patient = (struct dogType*)malloc(sizeof(struct dogType));
  current_file = fopen("dataDogs.dat", "r");

  //recibir posición máxima
  fseek(current_file, 0L, SEEK_END);
  int eof = ftell(current_file);
  rewind(current_file);

  fseek(current_file, number * sizeof(struct dogType), SEEK_SET);

  int read_result = fread(read_patient, 1, sizeof(struct dogType), current_file);

  if(read_result != sizeof(struct dogType)){
    printf("Se leyeron %i bytes.", read_result);
    perror("La lectura del registro fallo.get_patient\n");
    exit(-1);
  }

  fclose(current_file);
  return read_patient;
}

void ingresar(int clientdesc){
  pthread_mutex_lock(&mutex);
  int s;
  current_file = fopen("dataDogs.dat", "a");
  char *pathname = malloc(100);
  struct dogType *animal = (struct dogType*)malloc(sizeof(struct dogType));

  int current_position = ftell(current_file);

  s = recv(clientdesc, animal, sizeof(struct dogType), 0);
  if(s < 0){
    perror("Error recv");
    exit(-1);
  }

  int code = hash(animal -> nombre);

  animal -> previous = hash_table[code];
  hash_table[code] = current_position/sizeof(struct dogType);

  int write_result = fwrite(animal, sizeof(struct dogType), 1, current_file);

  if(write_result != 1){
    perror("No se pudo escribir correctamente");
    exit(-1);
  }

  //crear historia medica
  sprintf(pathname, "touch historias/%i.txt", current_position/sizeof(struct dogType));
  system(pathname);

  //Escribir tabla hash
  FILE *new_file = fopen("hashRegisters.dat", "wb");
  fwrite(hash_table, sizeof(int), HASH_SIZE, new_file);
  fclose(new_file);

  //abrir archivo de logs
  FILE *log_file = fopen("serverDogs.dat", "a");
  if(!log_file){
    log_file = fopen("serverDogs.dat", "w+");
  }

  //hacer linea de log
  strcat(logg, " inserción ");
  strcat(logg, animal -> nombre);
  strcat(logg, "\n");

  //escribir log no binario
  s = fputs(logg, new_file);

  if(s <= 0){
    perror("Error fwrite");
    exit(-1);
  }

  fclose(new_file);
  fclose(current_file);
  free(animal);
  free(pathname);
  pthread_mutex_unlock(&mutex);
}

void ver(int clientdesc){

  pthread_mutex_lock(&mutex);
  struct dogType *read_patient = (struct dogType*)malloc(sizeof(struct dogType));

  int search, num, s;
  char option;
  char *pathname;

  current_file = fopen("dataDogs.dat" , "r");

  //recibir tamaño del archivo
  fseek(current_file, 0L, SEEK_END);
  int sz = ftell(current_file);
  fclose(current_file);

  int pet_amount = sz/sizeof(struct dogType);

  printf("Existen %i registros\n", pet_amount);

  s = send(clientdesc, &pet_amount, sizeof(int), 0);
  if(s < 0){
    perror("Error send");
    exit(-1);
  }

  s = recv(clientdesc, &search, sizeof(int), 0);
  if(s < 0){
    perror("Error recv");
    exit(-1);
  }

  read_patient = get_patient(search);

  s = send(clientdesc, read_patient, sizeof(struct dogType), 0);
  if(s < 0){
    perror("Error send");
    exit(-1);
  }

  s = recv(clientdesc, &option, sizeof(char), 0);
  if(s < 0){
    perror("Error recv");
    exit(-1);
  }

  if(option == 's' || option == 'S'){
    printf("Se solicitó abrir la historia médica %c\n", option);
  }

  if(option == 'n' || option == 'N'){
    printf("Consulta concluida\n");
  }

  // if(choice == 's' || choice == 'S'){
  //   pathname = malloc(100);
  //   //abrir historia
  //   sprintf(pathname, "nano historias/%i.txt", search);
  //
  //   s = send(clientdesc, pathname, 100, 0);
  //   if(s < 0){
  //     perror("Error send");
  //     exit(-1);
  //   }
  //   free(pathname);
  // }
  // else if(choice == 'n' || choice == 'N'){
  //   printf("nel prro\n");
  // }
  //abrir archivo de logs
  FILE *new_file = fopen("serverDogs.dat", "a");
  if(!new_file){
    new_file = fopen("serverDogs.dat", "w+");
  }

  //convertir entero a string para escribir
  char number[8];
  sprintf(number, "%d", search + 1);

  //hacer linea de log
  strcat(logg, " lectura ");
  strcat(logg, number);
  strcat(logg, "\n");

  //escribir log no binario
  s = fputs(logg, new_file);

  if(s <= 0){
    perror("Error fwrite");
    exit(-1);
  }

  fclose(new_file);

  pthread_mutex_unlock(&mutex);
  return;
}

void borrar(int clientdesc){

  pthread_mutex_lock(&mutex);
  int search, num, c, s, read_result, pet_amount;
  FILE *new_file;
  char *pathname;
  struct dogType *animal = (struct dogType*)malloc(sizeof(struct dogType));

  current_file = fopen("dataDogs.dat" , "r"); //archivo actual
  new_file = fopen("tempDogs.dat" , "w"); //archivo nuevo

  //recibir tamaño del archivo fuente
  fseek(current_file, 0L, SEEK_END);
  int sz = ftell(current_file);
  rewind(current_file);

  pet_amount = sz/sizeof(struct dogType);

  s = send(clientdesc, &pet_amount, sizeof(int), 0);
  if(s < 0){
    perror("Error send");
    exit(-1);
  }

  s = recv(clientdesc, &search, sizeof(int), 0);
  if(s < 0){
    perror("Error recv");
    exit(-1);
  }

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

  s = send(clientdesc, animal -> nombre, NAME_SIZE, 0);
  if(s < 0){
    perror("Error send");
    exit(-1);
  }

  if(read_result != sizeof(struct dogType)){
    printf("Se leyeron %i bytes.", read_result);
    perror("La lectura del registro fallo. borrar\n");
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

  //abrir archivo de logs
  FILE *log_file = fopen("serverDogs.dat", "a");
  if(!log_file){
    log_file = fopen("serverDogs.dat", "w+");
  }

  //convertir entero a string para escribir
  char number[8];
  sprintf(number, "%d", num);

  //hacer linea de log
  strcat(logg, " borrado ");
  strcat(logg, number);
  strcat(logg, "\n");

  //escribir log no binario
  s = fputs(logg, log_file);

  if(s <= 0){
    perror("Error fwrite");
    exit(-1);
  }

  fclose(log_file);
  free(animal);
  pthread_mutex_unlock(&mutex);
}

void buscar(int clientdesc){
  pthread_mutex_lock(&mutex);


  struct dogType *read_animal = (struct dogType*)malloc(sizeof(struct dogType));

  int reg_pos, read_result, id, s;
  int flag = 0;
  int counter = 0;
  char *search = malloc(NAME_SIZE);

  current_file = fopen("dataDogs.dat" , "r");


  s = recv(clientdesc, search, NAME_SIZE, 0);
  if(s < 1){
    perror("Error recv en buscar");
    exit(-1);
  }



  int code = hash(search);

  fseek(current_file, hash_table[code]*sizeof(struct dogType), SEEK_SET);

  while(flag < 2){
    reg_pos = ftell(current_file);
    read_result = fread(read_animal, 1, sizeof(struct dogType), current_file);

    if(read_result != sizeof(struct dogType)){
      printf("Se leyeron %i bytes.", read_result);
      perror("La lectura del registro fallo. buscar\n");
      exit(-1);
    }
    //debug
    //printf("%s %i %s\n", search, strcmp(search, read_animal -> nombre), read_animal -> nombre);

    if(strcmp(search, read_animal -> nombre) == 0){
      id = (reg_pos/sizeof(struct dogType))+1; //conversión de dirección de archivo a número de registro
      s = send(clientdesc, &id, sizeof(int), 0);
      if(s < 0){
        perror("Error send");
        exit(-1);
      }
      //printf("ID: %i\n", (reg_pos/sizeof(struct dogType))+1);
      counter++;
    }

    if(read_animal -> previous == 0){
      flag++;
    }

    rewind(current_file);
    fseek(current_file, read_animal -> previous*sizeof(struct dogType), SEEK_SET);

  }

  id = -1;

  s = send(clientdesc, &id, sizeof(int), 0);
  if(s < 0){
    perror("Error send");
    exit(-1);
  }

  //abrir archivo de logs
  FILE *new_file = fopen("serverDogs.dat", "a");
  if(!new_file){
    new_file = fopen("serverDogs.dat", "w+");
  }

  //hacer linea de log
  strcat(logg, " búsqueda ");
  strcat(logg, search);
  strcat(logg, "\n");

  //escribir log no binario
  s = fputs(logg, new_file);
  if(s <= 0){
    perror("Error fwrite");
    exit(-1);
  }

  fclose(new_file);
  free(read_animal);
  free(search);

  pthread_mutex_unlock(&mutex);
  return;
}

char *lower(char *str){
  char *mystr = (char*)malloc(32);
  for(int i = 0; str[i]; i++){
    mystr[i] = tolower(str[i]);
  }
  return mystr;
}

void *thread_handler(void *arg){
  while(1){
    //tomar un cliente de la cola
    pthread_mutex_lock(&mutex);
    int *p_clientdesc = dequeue();
    pthread_mutex_unlock(&mutex);

    if(p_clientdesc != NULL){
      //el cliente existe, entonces hay una conexión
      //toma el tiempo actual



      printf("Conexión establecida.\n");
      //acá se llama a la función que gestiona las conexiones
      connection_handler(p_clientdesc);
    }
  }
}

void *connection_handler(void *p_client){
  int clientdesc = *(int*)p_client;
  int s;

  char choice;

  while(1){
    s = recv(clientdesc, &choice, sizeof(char), 0);
    if(s < 0){
      perror("Error recv");
      exit(-1);
    }

    //limpiar log
    strcpy(logg, "");

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,80,"%F %T ",timeinfo);

    strcat(logg, buffer);
    strcat(logg, "Cliente ");
    strcat(logg, inet_ntoa(server.sin_addr));



    switch(choice){
      case '1': ingresar(clientdesc); break;
      case '2': ver(clientdesc);      break;
      case '3': borrar(clientdesc);   break;
      case '4': buscar(clientdesc);   break;
      case '5': return NULL;
    }
  }
}

int main(){

  char log[60];
  int serverdesc, clientdesc,  check, l, s;

  socklen_t len_addr = sizeof(struct sockaddr);
  socklen_t len_addr_in = sizeof(struct sockaddr_in);



  //cargar tabla hash
  hash_table = (int*)malloc(HASH_SIZE*sizeof(int));
  bzero(hash_table, HASH_SIZE * sizeof(int));
  hash_file = fopen("hashRegisters.dat", "rb");

  if(!hash_file){
    perror("Por favor crear estructuras con generator");
    exit(-1);
  }

  //leer tabla hash
  fread(hash_table, sizeof(int), HASH_SIZE, hash_file);
  fclose(hash_file);

  //inicialización de thread pool
  for (int i = 0; i < BACKLOG; i++) {
    pthread_create(&threads[i], NULL, thread_handler, NULL);
  }

  //inicialización del socket
  serverdesc = socket(AF_INET, SOCK_STREAM, 0);

  if(serverdesc == -1){
    perror("Error creando el socket");
    exit(-1);
  }

  //inicialización de dirección
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = INADDR_ANY;
  bzero((server.sin_zero), 8);

  if(setsockopt(serverdesc, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0){
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(-1);
  }

  //asignación de dirección
  check = bind(serverdesc, (struct sockaddr*)&server, len_addr_in);

  if(check == -1){
    perror("Error asignando dirección");
    exit(-1);
  }

  check = listen(serverdesc, BACKLOG);
  if(check == -1){
    perror("Error en listen()");
    exit(-1);
  }

  while(1){


    //aceptar conexión
    clientdesc = accept(serverdesc, (struct sockaddr*)&server, &len_addr_in);

    if(clientdesc == -1){
      perror("Fallo en el accept");
      exit(-1);
    }


    // printf("IP address is: %s\n", inet_ntoa(client_addr.sin_addr));
    // printf("port is: %d\n", (int) ntohs(client_addr.sin_port));

    int *p_clientdesc = malloc(sizeof(int));
    *p_clientdesc = clientdesc;
    pthread_mutex_lock(&mutex);
    enqueue(p_clientdesc);
    pthread_mutex_unlock(&mutex);
  }

  //menu

  return 0;
}
