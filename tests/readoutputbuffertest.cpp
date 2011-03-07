#include "kcwprocess.h"
#include "kcwapp.h"
#include "kcwinjector.h"

int main(int argc, char**argv) {
    KcwApp app;
    KcwProcess proc("cmd.exe");
    if(!proc.start()) {
        printf("process failed to start!");
        return -1;
    }

    KcwInjector injector;
    injector.setDestinationProcess(proc.process(), proc.thread());
    injector.setInjectionDll("testinjector.dll");
    if(!injector.inject()) {
        printf("failed to inject dll");
        return -1;
    }

    app.addCallback(proc.process());
    proc.resume();
    return app.exec();
}