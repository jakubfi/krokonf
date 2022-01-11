#include <QFileDialog>
#include <QtDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileInfo>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "crookconf.h"
#include "kernelselect.h"

// -----------------------------------------------------------------------
MainWindow::MainWindow(CrookConf *conf, QWidget *parent)
	:QMainWindow(parent),
	ui(new Ui::MainWindow),
	cfg(conf)
{
	ui->setupUi(this);

	// switch to tab 0 on start
	ui->tabs->setCurrentIndex(0);

	// initialize widgets from empty configuration
	update_tab_system_from_config();
	update_tab_memory_from_config();
	update_tab_io_from_config();
	update_tab_disks_from_config();
	update_tab_tapes_from_config();
	update_tab_multix_from_config();
	update_tab_lines_from_config();

	// initially disable all tab contents
	for(auto *widget : ui->tabs->findChildren<QWidget *>())
	{
		for(auto *c : widget->findChildren<QWidget *>()) {
			c->setEnabled(false);
		}
	}

	// hack to easily access the overloaded signal in connect()s
	void (QSpinBox::* QSpinBoxValueChangedInt)(int) = &QSpinBox::valueChanged;
	void (QComboBox::* QComboBoxActivatedInt)(int) = &QComboBox::activated;

	// menu actions
	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::load);
	connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
	connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);

	// system tab updates
	connect(ui->sys_sem, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.sem = i; });
	connect(ui->sys_dw3, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.dw3 = i; });
	connect(ui->sys_lod, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.lod_unl = i; });
	connect(ui->sys_time10x, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.tick1x = !i; });
	connect(ui->sys_noswap, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.no_swap = i; });
	connect(ui->sys_use4kbuf, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.buf_4k = i; });
	connect(ui->sys_resident, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.all_resident = i; });
	connect(ui->sys_diskwait, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.disk_wait = i; });
	connect(ui->sys_ramfiles, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.auto_ram_files = i; });
	connect(ui->sys_dirlock, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.dir_guard = i; });
	connect(ui->sys_dircheck, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.dir_write_check = i; });
	connect(ui->sys_dircheck, &QCheckBox::clicked, [=](int i){ cfg->cfg.sys.dir_write_check = i; });
	connect(ui->sys_buf, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.sys.io_buf_count = i; }); // fired on any change!
	connect(ui->sys_dirvec, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.sys.dir_vec_count = i; }); // fired on any change!
	connect(ui->sys_block_num, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.sys.os_blocks = i; }); // fired on any change!
	connect(ui->sys_line_buf_len, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.sys.line_buf_len = i; }); // fired on any change!

	// memory tab updates
	// implicit on_mem_list_currentRowChanged(int row);
	// implicit on_mem_blocks_valueChanged(int blocks);
	connect(ui->mem_type, QComboBoxActivatedInt, [=](int i){ cfg->cfg.mem[ui->mem_list->currentRow()].mega = i; });
	connect(ui->mem_init, &QCheckBox::clicked, [=](int i){ cfg->cfg.mem[ui->mem_list->currentRow()].init_required = i; });
	connect(ui->mem_module_start, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.mem[ui->mem_list->currentRow()].module_start = i; }); // fired on any change!
	connect(ui->mem_block_start, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.mem[ui->mem_list->currentRow()].block_start = i; }); // fired on any change!

	// io tab updates
	connect(ui->chan_camac1, QComboBoxActivatedInt, [=](int i){ cfg->cfg.ch_camac[0].chan = i; });
	connect(ui->chan_camac2, QComboBoxActivatedInt, [=](int i){ cfg->cfg.ch_camac[1].chan = i; });
	connect(ui->chan_camac3, QComboBoxActivatedInt, [=](int i){ cfg->cfg.ch_camac[2].chan = i; });
	connect(ui->chan_camac4, QComboBoxActivatedInt, [=](int i){ cfg->cfg.ch_camac[3].chan = i; });
	connect(ui->chan_plix, QComboBoxActivatedInt, [=](int i){ cfg->cfg.ch_plix.chan = i; });
	connect(ui->chan_multix, QComboBoxActivatedInt, [=](int i){ cfg->cfg.ch_multix.chan = i; });
	connect(ui->chan_char1, QComboBoxActivatedInt, [=](int i){ cfg->cfg.ch_char[0].chan = i; });
	connect(ui->chan_char2, QComboBoxActivatedInt, [=](int i){ cfg->cfg.ch_char[1].chan = i; });
	connect(ui->net_chan, QComboBoxActivatedInt, [=](int i){ ui->net_dev->setDisabled(!i); cfg->cfg.net.configured = i; if (i != 0) cfg->cfg.net.chan = i - 1; });
	connect(ui->net_dev, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.net.dev = i; }); // fired on any change!
	connect(ui->rtc_chan, QComboBoxActivatedInt, [=](int i){ ui->rtc_dev->setDisabled(!i); cfg->cfg.rtc.configured = i; if (i != 0) cfg->cfg.rtc.chan = i - 1; });
	connect(ui->rtc_dev, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.rtc.dev = i; }); // fired on any change!
	connect(ui->mon_group_on, &QCheckBox::clicked, [=](int i){ ui->mon_group->setDisabled(!i); cfg->cfg.mongroup.configured = i; });
	connect(ui->mon_group, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.mongroup.dev = i; }); // fired on any change!
	connect(ui->multix_reset, &QCheckBox::clicked, [=](int i){ ui->sysmon->setDisabled(i); cfg->cfg.oprq.multix_reset = i; });
	connect(ui->sysmon, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.oprq.ini_line = i; }); // fired on any change!
	connect(ui->winch_line, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.winch_line = i; }); // fired on any change!
	connect(ui->flop_line, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.floppy_line = i; }); // fired on any change!

	// disks tab updates
	// implicit on_disk_list_currentRowChanged(int row);
	// implicit on_disk_type_currentIndexChanged(int index);
	connect(ui->ch_chan, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.m9425.chan = i; }); // fired on any change!
	connect(ui->ch_unit, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.m9425.dev = i; }); // fired on any change!
	connect(ui->ch_plate, QComboBoxActivatedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.m9425.fixed = i; }); // fired on any change!
	connect(ui->winch_quant, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.winch.start_quant = i; }); // fired on any change!
	connect(ui->winch_type, QComboBoxActivatedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.winch.type = i; }); // fired on any change!
	connect(ui->winch_no, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.winch.num = i; }); // fired on any change!
	connect(ui->chflop_no, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.flop8.dev = i; }); // fired on any change!
	connect(ui->chflop_door, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.flop8.door = i; }); // fired on any change!
	connect(ui->mflop_no, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.flop5.dev = i; }); // fired on any change!
	connect(ui->mflop_type, QComboBoxActivatedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.flop5.density = i; }); // fired on any change!
	connect(ui->mdisk_type, QComboBoxActivatedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.plix.type = i; }); // fired on any change!
	connect(ui->mdisk_plix_no, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.plix.controller = i; }); // fired on any change!
	connect(ui->mdisk_dev_no, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.disk[ui->disk_list->currentRow()].d.plix.dev = i; }); // fired on any change!

	// tape drive tab updates
	connect(ui->tape0_chan, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.tape[0].raw = i; }); // fired on any change!
	connect(ui->tape1_chan, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.tape[1].raw = i; }); // fired on any change!
	connect(ui->tape2_chan, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.tape[2].raw = i; }); // fired on any change!
	connect(ui->tape3_chan, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.tape[3].raw = i; }); // fired on any change!

	// multix tab updates
	connect(ui->mx_automultix, &QCheckBox::clicked, [=](int i){ cfg->cfg.mx.auto_multix = i; });
	connect(ui->mx_no_parity, &QCheckBox::clicked, [=](int i){ cfg->cfg.mx.no_parity = i; });
	connect(ui->mx_odd_parity, &QCheckBox::clicked, [=](int i){ cfg->cfg.mx.odd_parity = i; });
	connect(ui->mx_bits8, &QCheckBox::clicked, [=](int i){ cfg->cfg.mx.bits8 = i; });
	connect(ui->mx_xonxoff, &QCheckBox::clicked, [=](int i){ cfg->cfg.mx.xon_xoff = i; });
	connect(ui->mx_bscan, &QCheckBox::clicked, [=](int i){ cfg->cfg.mx.bs_can = i; });
	connect(ui->mx_uppercase, &QCheckBox::clicked, [=](int i){ cfg->cfg.mx.uppercase = i; });
	connect(ui->mx_strvectors, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.mx.stream_vectors = i; }); // fired on any change!
	connect(ui->winch_line, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.winch_line = i; }); // fired on any change!
	connect(ui->flop_line, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.floppy_line = i; }); // fired on any change!

	// lines tab updates
	connect(ui->line_protocol, QComboBoxActivatedInt, [=](int i){ cfg->cfg.lines[ui->line_list->currentRow()].d.mx.protocol = i; }); // fired on any change!
	connect(ui->line_used, &QCheckBox::clicked, [=](int i){ cfg->cfg.lines[ui->line_list->currentRow()].d.mx.used = i; }); // fired on any change!
	connect(ui->line_dir, QComboBoxActivatedInt, [=](int i){ cfg->cfg.lines[ui->line_list->currentRow()].d.mx.dir = ui2dir(i); }); // fired on any change!
	connect(ui->line_type, QComboBoxActivatedInt, [=](int i){ cfg->cfg.lines[ui->line_list->currentRow()].d.mx.type = i; }); // fired on any change!
	connect(ui->line_dev, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.lines[ui->line_list->currentRow()].d.ch.dev = i; set_line_list_item_names(); }); // fired on any change!

	// winchester types tab updates
	connect(ui->wtype_big, QComboBoxActivatedInt, [=](int i){ cfg->cfg.winch_type[ui->wtype_list->currentRow()].big = i; }); // fired on any change!
	connect(ui->wtype_heads, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.winch_type[ui->wtype_list->currentRow()].heads = i; }); // fired on any change!
	connect(ui->wtype_autopark, &QCheckBox::clicked, [=](int i){ ui->wtype_park_cyl->setDisabled(i); cfg->cfg.winch_type[ui->wtype_list->currentRow()].autopark = i; }); // fired on any change!
	connect(ui->wtype_park_cyl, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.winch_type[ui->wtype_list->currentRow()].park_cyl = i; }); // fired on any change!
	connect(ui->winch_quant_div, QSpinBoxValueChangedInt, [=](int i){ cfg->cfg.winch_quant = i; }); // fired on any change!
}

