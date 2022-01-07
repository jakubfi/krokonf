#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "crookconf.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	Ui::MainWindow *ui;
	CrookConf *cfg;

	void update_tab_system_from_config();
	void update_tab_memory_from_config();
	void update_tab_io_from_config();
	void update_tab_disks_from_config();
	void update_tab_tapes_from_config();
	void update_tab_multix_from_config();
	void update_tab_lines_from_config();
	void update_tab_winch_types_from_config();

	void set_memory_widgets_state(bool state);
	void set_memory_list_item_name(int i, int blocks);
	void set_disk_list_item_name(int i, int type, bool foreign);
	void set_line_list_item_names();
	void set_line_widgets_state(int chan_widget_index);
	void refresh_disk_widget_contents(int row);

	int dir2ui(int d);
	int ui2dir(int d);

	void closeEvent(QCloseEvent* event);

public:
	MainWindow(CrookConf *conf, QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void load();
	void save();

	void on_mem_list_currentRowChanged(int row);
	void on_mem_blocks_valueChanged(int blocks);

	void on_disk_list_currentRowChanged(int row);
	void on_disk_type_currentIndexChanged(int index);
	void on_disk_origin_currentIndexChanged(int origin);

	void on_line_chan_currentIndexChanged(int index);
	void on_line_list_currentRowChanged(int row);
	void on_line_count_valueChanged(int lines);

	void on_wtype_list_currentRowChanged(int row);
};
#endif // MAINWINDOW_H
