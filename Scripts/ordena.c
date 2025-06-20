// Lê os números reais de um arquivo binário, os ordena e os escreve de volta no arquivo

#include <stdio.h>
#include <stdlib.h>

#define ARQUIVO "numeros.dat"
#define MAXVAL 100000

float value[MAXVAL] ;
int num_values ;

int main (int argc, char *argv[])
{
  FILE* arq ;
  int i, j, menor ;
  float aux ;

  // abre o arquivo em leitura/escrita, preservando o conteúdo
  arq = fopen (ARQUIVO, "r+") ;
  if (!arq)
  {
    perror ("Erro ao abrir arquivo") ;
    exit (1) ;
  }

  // lê números do arquivo no vetor
  num_values = fread (value, sizeof (float), MAXVAL, arq) ;
  printf ("Encontrei %d numeros no arquivo\n", num_values) ;

  // ordena os numeros (por seleção)
  for (i = 0; i < num_values-1; i++)
  {
    // encontra o menor elemento no restante do vetor
    menor = i ;
    for (j = i+1; j < num_values; j++)
      if (value[j] < value[menor])
        menor = j ;

    // se existe menor != i, os troca entre si
    if (menor != i)
    {
      aux = value[i] ;
      value[i] = value[menor] ;
      value[menor] = aux ;
    }
  }

  // retorna o ponteiro ao início do arquivo
  rewind (arq) ;

  // escreve números do vetor no arquivo
  fwrite (value, sizeof (float), num_values, arq) ;

  // fecha o arquivo
  fclose (arq) ;
  return (0) ;
}