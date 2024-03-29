#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define PORT 3535

#define HASH_SIZE 1717
#define NAME_SIZE 32
#define TYPE_SIZE 32
#define BREED_SIZE 16

char choice;

void ingresar(int clientdesc);
void ver(int clientdesc);
void borrar();
void buscar(int clientdesc);
char *lower(char *str);

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

void ingresar(int clientdesc){

  system("clear");
  int s;
  struct dogType *animal = (struct dogType*)malloc(sizeof(struct dogType));

  printf("Digite la informacion del paciente a registrar: ");

  printf("\nNombre: ");
  scanf(" %[^\t\n]s", animal -> nombre);

  strcpy(animal -> nombre, lower(animal -> nombre));

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

  s = send(clientdesc, animal, sizeof(struct dogType), 0);
  if(s < 0){
    perror("Error send");
    exit(-1);
  }


  printf("Hecho!\n");
  printf("Presione cualquier tecla...");
  getchar();
  getchar();
}

void ver(int clientdesc){
  system("clear");
   int pet_amount, num, s, sz;
   int search = 0;
   char option, number[20];
   char *pathname, *historia_contents;
   struct dogType *read_patient = (struct dogType*)malloc(sizeof(struct dogType));
  //
  s = recv(clientdesc, &pet_amount, sizeof(int), 0);
  if(s < 0){
    perror("Error recv");
    exit(-1);
  }

  printf("\nEn el momento existen %d registros.\n", pet_amount);

  do{

    printf("Digite el numero del registro a revisar: ");
    //si la entrada no se puede recibir
    if(!fgets(number, 20, stdin)){
      perror("error recibiendo entrada");
      exit(-1);
    }
    //convertir linea a numero
    search = atoi(number);

  }while(search == 0 || search <= 0 || search > pet_amount);

  search--;
  num = search;

  s = send(clientdesc, &search, sizeof(int), 0);
  if(s < 0){
    perror("Error send");
    exit(-1);
  }
  //
  s = recv(clientdesc, read_patient, sizeof(struct dogType), 0);
  if(s < 0){
    perror("Error recv");
    exit(-1);
  }

  printf("-------Registro %i-------\n", num + 1);

  printf("Nombre: %s\n",    read_patient -> nombre);
  printf("Tipo: %s\n",      read_patient -> tipo);
  printf("Edad: %i\n",      read_patient -> edad);
  printf("Raza: %s\n",      read_patient -> raza);
  printf("Estatura: %i\n",  read_patient -> estatura);
  printf("Peso: %f\n",      read_patient -> peso);
  printf("Sexo: %c\n",      read_patient -> sexo);

  while(1){
    printf("¿Abrir historia médica? S/N: ");
    scanf(" %c", &option);

    if(option == 's' || option == 'S' || option == 'n' || option == 'N'){
      break;
    }

  }

  s = send(clientdesc, &option, sizeof(char), 0);
  if(s < 0){
    perror("Error send");
    exit(-1);
  }

  //si se quiere ver la historia médica
  if(option == 's' || option == 'S'){

    //recibir tamaño de archivo
    s = recv(clientdesc, &sz, sizeof(int), 0);
    if(s < 0){
      perror("Error recv");
      exit(-1);
    }

    if(sz != 0){
      FILE *historia_file = fopen("temp.dat","w+");
      historia_contents = (char*)malloc(sz+1);

      //recibir contenidos de la historia
      s = recv(clientdesc, historia_contents, sz+1, 0);
      if(s < 0){
        perror("Error recv");
        exit(-1);
      }

      fwrite(historia_contents, 1, sz, historia_file);
      fclose(historia_file);
    }

    system("touch temp.dat && nano temp.dat");

    FILE *historia_file = fopen("temp.dat","r");
    if(!historia_file){

      perror("??????\n");
      exit(-1);
    }

    //recibir tamaño del archivo
    fseek(historia_file, 0L, SEEK_END);
    sz = ftell(historia_file);
    rewind(historia_file);

    //enviar nuevo tamaño
    s = send(clientdesc, &sz, sizeof(int), 0);
    if(s < 0){
      perror("Error send");
      exit(-1);
    }

    if(sz > 1){
      //leer datos del nuevo archivo
      fread(historia_contents, 1, sz, historia_file);

      s = send(clientdesc, historia_contents, sz, 0);
      if(s < 0){
        perror("Error send");
        exit(-1);
      }

      system("rm temp.dat");
    }

  }

  printf("Hecho!\n");
  printf("Presione cualquier tecla...");
  getchar();
  getchar();
}

