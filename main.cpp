#include "mainwindow.h"
#include <QApplication>
#include <setjmp.h>
#include<signal.h>
#include<threadexceptionhandle.h>
#include <execinfo.h>
jmp_buf env_buf;
#define BACKTRACE_SIZE   16

void dump(void)
{
    int j, nptrs;
    void *buffer[BACKTRACE_SIZE];
    char **strings;

    nptrs = backtrace(buffer, BACKTRACE_SIZE);

    printf("backtrace() returned %d addresses\n", nptrs);

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nptrs; j++)
        printf("  [%02d] symbols:%s\n", j, strings[j]);

    free(strings);
}
void print_stacktrace()
{
    int size = 16;
    void * array[16];
    int stack_num = backtrace(array, size);
    char ** stacktrace = backtrace_symbols(array, stack_num);
    for (int i = 0; i < stack_num; ++i)
    {
        printf("%s\n", stacktrace[i]);
    }
    free(stacktrace);
}
uint  getCurrentFuncReturnAddr()
{
    const int maxLevel = 16;
    void* buffer[maxLevel];
    int level = backtrace(buffer, maxLevel);
    printf("backtrace:\n");
    for(int i=0;i<maxLevel;i++)
    {
        std::cout<<"buffer:"<<buffer[i]<<std::endl;
    }
    uint addr = (unsigned long)buffer[2];
    printf("ParentCallAddr:0x%X\n",addr);
    return addr;
}


void setStackBuf()
{
    const int maxLevel = 1024;
    void* buffer[maxLevel];
    uint buf[maxLevel]={0};
    int level = backtrace(buffer, maxLevel);
    printf("backtrace:\n");
    for(int i=0;i<maxLevel;i++)
    {
        buf[i] = (unsigned long)buffer[i];
    }
    threadExceptionHandle::setStackBuf(buf);
}
void errorDump()
{
    const int maxLevel = 16;
    void* buffer[maxLevel];
    int level = backtrace(buffer, maxLevel);
    printf("backtrace:\n");
    for(int i=0;i<maxLevel;i++)
    {
        std::cout<<"buffer:"<<buffer[i]<<std::endl;
//        printf("%c",(char*)buffer[i]);
    }
    const int SIZE_T = 1024;
    char cmd[SIZE_T] = "addr2line -C -f -e ";
    char* prog = cmd + strlen(cmd);
    readlink("/proc/self/exe", prog, sizeof(cmd) - (prog-cmd)-1);
    FILE* fp = popen(cmd, "w");
    if (!fp)
    {
        perror("popen");
        return;
    }
    for (int i = 0; i < level; ++i)
    {
        fprintf(fp, "%p\n", buffer[i]);
    }
    fclose(fp);
}
void printJmpBuf()
{

}
void recvSignal(int sig)
{
    //print_stacktrace();
//    dump();
    //errorDump();
    printf("received signal %d !!!\n",sig);
    //longjmp(env_buf,1);
    uint id = pthread_self();
    printf("currentThreadId:%d \n",id);
    fflush(0);
    jmp_buf env;

    env[0] = threadExceptionHandle::getJmpbuf(id);
    siglongjmp(env,id);
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    uint id = pthread_self();
    printf("main ThreadId:%d \n",id);
    fflush(0);
    signal(SIGSEGV,recvSignal);//合法地址的非法使用，产生段错误
    signal(SIGFPE, recvSignal);//非法数学运算，比如除0
    signal(SIGSYS, recvSignal);//非法的系统调用
    signal(SIGBUS, recvSignal);//非法地址访问
    MainWindow w;
    w.show();

    return a.exec();
}
