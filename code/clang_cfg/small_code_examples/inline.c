#include <stdio.h>

void bar(int i);

inline void foo(int i)
{
    if (i < 5)
        bar(i + 1);
}

inline void bar(int i)
{
    puts("Hello");
}

int main(int argc, char** argv)
{
    (void) argv;

    foo(42);
    if (argc == 1)
        bar(10);
}

