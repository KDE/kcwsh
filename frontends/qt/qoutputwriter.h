#ifndef qoutputwriter
#define qoutputwriter

#include "outputwriter.h"

#include <QString>

namespace KcwSH {
namespace QtFrontend {
class QtOutputWriter : public OutputWriter {
    public:
        QtOutputWriter();
        QString getBufferText();
    private:
};
};
};
#endif /* qoutputwriter */