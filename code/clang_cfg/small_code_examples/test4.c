#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    open("number42.txt", O_RDONLY);

    if (argc==42)
        close(3);
    
    return 0;
}
