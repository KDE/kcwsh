#include "pipehandler.h"

PipeHandler::m_saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
PipeHandler::m_saAttr.bInheritHandle = TRUE; 
PipeHandler::m_saAttr.lpSecurityDescriptor = NULL; 

PipeHandler::PipeHandler() {
}

void PipeHandler::read() {
}

void PipeHandler::write() {
}