// -----------------------------------------------------------------------
int MainWindow::dir2ui(int d)
{
	return d < 7 ? (d/2) - 1 : 3;
}

// -----------------------------------------------------------------------
int MainWindow::ui2dir(int d)
{
	return d < 3 ? (d+1) * 2 : 7;
}

// -----------------------------------------------------------------------
MainWindow::~MainWindow()
{
	delete ui;
}

// -----------------------------------------------------------------------
void MainWindow::set_disk_list_item_name(int i, int type, bool foreign)
{
	QString name;
	if (type == 0) {
		name = QString(tr("Brak konfiguracji"));
	} else {
		name = QString(tr("%1 (%2)")).arg(crk5_cfg_disk_type_name(type-1)).arg(foreign ? tr("obcy") : tr("własny"));
	}
	ui->disk_list->item(i)->setText(name);
}

// -----------------------------------------------------------------------
void MainWindow::refresh_disk_widget_contents(int row)
{
	if (row < 0) return;

	struct crk5_cfg_disk *disk = cfg->cfg.disk + row;

	switch (disk->type) {
	case CRK5_CFG_DISK_9425:
		ui->ch_chan->setValue(disk->d.m9425.chan);
		ui->ch_unit->setValue(disk->d.m9425.dev);
		ui->ch_plate->setCurrentIndex(disk->d.m9425.fixed);
		break;
	case CRK5_CFG_DISK_FLOP5:
		ui->mflop_type->setCurrentIndex(disk->d.flop5.density);
		ui->mflop_no->setValue(disk->d.flop5.dev);
		break;
	case CRK5_CFG_DISK_FLOP8:
		ui->chflop_no->setValue(disk->d.flop8.dev);
		ui->chflop_door->setValue(disk->d.flop8.door);
		break;
	case CRK5_CFG_DISK_PLIX:
		ui->mdisk_type->setCurrentIndex(disk->d.plix.type);
		ui->mdisk_plix_no->setValue(disk->d.plix.controller);
		ui->mdisk_dev_no->setValue(disk->d.plix.dev);
		break;
	case CRK5_CFG_DISK_WINCH:
		ui->winch_quant->setValue(disk->d.winch.start_quant);
		ui->winch_type->setCurrentIndex(disk->d.winch.type);
		ui->winch_no->setValue(disk->d.winch.num);
		break;
	}
}

