

int main(){

  while(1){
    system("clear");
    printf("-------------------------\n");
    printf("Sistemas Operativos - Practica 2. Bienvenido.\n");
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
}
