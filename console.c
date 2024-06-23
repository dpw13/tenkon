#include <stdio.h>

void memWrite(const char b, const int addr) {
    *(char *)addr = b;
}

char memRead(const int addr) {
   return *(char *)addr;
}

void run(const int addr) {
    // Do not do this. Seriously under no circumstances
    // should you copy this code into any other program!
    // Should you decide to do so your decendants will
    // curse you and the day you did for generations
    ((void (*)(void))addr)();
}

void loadData(int addr) {
    char value = 0;
    while(scanf("%02hhx", &value)) {
        memWrite(value, addr);
        addr++;
    }
}

void showData(int addr, const int size) {
    printf("        | 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15\n");
    const int target = addr + size;
    addr = (addr / 16) * 16;
    while(addr < target) {
        printf("%08x | ", addr);
        for(int j = 0; j < 16; j++) {
            printf("%02hhx ", memRead(addr));
            addr++;
        }
        printf("\n");
    }
}

void consoleLoop() {
    char cmd;
    int addr = 0;
    int size = 0;
    int line = 0;

    while(1) {  //run forever
        printf("%d >", line);
        line++;
        scanf(" %c", &cmd);
        switch(cmd) {
            case 'L':  //Load
            case 'l':
                scanf("%02x", &addr);
                loadData(addr);
                scanf("%c", &cmd); //Throw away the extra Z
                break;
            case 'P':  //Peek
            case 'p':
                scanf("%02x", &addr);
                scanf("%d", &size);
                showData(addr, size);
                break;
            case 'R':  //Run
            case 'r':
                scanf("%02x", &addr);
                run(addr);
                break;
            default:
                printf("Unrecognized Command: %c\n", cmd);
                break;
        }
    }
}

int main(void) {
    printf("Welcome to the Tenkon Basic Console!\n");
    printf("Commands:\n");
    printf("L addr      - Loads bytes represented in Hex starting at addr.\n");
    printf("              Continues to load bytes until you send Z.\n");
    printf("P addr size - Peeks at size bytes at addr.\n");
    printf("R addr      - Calls subroutine located at addr\n");
    consoleLoop();
    return 0;
}
