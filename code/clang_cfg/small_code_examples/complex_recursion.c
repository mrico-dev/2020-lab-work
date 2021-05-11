
void bar(int n);

void foo(int n)
{
    if (n == 42)
        bar(n);
}

void bar(int n)
{
    if (n == 2)
        foo(n);
}

int main(void)
{
    foo(2);

    return 0;
}