// -----------------------------------------------------------------------
void MainWindow::on_disk_type_currentIndexChanged(int type)
{
	int row = ui->disk_list->currentRow();
	struct crk5_cfg_disk *disk = cfg->cfg.disk + row;

	disk->configured = type ? true : false;
	disk->type = type - 1;
	ui->disk_origin->setDisabled(!type);
	ui->disk_stack->setCurrentIndex(type);
	set_disk_list_item_name(row, type, ui->disk_origin->currentIndex());
	refresh_disk_widget_contents(row);
}

// -----------------------------------------------------------------------
void MainWindow::on_disk_origin_currentIndexChanged(int origin)
{
	int row = ui->disk_list->currentRow();
	cfg->cfg.disk[row].foreign = origin;
	set_disk_list_item_name(row, ui->disk_type->currentIndex(), origin);
}

// -----------------------------------------------------------------------
void MainWindow::on_disk_list_currentRowChanged(int row)
{
	if (row < 0) return;

	struct crk5_cfg_disk *disk = cfg->cfg.disk + row;

	if (cfg->cfg.disk[row].configured) {
		ui->disk_type->setCurrentIndex(disk->type + 1);
		ui->disk_stack->setCurrentIndex(disk->type + 1);
		ui->disk_origin->setDisabled(false);
		ui->disk_origin->setCurrentIndex(disk->foreign);
	} else {
		ui->disk_type->setCurrentIndex(0);
		ui->disk_stack->setCurrentIndex(0);
		ui->disk_origin->setDisabled(true);
		ui->disk_origin->setCurrentIndex(0);
	}
	refresh_disk_widget_contents(row);
}

