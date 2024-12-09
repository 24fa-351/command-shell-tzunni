#include <stdio.h>

#include "xsh.h"

#define MAX_INPUT_SIZE 1024

int main()
{
  char input[MAX_INPUT_SIZE];

  while (1)
  {
    printf("xsh# ");
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL)
    {
      break;
    }
    parse_and_execute(input);
  }

  return 0;
}
