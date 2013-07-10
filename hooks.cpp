#include "hooks.h"

#include <windows.h>
#include <kcwdebug.h>

#include "inputwriter.h"
#include "outputreader.h"

using namespace KcwSH;

DWORD kcwshInputHook(void *val) {
    KcwDebug() << "kcwshInputHook was called!";
    InputWriter writer;
    writer.init();
    return writer.exec();
}

DWORD kcwshOutputHook(void *val) {
    KcwDebug() << "kcwshOutputHook was called!";
    OutputReader reader;
    reader.init();
    return reader.exec();
}

