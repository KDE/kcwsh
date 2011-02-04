#ifndef kcwapp_h
#define kcwapp_h

#include "kcweventloop.h"

class KcwApp : public KcwEventLoop {
    /**
    * KcwApp is the Application class which handles the Event distribution and the event loop.
    */
    public:
        KcwApp();
};
#endif /* kcwapp_h */
