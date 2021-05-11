#include <fcntl.h>
#include <unistd.h>

void loop()
{
    while(1)
    {
        open("toto.txt", 0);
        close(3);
    }

    return;
}

int main(int argc, char** argv)
{
    loop();
}
