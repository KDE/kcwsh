#ifndef kcwapp_h
#define kcwapp_h

#include "kcweventloop.h"

/**
* @author Patrick Spendrin
* @date 2011
* @brief KcwApp is the Application class which handles the Event distribution and the event loop.
* @details KcwApp is a simple wrapper around KcwEventLoop, which has no additional functionality
* until now. For future extensions, this class should be used though.
*/
class KcwApp : public KcwEventLoop {
    public:
        /**
        * construct a default KcwApp object.
        */
        KcwApp();
};
#endif /* kcwapp_h */
