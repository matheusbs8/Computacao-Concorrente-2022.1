#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include <pthread.h>
#include "timer.h"
long int size_linha;
long int size_coluna;
double *A, tempofunction=0, *Coeficientes, *ResultadoArq, *ResultadoGauss, *ResultadoFinal;

FILE *p;

//operação fundamental que multiplica a linha de uma matriz por um valor especificado no parametro
void estica_encolhe(int indice, double fator){
    for(int i=0; i<size_coluna; i++)
        A[indice*size_coluna + i] *= fator;  
}

//operação fundamental que faz uma linha receber o seu valor, somado o valor de outra linha multiplicada por uma constante
void soma_subtracao(int i_static,int j_static, double fator){
    for(int i=0; i<size_coluna; i++)
        A[i_static*size_coluna + i] -= fator* A[j_static*size_coluna + i];  
}

//operação fundamental que troca duas linhas da matriz de lugar
void troca(int i_static, int j_static){
    double aux;
    for(int i=0; i<size_coluna; i++){
        aux=A[i_static*size_coluna + i];
        A[i_static*size_coluna + i]=A[j_static*size_coluna + i];
        A[j_static*size_coluna + i]=aux;
    }

}

//função que percorre a coluna da matriz vasculhando se existe um possivel pivo(número diferente de 0) nela
int encontra_pivo(int i,int j){
    int i_troca=-1;
    for(int k = i; k<size_linha; k++){
        if(A[k*size_coluna + j]!=0.0){
            i_troca=k;
            break;
        }
    }
    return i_troca;  
}

//função responsável por zerar os elementos da coluna, exceto a posição que abriga o pivo unitário da coluna
//para cumprir esse papel, ela utiliza a função soma_subtração
void zerar_coluna(int i,int j){
 
    for(int k = 0; k<size_linha; k++){
        if (k!=i)
            soma_subtracao(k, i, A[k*size_coluna +j]);
    }
 }

//função que executa o gauss Jordan na matriz extraida do arquivo
void  Gauss_Jordan(){
    
    int i_atual=0;
    int j_atual=0;

    while(i_atual<size_linha && j_atual<size_coluna){
       
        int i_troca = encontra_pivo(i_atual, j_atual);
            
        if(i_troca == -1){
            if(i_atual==size_linha){
                j_atual=size_coluna;
                continue;
            }
            j_atual+=1;
            continue;
        }
      
        if(i_atual!=i_troca)
            troca(i_atual, i_troca);
       
        estica_encolhe(i_atual, 1.0/A[i_atual*size_coluna + j_atual]);  
        zerar_coluna(i_atual, j_atual);
        
        i_atual+=1;
        j_atual+=1;  
       
    }
}

//função com o papel de verificar se o resultado obtido pelo gauss jordan, é a solução do sistema
//verifica se o resultado obtido pela multiplicação é bem próximo do que foi fornecido pelo arquivo 
int verifica(){
    int aux = 1;
    for(int i=0; i<size_linha; i++){
       
        if(abs(ResultadoArq[i]-ResultadoFinal[i])>0.000000001){
            aux=0;
            break;
        }
    }     
    return aux;
 }

//função com o papel de realizar a multiplicação dos coeficientes com os valores da variáveis encontrados pelo gauss jordan
// o reultado encontrado será comparado com o resultado enviado por arquivo
void ResolveSistema(){
    double aux=0;
    for(int i=0; i<size_linha; i++){
        
      for(int k=0; k<size_linha; k++) 
            aux+=Coeficientes[i*size_linha+ k]*ResultadoGauss[k];
        ResultadoFinal[i]=aux;
        aux=0;
    
    }
}

//função que guarda em uma matriz os coeficientes do sistema, e em um vetor os valores do resultado
void separa(){
    for(int i=0; i<size_linha; i++){
        for(int j=0; j<size_coluna; j++){
            if(j==size_coluna-1){
                ResultadoArq[i] =A[i*size_coluna+j]; 
            }
            else{
                Coeficientes[i*size_linha + j] =A[i*size_coluna+j];   
            }
        }     
                    
    }  
}


//função responsável de guardar os valores da última coluna da matriz que foi aplicada o algoritmo de gauss Jordan
//a execução deste processo é necessário pois, na ultima coluna está gurdada a solução do sistema
void extrairResultado(){
    for(int i=0; i<size_linha; i++){
        ResultadoGauss[i]=A[i*size_coluna + size_coluna-1];
    }    
}


int main(int argc, char* argv[]){

    p = fopen("Sistema" , "rb");

    fread(&size_linha, sizeof(long int), 1, p);
    fread(&size_coluna, sizeof(long int), 1, p);
    
    A = (double *) malloc(sizeof(double) * size_linha * size_coluna);
    if (A == NULL) {printf("ERRO--malloc1\n"); return 2;}

    fread(A, sizeof(double), size_coluna*size_linha, p);

    Coeficientes  = (double *) malloc(sizeof(double) * size_linha * size_linha);
    if (Coeficientes == NULL) {printf("ERRO--malloc2\n"); return 2;}

    ResultadoArq  = (double *) malloc(sizeof(double) * size_linha);
    if (ResultadoArq == NULL) {printf("ERRO--malloc3\n"); return 2;}

    ResultadoGauss  = (double *) malloc(sizeof(double) * size_linha);
    if (ResultadoGauss == NULL) {printf("ERRO--malloc4\n"); return 2;}

    ResultadoFinal  = (double *) malloc(sizeof(double) * size_linha);
    if (ResultadoFinal == NULL) {printf("ERRO--malloc5\n"); return 2;}
    separa();

    double ini, fim, tempoGauss, tempoS; 
    GET_TIME(ini);
    Gauss_Jordan();
    GET_TIME(fim);
    extrairResultado();
    
    ResolveSistema();
    
    tempoGauss=fim-ini;
    printf("tempo de execução: %lf segundos\n",tempoGauss);
   
    if(verifica())
        printf("\nresultado verificado\n");
   
    free(A);
    free(Coeficientes);
    free(ResultadoFinal);
    free(ResultadoArq);
    free(ResultadoGauss);
    return 0;
}
