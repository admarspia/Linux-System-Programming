#include <stdio.h>
#include <stdlib.h>

// BAD: returns the address of a local (stack) variable.
// Once bad() returns, x's stack frame is gone — the returned
// pointer is dangling. Reading/writing through it is undefined
// behavior, even though it might "look like it works" sometimes.
int* bad() {
    int x = 1;
    return &x;
}

// GOOD: returns a pointer to heap memory, which stays valid
// until you explicitly free() it — safe to use after the
// function returns.
int* good() {
    int* num = malloc(sizeof(int));
    if (num == NULL) {
        return NULL; // always check malloc's return value
    }
    *num = 1;
    return num;
}

int main() {
    int* p1 = bad();
    printf("bad():  %p -> %d (undefined behavior, dangling pointer)\n", (void*)p1, *p1);

    int* p2 = good();
    printf("good(): %p -> %d (valid, heap-allocated)\n", (void*)p2, *p2);

    free(p2); // free what you malloc'd

    return 0;
}
