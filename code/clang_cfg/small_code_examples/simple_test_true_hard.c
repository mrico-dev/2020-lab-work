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
    if (a % 2 == 0)
    {
        if (a % 4 == 0)
            open(a, O_RDONLY);

        bar(a / 2);
    }

    close(a);
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

