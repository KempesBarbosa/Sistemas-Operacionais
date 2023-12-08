/*
ALUNO - GABRIEL SOUTO LOZANO BARBOSA
        20220080955
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>


// FUNÇAO RESPONSÁVEL POR REALIZAR A SEQUÊNCIA DE FIBONACCI. 
void fibonacci(int n) {
    int term1 = 0, term2 = 1;
    int nextTerm;
    printf("Série de Fibonacci com %d termos:\n", n);

    for (int i = 0; i < n; i++) {
        printf("%d ", term1);
        nextTerm = term1 + term2;
        term1 = term2;
        term2 = nextTerm;
    }
    printf("\n");
}

int main() {
    pid_t pid; // REPRESENTAÇÃO DO IDs DO PROCESSO
    int total_termos;

    printf("Digite o número de termos para a série de Fibonacci: ");
    scanf("%d", &total_termos);

    // RESTRIÇÃO DE ERRO, CASO SEJA DIGITADO UM NÚMERO INVALIDO PARA EXECUÇÃO DO FIBONACCI
    if (total_termos < 0){

        printf("A quantidade de termos deve ser um número maior que 0!");
        return 1;
    }

    // CRIA PROCESSO FILHO
    pid = fork(); // CRIA UMA CÓPIA DO PROCESSO PAI E ARMAZENA O ID DO FILHO

    if (pid < 0) { // CASO RETORNE UM VALOR NEGATIVO DA ERRO

        perror("Erro ao criar o processo filho"); 
        return 1;

    } else if (pid == 0) {

        // EXECUÇÃO DO CÓDIGO QUE O PROCESSO FILHO É RESPONSÁVEL ( FIBONACCI)
        printf("Processo filho em execução, gerando série de Fibonacci:\n");
        fibonacci(total_termos);
        printf ("Execução terminada.");
        exit(0);  // O processo filho termina aqui
    } 
    
    else {

        // Processo pai
        printf("Processo pai esperando pelo término do processo filho...\n");

        wait(NULL); //FUNÇÃO DE BLOQUEIO
        printf("Processo filho terminou. Continuando a execução do processo pai.\n");
    }

    return 0;
}
