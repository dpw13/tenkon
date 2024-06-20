#if 1

int main(void) {
    while (1);
    return 0;
}

#else

/* We force -O0 for this function because `count` gets optimized away without being used. */
int __attribute__((optimize("O0"))) main(void) {
    int count = 0;
    while (1) count++;
    return 0;
}

#endif