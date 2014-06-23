#include <iostream>
#include <string>
#include <vector>

#include <kcwprocess.h>

#include "terminal.h"

using namespace KcwSH;
using namespace std;


int wmain(int argc, wchar_t** argv) {
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