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
#include <windows.h>

#include <kcwdebug.h>
#include <kcwnotifier.h>
#include <string>

HINSTANCE s_module = 0;
KcwNotifier s_notifier;

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    s_module = hInstance;
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            std::wstringstream wss;
            wss << L"kcwsh-exitEvent-" << GetCurrentProcessId();
            s_notifier.open(wss.str());
            break;
        }
        case DLL_PROCESS_DETACH:
        {
//             KcwDebug() << "detaching from process!" << ::GetCurrentProcessId();
            s_notifier.notify();
            break;
        }
        case DLL_THREAD_ATTACH:
        {
//             KcwDebug() << "attaching thread!";
            break;
        }
        case DLL_THREAD_DETACH:
        {
//             KcwDebug() << "detaching thread!";
            break;
        }
    };
    return TRUE;
}
