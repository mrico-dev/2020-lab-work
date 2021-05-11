#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void foo(char *toto)
{
    if (strlen(toto) == 4)
        open("bar.txt", O_RDONLY);
    else
        close(3);
}

void bar(int a)
{
    if (a == 0)
        foo("toto");

    for (int i = 0; i < a; ++i)
        close(i);
}

int main(int argc, char **argv)
{

    if (argc == 1)
    {
        foo(argv[0]);
        close(0);
    }
    else
    {
        bar(argc);
    }

    return 0;
}

