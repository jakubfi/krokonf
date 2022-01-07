#include "crookconf.h"
#include <QFile>
#include <QDataStream>
#include <QtDebug>
#include <cstring>

// -----------------------------------------------------------------------
CrookConf::CrookConf()
	:file(nullptr)
{
	crk5_cfg_init_empty(&cfg);
}

// -----------------------------------------------------------------------
CrookConf::~CrookConf()
{

}

// -----------------------------------------------------------------------
struct crk5_kern_result * CrookConf::scan_for_kernels(QString filename)
{
	QFile image(filename);
	if (!image.open(QIODevice::ReadOnly|QIODevice::ExistingOnly)) {
		return nullptr;
	}
	FILE* f = fdopen(image.handle(), "r");
	struct crk5_kern_result *res = crk5_kern_findall_file(f, true);
	image.close();

	return res;
}

// -----------------------------------------------------------------------
bool CrookConf::load(QString f, off_t o)
{
	file.setFileName(f);
	offset = o;
	if (!file.open(QIODevice::ReadOnly|QIODevice::ExistingOnly)) {
		return false;
	}

	// load data into local buffer, required for comparison in modified()
	file.seek(offset);
	QDataStream input(&file);
	input.setByteOrder(QDataStream::BigEndian);
	for (int i=0 ; i<CRK5_CFG_SIZE_WORDS ; i++) {
			input >> data[i];
	}

	crk5_cfg_decode(data, &cfg);

	file.close();
	loaded = true;

	return true;
}

// -----------------------------------------------------------------------
void CrookConf::save()
{
	crk5_cfg_encode(&cfg, data);

	char buf[CRK5_CFG_SIZE_BYTES];
	for (int i=0 ; i<CRK5_CFG_SIZE_WORDS ; i++) {
			buf[2*i] = data[i] >> 8;
			buf[2*i+1] = data[i] & 0xff;
	}

	file.open(QIODevice::ReadWrite|QIODevice::ExistingOnly);
	file.seek(offset);
	file.write(buf, CRK5_CFG_SIZE_BYTES);
	file.close();
}

// -----------------------------------------------------------------------
bool CrookConf::modified()
{
	uint16_t data_test[CRK5_CFG_SIZE_WORDS];
	crk5_cfg_encode(&cfg, data_test);

	int ranges[2][2] = {
		{ CRK5_CFG_CFG_START, CRK5_CFG_CFG_END },
		{ CRK5_CFG_WTYPES_START, CRK5_CFG_WTYPES_END },
	};
	for (int j=0 ; j<2 ; j++) {
		for (int i=ranges[j][0] ; i<=ranges[j][1] ; i++) {
			if (data[i] != data_test[i]) {
				return true;
			}
		}
	}

	return false;
}

// -----------------------------------------------------------------------
bool CrookConf::initialized()
{
	return loaded;
}
