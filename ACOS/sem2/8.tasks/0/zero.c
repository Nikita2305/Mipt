#include <stdio.h>
#include <dlfcn.h>

typedef double (*lib_func)(double);

int main(int argc, char* argv[]) {
    void     *handle  = NULL;
    lib_func  func    = NULL;
    handle = dlopen(argv[1], RTLD_NOW | RTLD_GLOBAL);
    if (handle == NULL)
    {
        perror("dlopen");    
        return -1;
    }

    func = dlsym(handle, argv[2]);
    if (func == NULL) {
        perror("dlsym");
        return -1;
    }
    double x = 0;
    while (scanf("%lf", &x) != EOF) {
        printf("%.3f ", func(x));    
    }
    dlclose(handle);
}