void borrar(int clientdesc){

  int s, pet_amount, search, num;
  char number[20], petname[NAME_SIZE];

  s = recv(clientdesc, &pet_amount, sizeof(int), 0);
  if(s < 0){
    perror("Error recv");
    exit(-1);
  }

  printf("\nEn el momento existen %d registros.\n", pet_amount);

  do{

    printf("Digite el numero del registro a borrar: ");
    //si la entrada no se puede recibir
    if(!fgets(number, 20, stdin)){
      perror("error recibiendo entrada");
      exit(-1);
    }
    //convertir linea a numero
    search = atoi(number);
    printf("\n");

  }while(search <= 0 || search > pet_amount);

  s = send(clientdesc, &search, sizeof(int), 0);
  if(s < 0){
    perror("Error send");
    exit(-1);
  }

  s = recv(clientdesc, petname, NAME_SIZE, 0);
  if(s < 0){
    perror("Error recv");
    exit(-1);
  }

  printf("Borrando a %s...\n", petname);

  printf("Hecho!\n");
  printf("Presione cualquier tecla...");
  getchar();
}

void buscar(int clientdesc){

  int id, counter= 0;
  int s;
  char *search = malloc(NAME_SIZE);
  printf("Digite el nombre del paciente a buscar: ");
  scanf(" %[^\t\n]s", search);

  //conversion para buscar sin distinguir mayúsculas
  search = lower(search);

  s = send(clientdesc, search, NAME_SIZE, 0);
  if(s < 0){
    perror("Error send");
    exit(-1);
  }

  while(id != -1){
      s = recv(clientdesc, &id, sizeof(int), 0);
      if(s < 0){
        perror("Error recv");
        exit(-1);
      }

      if(id != -1){
        printf("ID: %i\n", id);
        counter++;
      }
  }
  free(search);
  printf("Hecho!\n");

  printf("Encontradas %i coincidencias.\n", counter);
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

  int clientdesc, r, s, search;
  char *option;
  struct sockaddr_in client;
  socklen_t len_addr = sizeof(struct sockaddr);
  socklen_t len_addr_in = sizeof(struct sockaddr_in);

  clientdesc = socket(AF_INET,SOCK_STREAM,0);
  if(clientdesc == -1){
      perror("Error creando el socket");
      exit(-1);
  }

  client.sin_family = AF_INET;
  client.sin_port = htons(PORT);
  client.sin_addr.s_addr = inet_addr("127.0.0.1");
  bzero(client.sin_zero,8);

  //intentar conectarse hasta que se pueda
  do{

    r = connect(clientdesc, (struct sockaddr*)&client, len_addr_in);
  }while(r != 0);

  printf("Conectado!\n");
  option = (char*)malloc(sizeof(char)*2);

  while(1){
    system("clear");
    printf("---------------------------------------------\n");
    printf("Sistemas Operativos - Practica 2. Bienvenido.\n");
    printf("\n1. Ingresar paciente.\n");
    printf("2. Ver paciente por numero.\n");
    printf("3. Borrar paciente.\n");
    printf("4. Buscar paciente.\n");
    printf("5. Salir.\n");


    do{

      printf("Seleccione una opcion: ");
      //si la entrada no se puede recibir
      if(!fgets(option, 2, stdin)){
        perror("error recibiendo entrada");
        exit(-1);
      }

      //convertir linea a numero
      search = atoi(option);

    }while(search > 5 || search < 1);

    s = send(clientdesc, option, sizeof(char), 0);
    if(s < 0){
      perror("Error send");
      exit(-1);
    }

    switch(search){
      case 1: ingresar(clientdesc); break;
      case 2: ver(clientdesc); break;
      case 3: borrar(clientdesc);   break;
      case 4: buscar(clientdesc);   break;
      case 5: system("clear"); exit(0);

    }
  }
}
