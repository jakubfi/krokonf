#include "mainwindow.h"
#include "crookconf.h"

#include <QApplication>
#include <QTranslator>

#ifndef INSTALL_PREFIX
#define INSTALL_PREFIX "/usr/local"
#endif

#define xstr(s) str(s)
#define str(s) #s

// -----------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator trans;
	if (!trans.load(QLocale(), "krokonf", "_", xstr (INSTALL_PREFIX)"/share/krokonf/translations/")) {
		trans.load(QLocale(), "krokonf", "_", ".");
	}
	a.installTranslator(&trans);
	CrookConf c;
	MainWindow w(&c);
	w.show();
	return a.exec();
}
