#include "mainwindow.h"
#include "crookconf.h"

#include <QApplication>
#include <QTranslator>

#define xstr(s) str(s)
#define str(s) #s

// -----------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator trans;
	trans.load(QLocale(), "krokonf", "_", xstr (INSTALL_PREFIX)"/share/krokonf/translations/");
	a.installTranslator(&trans);
	CrookConf c;
	MainWindow w(&c);
	w.show();
	return a.exec();
}
