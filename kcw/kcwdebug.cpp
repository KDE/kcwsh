#include <iostream>
#include <string>

#include <windows.h>

#include "kcwdebug.h"

bool KcwDebug::s_enabled = true;

KcwDebug::KcwDebug()
 :  m_maybeSpace( false ),
    m_stringptr( NULL )
{}

KcwDebug::KcwDebug(std::string* ptr)
 :  m_maybeSpace( false ),
    m_stringptr( ptr )
{}

KcwDebug::~KcwDebug() {
    if(s_enabled) {
//        std::cout << m_ss.str();
        OutputDebugStringA(m_ss.str().c_str());
    }
    if(m_stringptr) *m_stringptr = m_ss.str();
}

KcwDebug& KcwDebug::maybeSpaceReference() {
    m_maybeSpace = true;
    return *this;
}

void KcwDebug::spaceIt() {
    if(m_maybeSpace) m_ss << " ";
    m_maybeSpace = false;
}

KcwDebug& KcwDebug::operator<<(ManipFunc manipFunc) {
    return manipFunc(*this);
}

void KcwDebug::setEnabled(bool enable) {
    s_enabled = enable;
}

bool KcwDebug::enabled() {
    return s_enabled;
}

// manipulation function
/**
* writes an end of line character (std::endl) either to the debug console or the string
* used by the KcwDebug object.
*/
KcwDebug& endl(KcwDebug& os) {
    // before endl, we don't need a space
    os.m_maybeSpace = false;
    os.m_ss << std::endl;
    return os;
}

