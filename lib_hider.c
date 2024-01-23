#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

static int (*original_main)(int, char **, char **);

void hide_arguments(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        char *malloced_argument = malloc((strlen(argv[i]) + 1) * sizeof(char));
        strcpy(malloced_argument, argv[i]);
        memset(argv[i], 0, strlen(argv[i]) * sizeof(char));
        argv[i] = malloced_argument;
    }
}

void free_arguments(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        free(argv[i]);
    }
}

int main_hook(int argc, char **argv, char **envp)
{
    hide_arguments(argc, argv);
    int ret = original_main(argc, argv, envp);
    free_arguments(argc, argv);
    return ret;
}

int __libc_start_main(
    int (*main)(int, char **, char **),
    int argc,
    char **argv,
    int (*init)(int, char **, char **),
    void (*fini)(void),
    void (*rtld_fini)(void),
    void *stack_end)
{
    original_main = main;
    typeof(&__libc_start_main) original_libc_start_main = dlsym(RTLD_NEXT, "__libc_start_main");
    return original_libc_start_main(main_hook, argc, argv, init, fini, rtld_fini, stack_end);
}