// -----------------------------------------------------------------------
void MainWindow::set_memory_widgets_state(bool state)
{
	ui->mem_block_start->setDisabled(!state);
	ui->mem_init->setDisabled(!state);
	ui->mem_module_start->setDisabled(!state);
	ui->mem_type->setDisabled(!state);
}

// -----------------------------------------------------------------------
void MainWindow::on_mem_list_currentRowChanged(int row)
{
	if (row < 0) return;

	struct crk5_cfg_mem *mem = cfg->cfg.mem + row;

	if (cfg->cfg.mem[row].configured) {
		ui->mem_block_start->setValue(mem->block_start);
		ui->mem_blocks->setValue(mem->block_count);
		ui->mem_init->setChecked(mem->init_required);
		ui->mem_module_start->setValue(mem->module_start);
		ui->mem_type->setCurrentIndex(mem->mega ? 1 : 0);
	} else {
		ui->mem_block_start->setValue(0);
		ui->mem_blocks->setValue(0);
		ui->mem_init->setChecked(false);
		ui->mem_module_start->setValue(0);
		ui->mem_type->setCurrentIndex(0);
	}
	set_memory_widgets_state(mem->configured);
}

// -----------------------------------------------------------------------
void MainWindow::set_memory_list_item_name(int i, int blocks)
{
	QString name;
	QString bl;
	if (blocks == 1) bl = QString("blok");
	else if ((blocks >= 12) && (blocks <= 14)) bl = QString(tr("bloków"));
	else if ((blocks % 10 >= 2) && (blocks % 10 <= 4)) bl = QString(tr("bloki"));
	else bl = QString(tr("bloków"));

	if (!blocks) {
		name = tr("Brak konfiguracji");
	} else {
		name = QString(tr("%1 K (%2 %3)")).arg(blocks * 4).arg(blocks).arg(bl);
	}
	ui->mem_list->item(i)->setText(name);
}

