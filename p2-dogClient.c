#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#define PORT 3535

int choice;

void ingresar();
void ver();
void borrar();
void buscar();

void ingresar(){
}

void ver(){
}

void borrar(){
}
void buscar(){
}


int main(){

  int clientdesc, r;
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

    send(clientdesc, &choice, sizeof(int), 0);

    switch(choice){
      case '1': ingresar(); break;
      case '2': ver();      break;
      case '3': borrar();   break;
      case '4': buscar();   break;
      case '5': system("clear"); exit(0);
    }
  }
}
