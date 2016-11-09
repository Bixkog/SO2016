#include "strcnt.h"
#include "strdrop.h"
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

int main(void)
{
    void* handle;
    int (*strcnt_p)(const char*, const char*);
    int (*strdrop_p)(char*, const char*);
    char* error;
    int blocker;

    printf("Przed wczytaniem libstr.so\n");
    scanf("%d", &blocker);
    handle = dlopen("libstr.so", RTLD_LAZY);
    printf("Po wczytaniem libstr.so\n");
    scanf("%d", &blocker);
    
    if(!handle)
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    dlerror();
    strcnt_p = (int (*)(const char*, const char*)) dlsym(handle, "strcnt");
    strdrop_p = (int (*)(char*, const char*))dlsym(handle, "strdrop");
    
    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }
    
    char test[] = "Ala ma kota, a kot ma Ale";
    char* set = " A";
    strdrop_p(test, set);
    printf("Left: %d\n", strcnt_p(test, set));
    printf("%s\n", test);
    dlclose(handle);
    return 0;
}
