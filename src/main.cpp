#include "mainwindow.h"
#include "crookconf.h"

#include <QApplication>
#include <QTranslator>

// -----------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator trans;
	trans.load(QLocale(), "krokonf", "_", TRANSLATIONS_DIR);
	a.installTranslator(&trans);
	CrookConf c;
	MainWindow w(&c);
	w.show();
	return a.exec();
}
