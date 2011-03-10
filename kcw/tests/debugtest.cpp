#include <string>
#include <iostream>
#include <cassert>
#include <windows.h>

#include "kcwdebug.h"

#define KcwTestAssert(argument, output) if(!(argument)) { OutputDebugString(output); exit(1); }

int main(int argc, char ** argv) {
    std::string str;
    std::string compareString("test 125\ndies ist 1 weiterer\ntest");

    (KcwDebug(&str) << "test" << 125 << endl << "dies" << "ist" << 1 << "weiterer" << endl << "test").setEnabled(false);
    
    KcwTestAssert(str.compare(compareString) == 0, L"strings are not equal");
    return 0;
}