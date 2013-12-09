#include "hooks.h"

#include <windows.h>
#include <kcwdebug.h>

#include "inputwriter.h"
#include "outputreader.h"

using namespace KcwSH;

DWORD kcwshInputHook(void *val) {
//     KcwDebug() << "kcwshInputHook was called!";
    InputWriter writer;
    writer.init();
    int ret = writer.exec();
//     KcwDebug() << "inputwriter ended!";
    return ret;
}

DWORD kcwshOutputHook(void *val) {
//     KcwDebug() << "kcwshOutputHook was called!";
    OutputReader reader;
//    reader.addCallback((HANDLE)val);
    reader.init();
    int ret = reader.exec();
//     KcwDebug() << "outputreader ended!";
    return ret;
}

