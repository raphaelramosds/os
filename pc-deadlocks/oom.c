#include <stdlib.h>

// sudo dmesg -c

int main() {
    size_t i;
    size_t len = 1024*1024*1024;
    for (;;) {
        char* ptr = malloc(len);
        // Caso não consiga alocar, fique preso no loop
        if (!ptr) continue;
        // Tocar nas paginas da alocação
        for (i = 0; i < len; i += 4096) ptr[i] = 0;
    }
    
}