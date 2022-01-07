#ifndef KERNELSELECT_H
#define KERNELSELECT_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class KernelSelect;
}

class KernelSelect : public QDialog
{
	Q_OBJECT

public:
	explicit KernelSelect(QWidget *parent = nullptr);
	~KernelSelect();
	void additem(QListWidgetItem *i);
	int get_item();

private:
	Ui::KernelSelect *ui;
};

#endif // KERNELSELECT_H
