#include <iostream>
#include <string>

#include <windows.h>

#include "kcwdebug.h"

KcwDebug::KcwDebug()
 :  m_maybeSpace( false ),
    m_enabled( true ),
    m_stringptr( NULL )
{}

KcwDebug::KcwDebug(std::string* ptr)
 :  m_maybeSpace( false ),
    m_enabled( true ),
    m_stringptr( ptr )
{}

KcwDebug::~KcwDebug() {
    if(m_enabled) {
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

KcwDebug& KcwDebug::operator<<(KcwDebug& (manipFunc)(KcwDebug &)) {
    return manipFunc(*this);
}

void KcwDebug::setEnabled(bool enable) {
    m_enabled = enable;
}

bool KcwDebug::enabled() const {
    return m_enabled;
}

// manipulation function
KcwDebug& endl(KcwDebug& os) {
    // before endl, we don't need a space
    os.m_maybeSpace = false;
    os.m_ss << std::endl;
    return os;
}

