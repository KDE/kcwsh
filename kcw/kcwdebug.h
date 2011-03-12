#ifndef kcwdebug_h
#define kcwdebug_h

#include <sstream>
#include <string>
#include <locale>

class KcwDebug;

KcwDebug& endl(KcwDebug& os);

/**
* @author Patrick Spendrin
* @date 2011
* @brief C++ Stream based debug output.
* @details KcwDebug can be used to output most values understood by std::ostream (except wide characters!)
* similar to the following code snippet:
* @code
* KcwDebug() << "the value for x is" << x << endl;
* @endcode
* @note endl is different from std::endl which cannot be used here.
*/

class KcwDebug {
    public:
        /**
        * The default constructor. If the KcwDebug is enabled, output goes to the Windows debug console.
        * You can see output from within debugview (@see http://technet.microsoft.com/en-us/sysinternals/bb896647)
        */
        KcwDebug();

        /**
        * Constructs an object which redirects the output to a string in addition to the debug console.
        * If output is disabled using setEnabled(), the string will still be changed.
        */
        KcwDebug(std::string* result);

        /**
        * the destructor does all the work, to flush the output, the object must be destroyed.
        */
        ~KcwDebug();

        /**
        * template operator method that outputs everything that fits.
        * @return a reference to the object for which this method is called.
        */
        template<typename T> KcwDebug& operator<<(T i) {
            spaceIt();
            m_ss << i;
            return maybeSpaceReference();
        }
        
        template<> KcwDebug& operator<<(const wchar_t* i) {
            return operator<<(std::wstring(i));
        }
        template<> KcwDebug& operator<<(std::wstring i) {
            spaceIt();
            const wchar_t *wstr = i.c_str();
            char *str = new char[i.length() + 1];
            ZeroMemory(str, (i.length() + 1) * sizeof(char));
            std::use_facet<std::ctype<wchar_t>>(s_loc).narrow(wstr, wstr+wcslen(wstr), '?', str);
            str[wcslen(wstr)] = 0;
            m_ss << str;
            return maybeSpaceReference();
        }

        /**
        * typedef for manipulation functions giving special output similar to endl.
        */
        typedef KcwDebug& (ManipFunc)(KcwDebug &);

        /**
        * Output results of manipulation functions.
        * @return a reference to the KcwDebug object.
        */
        KcwDebug& operator<<(ManipFunc manipFunc);

        /**
        * enable or disable debug output according to @p enable.
        * This is a global property, so you only need to set it once in your program.
        */
        static void setEnabled(bool enable);

        /**
        * @return whether debug output is enabled or disabled.
        */
        static bool enabled();

    protected:
        /**
        * output a space character, given that the previous character wasn't an endl
        * to make this work, instead of returning the reference of your KcwDebug object directly
        * you should return maybeSpaceReference() in your overloaded function instead.
        */
        void spaceIt();
        /**
        * @see spaceIt()
        */
        KcwDebug& maybeSpaceReference();

    private:
        static std::locale  s_loc;
        static bool         s_enabled;
        bool                m_maybeSpace;
        std::stringstream   m_ss;
        std::string*        m_stringptr;
        friend KcwDebug& endl(KcwDebug& os);
};

#endif /* kcwdebug_h */