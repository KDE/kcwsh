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

#define KcwTestAssert(argument, output) if(!(argument)) { wcout << output << endl; exit(1); }

class TestTerminal : public Terminal {
    public:
        TestTerminal();

        KCW_CALLBACK(TestTerminal, activityChanged);
        KCW_CALLBACK(TestTerminal, titleChanged);
        KCW_CALLBACK(TestTerminal, sizeChanged);
        KCW_CALLBACK(TestTerminal, startCmd);
        KCW_CALLBACK(TestTerminal, startResize);

        void startCmdDelayed(vector<wstring> cmd, int delay = 1000);
        void startResizeTest();

        vector<wstring> m_cmd;
        wstring m_testcase;
        wstring m_titleStore;
        COORD m_testSize;
        HANDLE m_timer;
};

TestTerminal::TestTerminal() : Terminal(), m_timer(nullptr) {
}

void TestTerminal::startCmdDelayed(vector<wstring> cmd, int delay) {
    m_cmd = cmd;
    if(m_timer != nullptr) {
        // FIXME: do something here
    }
    m_timer = CreateWaitableTimer(NULL, FALSE, NULL);
    addCallback(m_timer, CB(startCmd));

    // start the timer for delayed cmd startup.
    LARGE_INTEGER li;
    li.QuadPart = -1 * delay * 10000LL; // 1 second

    SetWaitableTimer(m_timer, &li, 0, NULL, NULL, FALSE);
}

void TestTerminal::startCmd() {
    // send concatenated commandline as command
    wostringstream imploded;
    copy(m_cmd.begin(), m_cmd.end(),
           ostream_iterator<wstring, wchar_t>(imploded, L" "));
    sendCommand(imploded.str());
}

void TestTerminal::startResize() {
    setTerminalSize(m_testSize);
}

void TestTerminal::activityChanged() {
    // set size to the same size
    COORD c;
    c.X = 80; c.Y = 25;
    setTerminalSize(c);
    m_titleStore = title();
}

void TestTerminal::titleChanged() {
    static int counter = 0;
    counter++;
    switch(counter) {
        case 1: {
                    KcwTestAssert(title() != m_titleStore, L"title isn't changed after startup");
                    m_titleStore = title();
                    break;
        }
        case 2: break;
        case 3: {
                    KcwTestAssert(title() == L"New Title here", L"title isn't the one from the executable");
                    break;
        }
        case 4: {
                    KcwTestAssert(title() == m_titleStore, L"title isn't set back to original title");
                    quit();
                    break;
        }
        default: KcwTestAssert(false, L"error: unreachable point reached"); break;
    };
}

bool operator == (COORD a, COORD b) {
    return a.X == b.X && a.Y == b.Y;
}

bool operator != (COORD a, COORD b) {
    return a.X != b.X || a.Y != b.Y;
}

void TestTerminal::sizeChanged() {
    if(m_testcase != L"resize") return;

    static int counter = 0;
    counter++;
    COORD c;
    switch(counter) {
        case 1: {
                    c = { 80, 25 };
                    KcwTestAssert(terminalSize() == c, L"terminal didn't reach default size after startup");
                    m_testSize = { 162, 52 };
                    break;
        }
        case 2: {
                    KcwTestAssert(terminalSize() == m_testSize, L"terminal didn't reach previously set size");
                    m_testSize = { 160, 50 };
                    break;
        }
        case 3: {
                    KcwTestAssert(terminalSize() == m_testSize, L"terminal didn't reach previously set size");
                    m_testSize = { 10000, 10000 }; // we want to restrict to the maximum size now
                    break;
        }
        case 4: {
                    KcwTestAssert(terminalSize() != m_testSize, L"terminal should be restricted to maximum size");
                    quit();
                    break;
        }
        default: KcwTestAssert(false, L"error: unreachable point reached"); break;
    };
    wcout << L"leaving sizeChanged " << counter << endl;
}

void TestTerminal::startResizeTest() {
    HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);
    addCallback(timer, CB(startResize));

    // start the timer for delayed resize.
    LARGE_INTEGER li;
    li.QuadPart = -1000 * 10000LL; // 1 second

    SetWaitableTimer(timer, &li, 1000, NULL, NULL, FALSE);
}

int main() {
    int argc;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    vector<wstring> args;
    for(int i = 0; i < argc; i++) args.push_back(wstring(argv[i]));

    int result = 0;

    wstring shell;
    wstring testcase;

    wcout << L"Begin testing Terminal" << endl;
    args.erase(args.begin());
    if(args.size() == 2) {
        testcase = args[0];
        args.erase(args.begin());
        if(testcase == L"shell") {
            shell = args[0];
            args.erase(args.begin());
        }
    } else {
        wcout << L"wrong number of arguments" << endl << endl;
        wcout << L"\tsyntax: " << args[0] << L" testcase TESTEREXECUTABLE" << endl;
        wcout << L"OR" << endl;
        wcout << L"\tsyntax: " << args[0] << L" shell SHELL COMMAND [ARGUMENTS]" << endl << endl;
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
    t.m_testcase = testcase;
    if(testcase == L"shell" || testcase == L"title") {
        t.startCmdDelayed(args);
    } else if(testcase == L"resize") {
        t.m_testSize = { 80, 25 };
        t.startResizeTest();
    }

    HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);
    LARGE_INTEGER li;
    li.QuadPart = -200 * 1000000LL; // 20 seconds

    t.addCallback(timer, CB(Terminal::quit), &t);

    SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE);
    result = t.run();
    if(result != 0) wcout << L"failed to run Terminal" << endl;

    wcout << L"Finished testing Terminal" << endl;
    return 0;
}