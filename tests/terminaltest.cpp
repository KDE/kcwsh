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
#include <sstream>
#include <vector>
#include <iterator>

#include <kcwprocess.h>
#include <kcwdebug.h>

#include "terminal.h"
#include "inputreader.h"

using namespace KcwSH;
using namespace std;

class TestTerminal : public Terminal {
    public:
        KCW_CALLBACK(TestTerminal, activityChanged);
        vector<wstring> m_cmd;
};

void TestTerminal::activityChanged() {
    // set size to the same size
    COORD c;
    c.X = 80; c.Y = 25;
    setTerminalSize(c);

    // send concatenated commandline as command
    wostringstream imploded;
    copy(m_cmd.begin() + 1, m_cmd.end(),
           ostream_iterator<wstring, wchar_t>(imploded, L" "));
    sendCommand(imploded.str());

    Sleep(1000);
}

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
    TestTerminal t;
    if(!shell.empty()) {
        t.setCmd(shell);
    }
//     t.setOutputWriter(new TestOutputWriter(&t));
    t.setInputReader(new InputReader);

    KcwProcess::KcwProcessEnvironment env = KcwProcess::KcwProcessEnvironment::getCurrentEnvironment();
    env[L"KCW_DEBUG"] = L"1";

    t.setEnvironment(env);
    t.m_cmd = args;

    HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);
    LARGE_INTEGER li;
    li.QuadPart = -20 * 1000000LL; // 2 seconds

    t.addCallback(timer, CB(Terminal::quit), &t);

    SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE);
    result = t.run();
    if(result != 0) wcout << L"failed to run Terminal" << endl;

    wcout << L"Finished testing Terminal" << endl;
    return 0;
}