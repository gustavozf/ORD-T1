#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>

#define DELIM_STR '|'
#define fld_to_recbuff(rb,fld) strcat((rb),(fld)); \
                               strcat((rb),"|")

//Variaveis Globais
FILE * fd;

typedef struct {
    int head, prox, atual, anterior;
}LED;

//Funcoes
void continuar(){
  printf("\n\nPressione qualquer botao para continuar...");
  getchar();
  __fpurge(stdin);
  system("clear");
}

void insercao(){
    int i, tam, aux1, inserido = 0, num_lidos = 1, tam_reg;
    LED LED;
    char aux[90], buffer[512], c, prox[20];
    char *prompt[] = {
            "    Inscricao: ",
            "         Nome: ",
            "        Curso: ",
            "        Score: ",
            "" //null string to terminate the prompt loop
    };

    buffer[0] = '\0';
    __fpurge(stdin);

    for (i = 0; prompt[i] != ""; i++){
        printf("%s", prompt[i]);
        fgets(aux, 90, stdin); //Pega as informacoes
        aux[strlen(aux)-1] = '\0';
        __fpurge(stdin);
        fld_to_recbuff(buffer, aux); //Salva em um buffer, que formara o novo registro
    }
    tam = strlen(buffer); //Pega o tamanho desse buffer
    fseek(fd,0, SEEK_SET); //vai pro inicio do arq
    fread(&aux1, sizeof(int), 1, fd); //Le o LED Head
    LED.head = LED.atual = LED.anterior = aux1; //salva o valor do LED Head, indicando o ponteiro e o LED Head
    while(!inserido && num_lidos) {
        if (LED.atual == -1) {
            inserido = 1; //para o enquanto
            fseek(fd, 0, SEEK_END); //move o cabecote para o final do arquivo
            fwrite(&tam, sizeof(int), 1, fd); // escreve o tamanho do registro
            fputs(buffer, fd); // escreve o registro

        } else {
            fseek(fd, LED.atual - sizeof(int), SEEK_SET); //para antes do int do registro
            num_lidos = fread(&tam_reg, sizeof(int), 1, fd); //le o tam do reg
            //printf("tam = %d, tam_reg = %d, LED HEAD = %d, LEAD atual = %d\n", tam, tam_reg, LED.head, LED.atual);
            if (tam <= tam_reg) {
                inserido = 1; // para parar o enquanto
                fseek(fd, 2, SEEK_CUR); // pula o *|
                i = 0;
                while ((c = fgetc(fd)) != DELIM_STR) { //Le o ponteiro do registro
                    prox[i++] = c;
                }
                prox[i] = '\0';
                LED.prox = atoi(prox); // salva o proximo
                fseek(fd, LED.atual, SEEK_SET);
                //fwrite(buffer, tam, 1, fd);
                fputs(buffer, fd);
                if (LED.head == LED.atual) { // Se inserirmos na Head
                    fseek(fd, 0, SEEK_SET);
                    aux1 = LED.prox;
                    fwrite(&aux1, sizeof(int), 1, fd); // a head vira o proximo da lista
                } else { // caso contrario
                    fseek(fd, LED.anterior + 2, SEEK_SET); //vai ate o anterior da lista (pulando o *|
                    prox[i++] = '|';
                    prox[i] = '\0';
                    //printf("\nProx do if interno: ");
                    //puts(prox);
                    fputs(prox, fd); // adiciona ao ponteiro dele o proximo da lista
                }
            } else {
                LED.anterior = LED.atual;
                fseek(fd, LED.atual + 2*sizeof(char), SEEK_SET); // pula o *|
                i = 0;
                while ((c = fgetc(fd)) != DELIM_STR) { //Le o ponteiro do registro
                    prox[i++] = c;
                }
                prox[i] = '\0';
                //printf("\nProx do if externo: ");
                //puts(prox);
                LED.atual = atoi(prox); // salva o proximo
            }
        }
    }

    printf("\nInserido com sucesso!");
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
  dados_inline = fopen("dados-inline.txt", "r"); // abre o dados inline pra leitura
  fd = fopen("registros.txt", "w"); // apre pra escrita dos registros
  fseek(fd, sizeof(int), SEEK_SET);
  while((tam = readreg(aux, dados_inline)) > 0){ //enquanto houver registros para ler
      fwrite(&tam, sizeof(int), 1, fd); //escreve o tam do registro retornado por readreg()
      fputs(aux, fd); //escreve no arquivo o buffer
  }
  fseek(fd, 0, SEEK_SET);
  fwrite(&LEDhead, sizeof(int), 1, fd);
  fclose(dados_inline);
  fclose(fd); //Fecha, pois esta apenas como leitura
  fd = fopen("registros.txt", "r+"); //abre para leitura e escrita
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
        importa_dados();
    } else {// Caso ele exista
      fclose(fd); //Fecha, pois esta apenas como leitura
      fd = fopen("registros.txt", "r+"); //abre para leitura e escrita
    }

    while((escolha = menu()) != 0){
        switch(escolha){
            //Importar
            case 1:
                //remove("registros.txt");
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
                system("clear");
                printf("Insira os dados a serem registrados:\n");
                insercao();
                continuar();

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
