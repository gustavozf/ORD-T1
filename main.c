#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DELIM_STR '|'

//Variaveis Globais
FILE * fd;

//Estrutura
typedef struct{
  int offset;
  int inscricao;
}indices;

//Funcoes
void continuar(){
  printf("\n\nPressione qualquer botao para continuar...");
  getchar();
  __fpurge(stdin);
  system("clear");
}


void remover(char * numero_inscricao){
  int tam, num_lidos = 1, encontrado = 0, LEDhead, offset;
  char buffer[100], *inscricao;

  offset = 2* sizeof(int); //comeca contando o cabecalho
  fseek(fd, 0, SEEK_SET); // move o cabecote para o inicio
  fread(&LEDhead, sizeof(int), 1, fd); //pula o cabecalho e salva o LED head
  printf("\nLED Head: %d", LEDhead);
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
      printf("\nRegistro excluido com sucesso!\n");
    }
    offset += sizeof(int) + tam; // Calculo do offset
  }

  if(!encontrado){
    printf("\nRegistro Nao Encontrado!\n\n");
  }
}

void busca(char * numero_inscricao){
  int tam, i, j, encontrado = 0, comparacao, num_lidos = 1;
  char aux[7], aux2[25], c;
  char * reg[] = {"Nome: ", "Curso: ", "Score: "};

  //fd = fopen("registros.txt", "r");
  fseek(fd, sizeof(int), SEEK_SET);// le depois do cabeçalho
  while(!encontrado && num_lidos){
    num_lidos = fread(&tam, sizeof(int), 1, fd); // se o retorno for 0, chegou ao final do arquivo
    fgets(aux, 7, fd); //7, pois le ate n-1
    comparacao = strcmp(aux, numero_inscricao); //lembrando que se forem iguais, o retorno eh 0
    if (!comparacao){ //se forem iguais
      encontrado = 1; // muda o boolean para true
      printf("\nRegistro Encontrado!\n");
      printf("Numero de Inscricao: ");
      puts(aux);
      fgetc(fd); //anda um '| para frente'
      for(i = 0; i < 3;i++){ //3 vezes, para nome/curso/score
        printf("%s", reg[i]); //printa a string correspondente
        j = 0;
        while((c = fgetc(fd)) != DELIM_STR){ //pega toda a string ate o '|'
          aux2[j++] = c;
        }
        aux2[j] = '\0';
        puts(aux2);
      }
    } else { //caso sejam diferentes
      fseek(fd, tam-6, SEEK_CUR); // tam-6, pois o fgets andou com o ponteiro 6 vezes
    }

  }

  if(!encontrado){
    printf("\nRegistro Nao Encontrado!\n\n");
  }
  //fclose(fd);
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
    int escolha, tam, i, offset;
    int totReg, tamReg;
    char num_busca[7], buffer[500], *regID;

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

    /*
    //faz
    fseek(fd, 0, SEEK_SET);
    fread(&totReg, sizeof(totReg), 1, fd);
    indices index[totReg];
    offset = sizeof(int);

    for(i=0; i<totReg; i++){
        fread(&tamReg, sizeof(tamReg), 1, fd);
        fread(buffer, tamReg, 1, fd);
        regID = strtok(buffer, "|");

        index[i].inscricao = atoi(regID);
        index[i].offset = offset;

        offset += tamReg + sizeof(int);
    }
    */

    while((escolha = menu()) != 0){
        switch(escolha){
            //Importar
            case 1:
                //fd = fopen("registros.txt", "w");
                importa_dados();

                break;
            //Buscar
            case 2:
                system("clear");
                printf("Insira o numero de inscricao a ser buscado: ");
                gets(num_busca);
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
                gets(num_busca);
                __fpurge(stdin);
                remover(num_busca);
                continuar();

                break;
            default:
                system("clear");
                printf("\nEscolha Errada! Tente Novamente!\n");
        }
    }

    fclose(fd);
    printf("\n\nA D E U S\n");
}
