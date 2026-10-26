#include "mainwindow.h"
#include "crookconf.h"

#include <QApplication>
#include <QTranslator>

// -----------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator trans;
	if (trans.load(QLocale(), "krokonf", "_", TRANSLATIONS_DIR)) {
		a.installTranslator(&trans);
	}
	CrookConf c;
	MainWindow w(&c);
	w.show();
	if (a.arguments().size() > 1) {
		w.open_file(a.arguments().at(1));
	}
	return a.exec();
}
