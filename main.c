#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>

#define DELIM_STR '|'

//Variaveis Globais
FILE * fd;

//Funcoes
void continuar(){
  printf("\n\nPressione qualquer botao para continuar...");
  getchar();
  __fpurge(stdin);
  system("clear");
}


void remover(char * numero_inscricao){
  int tam, num_lidos, encontrado, LEDhead, offset;
  char buffer[100], *inscricao;

  encontrado = 0;
  num_lidos = 1;
  offset = 2* sizeof(int); //comeca contando o cabecalho
  fseek(fd, 0, SEEK_SET); // move o cabecote para o inicio
  fread(&LEDhead, sizeof(int), 1, fd); //pula o cabecalho e salva o LED head
  //printf("\nLED Head: %d", LEDhead);
  while(!encontrado && num_lidos){ //Enquanto nao encontrar e tiver linhas
    num_lidos = fread(&tam, sizeof(int), 1, fd);
    fread(buffer, tam, 1, fd);
    inscricao = strtok(buffer, "|");
    if (!strcmp(inscricao,numero_inscricao)){ //se forem iguais
      encontrado = 1; // muda o boolean para true
      printf("\nRegistro Encontrado!\n");
      fseek(fd, 0, SEEK_SET);
      fwrite(&offset, sizeof(int), 1, fd);
      fseek(fd, offset, SEEK_SET);
      fprintf(fd, "*|%d|", LEDhead);
      printf("Registro excluido com sucesso!\n");
    }
    offset += sizeof(int) + tam; // Calculo do offset
  }

  if(!encontrado){
    printf("\nRegistro Nao Encontrado!\n\n");
  }
}

void busca(char * numero_inscricao){
  int tam, i, encontrado , num_lidos = 1;
  char buffer[100], *aux;
  char * reg[] = {"Nome: ", "Curso: ", "Score: "};

  encontrado = 0;
  fseek(fd, sizeof(int), SEEK_SET);// le depois do cabeçalho
  while(!encontrado && num_lidos){
    num_lidos = fread(&tam, sizeof(int), 1, fd); // se o retorno for 0, chegou ao final do arquivo
    fread(buffer, tam, 1, fd);
    aux = strtok(buffer, "|");
    if (!strcmp(aux, numero_inscricao)){ //se forem iguais
      encontrado = 1; // muda o boolean para true
      printf("\nRegistro Encontrado!\n");
      printf("Numero de Inscricao: ");
      puts(aux);
      for(i = 0; i < 3;i++){ //3 vezes, para nome/curso/score
        printf("%s", reg[i]); //printa a string correspondente
        puts(strtok(NULL, "|"));
      }
    }
  }

  if(!encontrado){
    printf("\nRegistro Nao Encontrado!\n\n");
  }
}

int readreg(char aux[], FILE * arq){
    int cont, i;
    char c;

    cont = 0;
    i = 0;
     //Cada registro tem 4 campos, entao o numero de delimitadores eh tbm 4
    while(cont < 4 && ((c = fgetc(arq)) != EOF)){
        if(c == DELIM_STR){ //se o caracter for delimitador
            cont++;//incrementa o delimitador
        }
        aux[i++] = c; //buffer recebe o caracter
    }
    aux[i] = '\0';//fim da string

    return i; //retorna tamanho da string
}

void importa_dados(){
  FILE * dados_inline;
  int tam, LEDhead = -1;
  char aux[90];

  system("clear");
  dados_inline = fopen("dados-inline.txt", "r");
  fseek(fd, sizeof(int), SEEK_SET);
  while((tam = readreg(aux, dados_inline)) > 0){ //enquanto houver registros para ler
      fwrite(&tam, sizeof(int), 1, fd); //escreve o tam do registro retornado por readreg()
      fputs(aux, fd); //escreve no arquivo o buffer
  }
  fseek(fd, 0, SEEK_SET);
  fwrite(&LEDhead, sizeof(int), 1, fd);
  fclose(dados_inline);
  printf("Arquivo de Registros Criado!\n\n");

}

int menu(){
    int escolha;

    printf("ULTIMATE PROGRAMA OF ARQUIVOS\n");
    printf("1 - Importar os dados\n");
    printf("2 - Buscar os dados\n");
    printf("3 - Inserir os dados\n");
    printf("4 - Remover os dados\n");
    printf("0 - Sair\n");
    printf("Digite sua escolha: ");
    scanf("%d", &escolha);
    __fpurge(stdin);

    return escolha;
}

//Main
void main() {
    int escolha;
    char num_busca[7];

    system("clear");
    fd = fopen("registros.txt", "r");
    if (fd == NULL){// Caso o registro nao exista
        fd = fopen("registros.txt", "w");
        importa_dados();
        fclose(fd); //Fecha, pois esta apenas como leitura
        fd = fopen("registros.txt", "r+"); //abre para leitura e escrita
    } else {// Caso ele exista
      fclose(fd); //Fecha, pois esta apenas como leitura
      fd = fopen("registros.txt", "r+"); //abre para leitura e escrita
    }

    while((escolha = menu()) != 0){
        switch(escolha){
            //Importar
            case 1:
                importa_dados();

                break;
            //Buscar
            case 2:
                system("clear");
                printf("Insira o numero de inscricao a ser buscado: ");
                fgets(num_busca, 7, stdin);
                num_busca[6] = '\0';
                __fpurge(stdin);
                busca(num_busca);
                continuar();

                break;
            //Inserir
            case 3:


                break;
            //Remover
            case 4:
                system("clear");
                printf("Insira o numero de inscricao a ser removido: ");
                fgets(num_busca, 7, stdin);
                num_busca[6] = '\0';
                __fpurge(stdin);
                remover(num_busca);
                continuar();

                break;
            default:
                system("clear");
                printf("Escolha Errada! Tente Novamente!\n\n");
        }
    }

    fclose(fd);
    printf("\n\nA D E U S\n");
}
