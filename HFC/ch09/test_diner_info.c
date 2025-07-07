#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *my_env[] = {"JUICE=peach and apple", NULL};

    execle("diner_info", "diner_info", "4", NULL, my_env);

    return 0;
}
