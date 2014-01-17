/* Copyright 2013-2014  Patrick Spendrin <ps_ml@gmx.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <iostream>
#include <string>
#include <vector>

#include <windows.h>
#include <Shlobj.h>

#include <kcwapp.h>
#include <kcwdebug.h>

#include "consoleterminal.h"

#define BUFSIZE 4096

static bool g_debug = false;

using namespace KcwSH;

void usage() {
    std::cout << "kcwsh is a wrapper Shell for cmd.exe which understands ansi " << std::endl \
         << "escape sequences for some functionality." << std::endl \
         << std::endl << "options:" << std::endl \
         << "-h, --help" << "\tDisplays this help message." << std::endl \
         << "-d, --debug" << "\tswitches on debug mode" << std::endl \
         << std::endl << "Copyright (C) by Patrick Spendrin 2011" << std::endl \
         << "Redistributable under GPLv3" << std::endl;
}

std::wstring getDefaultCmdInterpreter() {
    WCHAR szPath[MAX_PATH];

    if(SUCCEEDED(SHGetFolderPathW(NULL,
                                 CSIDL_SYSTEM|CSIDL_FLAG_CREATE,
                                 NULL,
                                 0,
                                 szPath)))
    {
        std::wstring ret(szPath);
        return ret;
    }
    return std::wstring();
}

int main(int argc, char **argv) {
    std::vector<std::string> args(argv, argv + argc);
    KcwApp app;

    std::cout << "kcwsh - Windows wrapper shell" << std::endl;
    for(std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); it++) {
        if(*it == "-h" || *it == "--help") {
            usage();
            return 1;
        }

        if(*it == "-d" || *it == "--debug") {
            g_debug = true;
        }
    }

    if(g_debug) std::cout << "enabled debug mode!" << std::endl;
    ConsoleTerminal term;
    term.setCmd(getDefaultCmdInterpreter() + L"\\cmd.exe");
    term.start();
    app.addCallback(term.exitEvent());
    app.exec();
    KcwDebug() << "kcwsh quit";
    std::cout << "kcwsh quit" << std::endl;
    return 0;
}
