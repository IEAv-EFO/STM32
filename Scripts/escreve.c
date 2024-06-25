// Escreve N valores reais aleatórios em um arquivo, em formato binário

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARQUIVO "numeros.dat"
#define NUMVAL 10

int main (int argc, char *argv[])
{
  FILE* arq ;
  int i, ret ;
  float value[NUMVAL] ;

  // abre o arquivo em modo "append"
  arq = fopen (ARQUIVO, "a") ;
  if (!arq)
  {
    perror ("Erro ao abrir arquivo") ;
    exit (1) ;
  }

  // inicia gerador de números aleatórios
  srand (clock()) ;

  // gera NUMVAL valores aleatórios reais
  for (i = 0; i < NUMVAL; i++)
    value[i] = rand() + (rand() / 100000.0) ;

  // escreve os valores gerados no final do arquivo
  ret = fwrite (value, sizeof (float), NUMVAL, arq) ;
  if (ret)
    printf ("Gravou %d valores com sucesso!\n", ret) ;
  else
    printf ("Erro ao gravar...\n") ;

  // fecha o arquivo
  fclose (arq) ;
  return (0) ;
}