// -----------------------------------------------------------------------
void MainWindow::on_mem_blocks_valueChanged(int blocks)
{
	int row = ui->mem_list->currentRow();
	// set configuration
	cfg->cfg.mem[row].configured = blocks ? true : false;
	cfg->cfg.mem[row].block_count = blocks;
	// enable/disable other memory widgets
	set_memory_widgets_state(blocks ? true : false);
	// update item name in the memory list
	set_memory_list_item_name(row, blocks);
}

// -----------------------------------------------------------------------
void MainWindow::set_line_widgets_state(int chan_widget_index)
{
	ui->line_stack->setCurrentIndex(chan_widget_index);
}

// -----------------------------------------------------------------------
void MainWindow::on_line_count_valueChanged(int lines)
{
	int row = ui->line_list->currentRow();
	cfg->cfg.lines[row].d.mx.count = lines;
	set_line_list_item_names();
}

// -----------------------------------------------------------------------
void MainWindow::on_line_chan_currentIndexChanged(int index)
{
	set_line_widgets_state(index);

	int row = ui->line_list->currentRow();
	struct crk5_cfg_lines *lines = cfg->cfg.lines + row;

	switch (index) {
	case 0:
		lines->configured = false;
		break;
	case 1:
		lines->configured = true;
		lines->multix = false;
		lines->d.ch.dev = ui->line_dev->value();
		break;
	case 2:
		lines->configured = true;
		lines->multix = true;
		lines->d.mx.count = ui->line_count->value();
		lines->d.mx.dir = ui2dir(ui->line_dir->currentIndex());
		lines->d.mx.protocol = ui->line_protocol->currentIndex();
		lines->d.mx.type = ui->line_type->currentIndex();
		lines->d.mx.used = ui->line_used->isChecked();
		break;
	}
	set_line_list_item_names();
}

// -----------------------------------------------------------------------
void MainWindow::on_line_list_currentRowChanged(int row)
{
	if (row < 0) return;

	struct crk5_cfg_lines *lines = cfg->cfg.lines + row;

	int line_chan;
	if (lines->configured) {
		if (lines->multix) line_chan = 2;
		else line_chan = 1;
	} else {
		line_chan = 0;
	}

	// update widget contents
	if (lines->configured) {
		ui->line_used->setChecked(lines->d.mx.used);
		ui->line_dir->setCurrentIndex(dir2ui(lines->d.mx.dir));
		ui->line_type->setCurrentIndex(lines->d.mx.type);
		ui->line_protocol->setCurrentIndex(lines->d.mx.protocol);
		ui->line_count->setValue(lines->d.mx.count);
		ui->line_dev->setValue(lines->d.ch.dev);
	} else {
		ui->line_used->setChecked(false);
		ui->line_dir->setCurrentIndex(0);
		ui->line_type->setCurrentIndex(0);
		ui->line_protocol->setCurrentIndex(0);
		ui->line_count->setValue(0);
		ui->line_dev->setValue(0);
	}

	// set line chan widget according to line channel
	ui->line_chan->setCurrentIndex(line_chan);
	// enable/disable widgets accordig to chan selection
	set_line_widgets_state(line_chan);

}

