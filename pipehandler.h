#ifndef PIPEHANDLER_H
#define PIPEHANDLER_H

class PipeHandler {
    public:
        PipeHandler();
        void read();
        void write();
    private:
        static SECURITY_ATTRIBUTES m_saAttr;
};
#endif /* PIPEHANDLER_H */
