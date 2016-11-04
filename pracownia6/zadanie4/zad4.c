#include <stdio.h>
#define __USE_GNU
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#include <ucontext.h>
#include <string.h>

void generate_SEGV_MAPERR(void)
{
   char* s = (char*) 0x413534;
   puts(s);
}

void generate_SEGV_ACCERR(void)
{
    int* p = (int*) &generate_SEGV_MAPERR;
    *p = 42;
}

void handler(int signum, siginfo_t* siginfo, void* context)
{
    char buffer[512] = "";

    snprintf(buffer, 512,"si_addr = %llx\n", (unsigned long long) siginfo->si_addr); // uzywa wlasnego buffera
    write(2, buffer, 512); // jest restartowane jesli zostalo przerwane
    memset(buffer, 0, 512);
    
    snprintf(buffer, 512, "si_code = %x\n", siginfo->si_code);
    write(2, buffer, 512);
    memset(buffer, 0, 512);

    ucontext_t* ucontext = (ucontext_t*) context;
    mcontext_t* mcontext = &(ucontext->uc_mcontext);

    snprintf(buffer, 512, "stack pointer = %llx\ninstruction pointer = %llx\n", mcontext->gregs[REG_RSP], mcontext->gregs[REG_RIP]);
    write(2, buffer, 512);
    memset(buffer, 0, 512);
    memset(buffer, 0, 512);

    void *bt_buffer[128];
    char **bt_strings;

    int nptrs = backtrace(bt_buffer, 128);
    bt_strings = backtrace_symbols(bt_buffer, nptrs);
    int i;
    for (i = 0; i < nptrs; i++)
    {
        snprintf(buffer, 512, "%s\n", bt_strings[i]);
        write(2, buffer, 512);
        memset(buffer, 0, 512);
    }
    free(bt_strings);
    _exit(SIGSEGV);
}


int main(int argc, char * const argv[])
{
    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &act, NULL);
    
    if(argc < 2) generate_SEGV_MAPERR();
    else if(argv[1][0] == 'r') generate_SEGV_MAPERR();
    else generate_SEGV_ACCERR();
}
