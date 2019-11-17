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

int choice;

void ingresar();
void ver();
void borrar();
void buscar();
char *lower(char *str);

void ingresar(){
}

void ver(){
}

void borrar(){
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
      printf("ID: %i\t\t", id);
      counter++;
      if(counter % 3 == 0){
        printf("\n");
      }
  }
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

  int clientdesc, r, s;
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
    //printf("Intentando conectar...");
    r = connect(clientdesc, (struct sockaddr*)&client, len_addr_in);
  }while(r != 0);

  printf("Conectado!\n");

  while(1){
    system("clear");
    printf("---------------------------------------------\n");
    printf("Sistemas Operativos - Practica 2. Bienvenido.\n");
    printf("\n1. Ingresar paciente.\n");
    printf("2. Ver paciente por numero.\n");
    printf("3. Borrar paciente.\n");
    printf("4. Buscar paciente.\n");
    printf("5. Salir.\n");
    printf("Seleccione una opcion: ");
    choice = getchar();

    s = send(clientdesc, &choice, sizeof(int), 0);
    if(s < 0){
      perror("Error send");
      exit(-1);
    }

    switch(choice){
      case '1': ingresar(); break;
      case '2': ver();      break;
      case '3': borrar();   break;
      case '4': buscar(clientdesc);   break;
      case '5': system("clear"); exit(0);
    }
  }
}
