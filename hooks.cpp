#include "hooks.h"

#include <windows.h>
#include <kcwdebug.h>

#include "inputwriter.h"

DWORD kcwshInputHook(void *val) {
    KcwDebug() << "kcwshInputHook was called!";
    InputWriter writer;
    writer.init();
    return writer.exec();
}

DWORD kcwshOutputHook(void *val) {
    KcwDebug() << "kcwshOutputHook was called!";
    return 0;
}

