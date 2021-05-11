
void bar();

void foo()
{
    bar();
}

void baz()
{
    foo();
}

void bar()
{
    foo();
}

int main(void)
{
    bar();
    baz();
}
