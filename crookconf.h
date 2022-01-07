#ifndef CROOKCONF_H
#define CROOKCONF_H

#include <QString>
#include <QFile>

#include <emcrk/cfg.h>
#include <emcrk/kfind.h>

// -----------------------------------------------------------------------
class CrookConf
{
private:
	QFile file;
	off_t offset;
	uint16_t data[CRK5_CFG_SIZE_WORDS];
	bool loaded = false;

public:
	struct crk5_cfg cfg;

	CrookConf();
	~CrookConf();
	struct crk5_kern_result * scan_for_kernels(QString filename);
	bool load(QString filename, off_t offset);
	void save();
	bool modified();
	bool initialized();

};

#endif // CROOKCONF_H
