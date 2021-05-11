
void foo(char *toto)
{
    if (*toto)
        foo(toto + 1);
}

void bar(int a)
{
    if (a < 0)
        foo("toto");
    if (a > 0)
        bar(a - 1);
}

int main(int argc, char **argv)
{
    if (argc == 1)
        foo(argv[0]);
}
