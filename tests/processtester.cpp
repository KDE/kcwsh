#include <iostream>
#include <vector>
#include <string>

#include <windows.h>

#include <kcweventloop.h>
#include <kcwprocess.h>

using namespace std;

class ProcessTest : public KcwEventLoop {
    public:
        ProcessTest() : m_runningProcesses(0) {}
        ~ProcessTest() {
            while(!m_processes.empty()) {
                delete m_processes.back();
                m_processes.pop_back();
            }
        }
        void case1(const wstring& executable) {
            KcwProcess *p1 = new KcwProcess;
            p1->setCmd(executable + L" " + L" stop");
            p1->setIsStartedAsPaused(true); p1->setStartupAsHidden();
            p1->start();
            addCallback(p1->process(), CB(processFinished));
            p1->resume();
            m_runningProcesses++;
            m_processes.push_back(p1);
            Sleep(1000);
            KcwProcess *p2 = new KcwProcess;
            p2->setCmd(executable + L" " + L" fork");
            p2->setIsStartedAsPaused(true); p2->setStartupAsHidden();
            p2->start();
            addCallback(p2->process(), CB(processFinished));
            p2->resume();
            m_runningProcesses++;
            m_processes.push_back(p2);
        }

        void case2fork(const wstring& executable) {
            KcwProcess *p1 = new KcwProcess;
            p1->setCmd(executable + L" " + L" stop");
            p1->setIsStartedAsPaused(true); p1->setStartupAsHidden();
            p1->start();
            addCallback(p1->process(), CB(processFinished));
            p1->resume();
            m_runningProcesses++;
            m_processes.push_back(p1);
            Sleep(1000);
            KcwProcess *p2 = new KcwProcess;
            p2->setCmd(executable + L" " + L" stop");
            p2->setIsStartedAsPaused(true); p2->setStartupAsHidden();
            p2->start();
            addCallback(p2->process(), CB(processFinished));
            p2->resume();
            m_runningProcesses++;
            m_processes.push_back(p2);
        }

        KCW_CALLBACK(ProcessTest, processFinished);

    vector<KcwProcess*> m_processes;
    int m_runningProcesses;
};

void ProcessTest::processFinished() {
    if(!(m_runningProcesses--)) quit();
}

int main() {
    int argc;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    vector<wstring> args;
    for(int i = 0; i < argc; i++) args.push_back(wstring(argv[i]));
    wcout << "another process called" << endl;
    ProcessTest l;
    switch(argc) {
        case 1: l.case1(args[0]); break;
        case 2: { 
            if(args[1] == L"stop") {
                Sleep(5000);
                return 0;
            } else if(args[1] == L"fork") {
                l.case2fork(args[0]); break;
                return 0;
            } else {
                return 0;
            }
            break;
        }
        default: {
            wcout << L"test executable for kcwsh, plz read source code" << endl;
            return -1;
        }
    };
    l.exec();
    Sleep(1000);
    return 0;
}