// -----------------------------------------------------------------------
void MainWindow::on_wtype_list_currentRowChanged(int row)
{
	if (row < 0) return;

	struct crk5_cfg_winch_type *wtype = cfg->cfg.winch_type + row;
	ui->wtype_big->setCurrentIndex(wtype->big);
	ui->wtype_heads->setValue(wtype->heads);
	ui->wtype_autopark->setChecked(wtype->autopark);
	ui->wtype_park_cyl->setValue(wtype->park_cyl);
	ui->wtype_park_cyl->setDisabled(wtype->autopark);
	ui->winch_quant_div->setValue(cfg->cfg.winch_quant);
}

// -----------------------------------------------------------------------
void MainWindow::update_tab_system_from_config()
{
	struct crk5_cfg_sys *sys = &cfg->cfg.sys;
	ui->sys_sem->setChecked(sys->sem);
	ui->sys_dw3->setChecked(sys->dw3);
	ui->sys_lod->setChecked(sys->lod_unl);
	ui->sys_time10x->setChecked(!sys->tick1x);
	ui->sys_noswap->setChecked(sys->no_swap);
	ui->sys_use4kbuf->setChecked(sys->buf_4k);
	ui->sys_resident->setChecked(sys->all_resident);
	ui->sys_diskwait->setChecked(sys->disk_wait);
	ui->sys_buf->setValue(sys->io_buf_count);
	ui->sys_ramfiles->setChecked(sys->auto_ram_files);
	ui->sys_dirlock->setChecked(sys->dir_guard);
	ui->sys_dircheck->setChecked(sys->dir_write_check);
	ui->sys_dirvec->setValue(sys->dir_vec_count);
	ui->sys_block_num->setValue(sys->os_blocks);
	ui->sys_line_buf_len->setValue(sys->line_buf_len);
}

// -----------------------------------------------------------------------
void MainWindow::update_tab_memory_from_config()
{
	ui->mem_list->setCurrentRow(0);
	on_mem_list_currentRowChanged(0); // explicit, because row doesn't change and signal does not fire
	for (int i=0 ; i<CRK5_CFG_MEM_SLOTS ; i++) {
		set_memory_list_item_name(i, cfg->cfg.mem[i].block_count);
	}
}

