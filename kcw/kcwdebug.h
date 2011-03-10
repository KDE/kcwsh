#ifndef kcwdebug_h
#define kcwdebug_h

#include <sstream>
#include <string>

class KcwDebug;

KcwDebug& endl(KcwDebug& os);

class KcwDebug {
    public:
        KcwDebug();
        KcwDebug(std::string* result);
        ~KcwDebug();

        KcwDebug& maybeSpaceReference();
        template<typename T> KcwDebug& operator<<(T i) {
            spaceIt();
            m_ss << i;
            return maybeSpaceReference();
        }
        KcwDebug& operator<<(KcwDebug& (manipFunc)(KcwDebug &));
        
        void setEnabled(bool enable);
        bool enabled() const;
        void spaceIt();
    private:
        bool                m_enabled;
        bool                m_maybeSpace;
        std::stringstream   m_ss;
        std::string*        m_stringptr;
        friend KcwDebug& endl(KcwDebug& os);
};

#endif /* kcwdebug_h */