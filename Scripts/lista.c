// Lista o conteúdo de um arquivo que contém números reais em formato binário 

#include <stdio.h>
#include <stdlib.h>

#define ARQUIVO "numeros.dat"

int main (int argc, char *argv[])
{
  FILE* arq ;
  float value ;

  // abre o arquivo em modo leitura
  arq = fopen (ARQUIVO, "r") ;
  if (!arq)
  {
    perror ("Erro ao abrir arquivo") ;
    exit (1) ;
  }

  // lê e imprime os valores contidos no arquivo
  fread (&value, sizeof (float), 1, arq) ;
  while (! feof (arq))
  {
    printf ("%f\n", value) ;
    fread (&value, sizeof (float), 1, arq) ;
  }

  // fecha o arquivo
  fclose (arq) ;
  return (0) ;
}