// -----------------------------------------------------------------------
void MainWindow::update_tab_io_from_config()
{
	struct crk5_cfg_chandev *camac = cfg->cfg.ch_camac;
	struct crk5_cfg_chandev *plix = &cfg->cfg.ch_plix;
	struct crk5_cfg_chandev *multix = &cfg->cfg.ch_multix;
	struct crk5_cfg_chandev *ch = cfg->cfg.ch_char;
	struct crk5_cfg_chandev *net = &cfg->cfg.net;
	struct crk5_cfg_chandev *rtc = &cfg->cfg.rtc;
	struct crk5_cfg_mongroup *mongroup = &cfg->cfg.mongroup;
	struct crk5_cfg_oprq *oprq = &cfg->cfg.oprq;

	ui->chan_camac1->setCurrentIndex(camac[0].configured ? camac[0].chan + 1 : 0);
	ui->chan_camac2->setCurrentIndex(camac[1].configured ? camac[1].chan + 1 : 0);
	ui->chan_camac3->setCurrentIndex(camac[2].configured ? camac[2].chan + 1 : 0);
	ui->chan_camac4->setCurrentIndex(camac[3].configured ? camac[3].chan + 1 : 0);
	ui->chan_plix->setCurrentIndex(plix->configured ? plix->chan + 1 : 0);
	ui->chan_multix->setCurrentIndex(multix->configured ? multix->chan + 1 : 0);
	ui->chan_char1->setCurrentIndex(ch[0].configured ? ch[0].chan + 1 : 0);
	ui->chan_char2->setCurrentIndex(ch[1].configured ? ch[1].chan + 1 : 0);

	ui->net_dev->setDisabled(!net->configured);
	ui->net_chan->setCurrentIndex(net->configured ? net->chan + 1 : 0);
	ui->net_dev->setValue(net->configured ? net->dev : 0);

	ui->rtc_dev->setDisabled(!rtc->configured);
	ui->rtc_chan->setCurrentIndex(rtc->configured ? rtc->chan + 1 : 0);
	ui->rtc_dev->setValue(rtc->configured ? rtc->dev : 0);

	ui->mon_group->setDisabled(!mongroup->configured);
	ui->mon_group_on->setChecked(mongroup->configured);
	ui->mon_group->setValue(mongroup->configured ? mongroup->dev : 0);

	ui->sysmon->setDisabled(oprq->multix_reset);
	ui->sysmon->setValue(oprq->multix_reset ? 0 : oprq->ini_line);
	ui->multix_reset->setChecked(oprq->multix_reset);

	ui->winch_line->setValue(cfg->cfg.winch_line);
	ui->flop_line->setValue(cfg->cfg.floppy_line);
}

// -----------------------------------------------------------------------
void MainWindow::update_tab_disks_from_config()
{
	// select first row and update disk widgets accordingly
	ui->disk_list->setCurrentRow(0);
	on_disk_list_currentRowChanged(0);

	// update item names on the list
	for (int i=0 ; i<CRK5_CFG_DISK_SLOTS ; i++) {
		struct crk5_cfg_disk *disk = cfg->cfg.disk + i;
		set_disk_list_item_name(i, disk->configured ? disk->type + 1 : 0, disk->foreign);
	}
}

// -----------------------------------------------------------------------
void MainWindow::update_tab_tapes_from_config()
{
	struct crk5_cfg_tape *tape = cfg->cfg.tape;

	ui->tape0_chan->setValue(tape[0].raw);
	ui->tape1_chan->setValue(tape[1].raw);
	ui->tape2_chan->setValue(tape[2].raw);
	ui->tape3_chan->setValue(tape[3].raw);
}

// -----------------------------------------------------------------------
void MainWindow::update_tab_multix_from_config()
{
	struct crk5_cfg_mx_lines *mx = &cfg->cfg.mx;

	ui->mx_automultix->setChecked(mx->auto_multix);
	ui->mx_no_parity->setChecked(mx->no_parity);
	ui->mx_odd_parity->setChecked(mx->odd_parity);
	ui->mx_bits8->setChecked(mx->bits8);
	ui->mx_xonxoff->setChecked(mx->xon_xoff);
	ui->mx_bscan->setChecked(mx->bs_can);
	ui->mx_uppercase->setChecked(mx->uppercase);
	ui->mx_strvectors->setValue(mx->stream_vectors);
}

// -----------------------------------------------------------------------
void MainWindow::set_line_list_item_names()
{
	int start_line = 0;
	for (int i=0 ; i<CRK5_CFG_LINE_SLOTS ; i++) {
		struct crk5_cfg_lines *lines = cfg->cfg.lines + i;
		QString name;
		if (!lines->configured) {
			name = QString(tr("Brak konfiguracji"));
		} else if (lines->multix) {
			name = QString(tr("MULTIX, linie %1..%2")).arg(start_line).arg(start_line + lines->d.mx.count - 1);
			if (lines->multix) start_line += lines->d.mx.count;
		} else {
			name = QString(tr("Kanał znakowy, urządzenie %1")).arg(lines->d.ch.dev);
		}
		ui->line_list->item(i)->setText(name);
	}
}

