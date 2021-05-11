#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    while (1)
    {
        if (argc == 1)
            open("toto.txt", 0);
        
        close(3);
    }

    return 1;
}
