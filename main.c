#include <stdio.h>
#include <stdlib.h>

void c(int16_t A[16]) {
    int32_t sm = 0;
    int8_t count = 0;
    int16_t B[16];
    int16_t* C[16];

    for (size_t i = 0; i < 16; i++) {
        if (A[i] <= -8)
            continue;
        sm += abs(A[i]);
        B[count] = A[i];
        C[count] = A + i;
        count++;
    }

    printf("COUNT: %d\n\t\tSUM: %d\n\t\tB: [", count, sm);
    for (int i = 0; i < count; i++)
        printf("%4d ", B[i]);
    printf("]\n\t\tC: [");
    for (int i = 0; i < count; i++)
        printf("0x%08X\n\t\t    ", C[i]);
    printf("]\n");
}

void f(int16_t A[16]) {
    int32_t sm = 0;
    int8_t count = 0;
    int16_t B[16] = {0};
    int16_t* C[16] = {0};
   
    __asm__ (
        "xor %%cl, %%cl\n\t"         // i <- 0
		"xor %%eax, %%eax\n"         // EAX <- 0

        "1:\n\t"                     // loop: iterate over A
        "movw (%[A]), %%ax\n\t"      // AL <- A[i]
        "cmpw $-8, %%ax\n\t"         // if :w(A[i] <= -8)
        "jle 2f\n\t"                 // continue
        "movw %%ax, (%[B])\n\t"      // B[count] <- A[i]
        "movq %[A], (%[C])\n\t"      // C[count] <- A + i
        "movw %%ax, %%dx\n\t"        // DX <- AX               -|
        "sarw $31, %%dx\n\t"         // DX <- sign bit of A[i]  |
        "xorw %%dx, %%ax\n\t"        // AX <- AX xor DX         |--> abs(A[i])
        "subw %%dx, %%ax\n\t"        // AX <- DX - AX          -|
        "add %%eax, %[sm]\n\t"       // sum += abs(A[i])
        "inc %[count]\n\t"           // count += 1
        "add $2, %[B]\n\t"               // next B
        "add $8, %[C]\n"             // next C

        "2:\n\t"
        "add $2, %[A]\n\t"               // next A
        "inc %%cl\n\t"               // i += 1
        "cmp $16, %%cl\n\t"          // if (i < 16)
        "jl 1b\n"                    //   goto 1
    : [sm] "+r" (sm),
    [count] "+r" (count)
    : [A] "S" (A),
    [B] "D" (B),
    [C] "b" (C)
    : "eax", "dx", "cl"
    );
    
    printf("COUNT: %d\n\t\tSUM: %d\n\t\tB: [", count, sm);
    for (int i = 0; i < count; i++)
        printf("%4d ", B[i]);
    printf("]\n\t\tC: [");
    for (int i = 0; i < count; i++)
        printf("0x%08X\n\t\t    ", C[i]);
    printf("]\n");
}

int main() {
    int16_t AA[][16] = {
        {0},
        {32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767},
        {-20,  20, -17,  17,   8,  -6,   0,   0,   1,   4, -31,  32,   6,   5,  10,   0},
        {-8,   -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  20,  -1,  -1,  -1,  -1,  -2},
        {-8, 1}
    };
    for (int i = 0; i < sizeof(AA) / sizeof(*AA); i++) {
        printf("#%d:\n\tA: [", i);
        for (int j = 0; j < sizeof (*AA) / sizeof (**AA); j++)
            printf("%4d ", AA[i][j]);
        printf("]\n\tC:\n\t\t");
        c(AA[i]);
        printf("\tASM:\n\t\t");
        f(AA[i]);
    }
}