// -----------------------------------------------------------------------
void MainWindow::update_tab_lines_from_config()
{
	// select first row and update line widgets accordingly
	ui->line_list->setCurrentRow(0);
	on_line_list_currentRowChanged(0);
	set_line_list_item_names();
}

// -----------------------------------------------------------------------
void MainWindow::update_tab_winch_types_from_config()
{
	ui->wtype_list->setCurrentRow(0);
	on_wtype_list_currentRowChanged(0);
}

// -----------------------------------------------------------------------
void MainWindow::load()
{
	if (!continue_with_save()) return;

	QString title;
	this->setWindowTitle("Krokonf");

	QString filename = QFileDialog::getOpenFileName(this, tr("Otwórz obraz dysku lub plik z jądrem CROOK-5"), nullptr, nullptr);
	if (filename.isNull()) {
		return;
	}

	QFileInfo fi(filename);

	struct crk5_kern_result *res = cfg->scan_for_kernels(filename);
	off_t offset;

	if (!res) {
		QMessageBox msgBox;
		msgBox.setText(tr("W pliku nie znaleziono jądra CROOK-5"));
		msgBox.exec();
		goto cleanup;
	}

	// just one kernel - this is the only one possible to load
	if (!res->next) {
		offset = res->offset * 2;
	// more than one kernel found - let the user choose
	} else {
		KernelSelect dialog(this);
		QList<int> offsets;
		struct crk5_kern_result *nres = res;
		while (nres) {
			offsets.append(nres->offset * 2);
			QString name = QString(tr("Wersja %3/%4 dla %5 (%2zainstalowany, offset %1)"))
					.arg(nres->offset)
					.arg(nres->raw ? tr("nie ") : "")
					.arg(nres->vmaj)
					.arg(nres->vmin)
					.arg(nres->mod ? "MX-16" : "MERA-400");
			dialog.additem(new QListWidgetItem(name));
			nres = nres->next;
		}

		int ret = dialog.exec();

		if (ret == QDialog::Rejected) {
			goto cleanup;
		}

		int item = dialog.get_item();
		offset = offsets[item];
	}

	// load selected kernel configuration
	if (!cfg->load(filename, offset)) {
		QMessageBox msgBox;
		msgBox.setText(tr("Nie udało się załadować konfiguracji"));
		msgBox.exec();
		goto cleanup;
	}

	for(auto *widget : ui->tabs->findChildren<QWidget *>())
	{
		for(auto *c : widget->findChildren<QWidget *>()) {
			c->setEnabled(true);
		}
	}

	update_tab_system_from_config();
	update_tab_memory_from_config();
	update_tab_io_from_config();
	update_tab_disks_from_config();
	update_tab_tapes_from_config();
	update_tab_multix_from_config();
	update_tab_lines_from_config();
	update_tab_winch_types_from_config();

	title = QString("%1 @%2 — Krokonf").arg(fi.fileName()).arg(offset);
	this->setWindowTitle(title);

cleanup:
	if (res) crk5_kern_res_drop(res);
}

// -----------------------------------------------------------------------
void MainWindow::save()
{
	if (!cfg->is_loaded()) return;
	cfg->save();
}

// -----------------------------------------------------------------------
bool MainWindow::continue_with_save()
{
	// no configuration loaded or no changes made
	if (!cfg->modified()) {
		return true;
	}

	QMessageBox msgBox;
	msgBox.setText(tr("Konfiguracja została zmodyfikowana."));
	msgBox.setInformativeText(tr("Chcesz zapisac zmiany?"));
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);

	switch (msgBox.exec()) {
	case QMessageBox::Discard:
		return true;
	case QMessageBox::Save:
		save();
		return true;
	}

	return false;
}

// -----------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent* event)
{

	if (continue_with_save()) {
		event->accept();
	} else {
		event->ignore();
	}

}
