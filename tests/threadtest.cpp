#include <windows.h>

#include "kcwapp.h"
#include "kcwthread.h"

//class KCW_CALLBACK()
int main(int argc, char **argv) {
	KcwApp app;
	KcwThread tt;
	app.addCallback(tt.exitEvent());
	tt.start();
	return app.exec();
}