#include "kernelselect.h"
#include "ui_kernelselect.h"

// -----------------------------------------------------------------------
KernelSelect::KernelSelect(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::KernelSelect)
{
	ui->setupUi(this);
}

// -----------------------------------------------------------------------
KernelSelect::~KernelSelect()
{
	delete ui;
}

// -----------------------------------------------------------------------
void KernelSelect::additem(QListWidgetItem *i)
{
	ui->list->addItem(i);
}

// -----------------------------------------------------------------------
int KernelSelect::get_item()
{
	return ui->list->currentRow();
}
