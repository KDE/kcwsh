#ifndef qinputreader
#define qinputreader

#include "inputreader.h"

#include <QKeyEvent>

namespace KcwSH {
namespace QtFrontend {
class QtInputReader : public InputReader {
    public:
        QtInputReader();

//        KCW_CALLBACK(ConsoleInputReader, transferData);
        void transferData(QKeyEvent* event, bool keyDown);
    private:
};
};
};
#endif /* qinputreader */