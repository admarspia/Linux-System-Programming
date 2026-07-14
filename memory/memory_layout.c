#include <stdio.h>
#include <stdlib.h> // For malloc an free

int global_var = 21;
int global_var2 = 22;

int main() {
  int local_var = 1;
  int local_var2 = 2;
  int *heap_var = malloc(sizeof(int));
  int *heap_var2 = malloc(sizeof(int));
  *heap_var = 24;
  *heap_var2 = 24;


  printf("global_var: %p\n", (void*) &global_var);
  printf("global_var2: %p\n", (void*) &global_var2);
  printf("local_var: %p\n", (void*) &local_var);
  printf("local_var2: %p\n", (void*) &local_var);
  printf("heap_var: %p\n", (void*) heap_var);
  printf("heap_var2: %p\n", (void*) heap_var2);


  free(heap_var);

  return 0;
}
