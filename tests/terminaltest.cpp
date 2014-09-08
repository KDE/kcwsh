/* Copyright 2014  Patrick Spendrin <ps_ml@gmx.de>
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

#include <kcwprocess.h>

#include "terminal.h"

using namespace KcwSH;
using namespace std;


int main() {
    int argc;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    vector<wstring> args;
    for(int i = 0; i < argc; i++) args.push_back(wstring(argv[i]));

    int result = 0;

    wstring shell;

    wcout << L"Begin testing Terminal" << endl;
    if(args.size() == 2) {

    } else if(args.size() >= 3) {
        shell = args[0];
        args.erase(args.begin());
    } else {
        wcout << L"wrong number of arguments" << endl << endl;
        wcout << L"\tsyntax: " << args[0] << L" COMMAND" << endl;
        wcout << L"OR" << endl;
        wcout << L"\tsyntax: " << args[0] << L" SHELL COMMAND [ARGUMENTS]" << endl << endl;
        wcout << L"where SHELL is an executable windows console application and COMMAND can be" << endl;
        wcout << L"a command run in that shell with any number of [ARGUMENTS]." << endl;
        return -1;
    }

    /*************************************************************************/
    Terminal t;
    if(!shell.empty()) {
        t.setCmd(shell);
    }

    KcwProcess::KcwProcessEnvironment env = KcwProcess::KcwProcessEnvironment::getCurrentEnvironment();
    env[L"KCW_DEBUG"] = L"1";

    t.setEnvironment(env);

    HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);
    LARGE_INTEGER li;
    li.QuadPart = -20 * 1000000LL; // 2 seconds

    SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE);
    t.addCallback(timer, CB(Terminal::quit), &t);

    result = t.run();
    if(result != 0) wcout << L"failed to run Terminal" << endl;

    wcout << L"Finished testing Terminal" << endl;
    return 0;
}