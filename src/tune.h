#ifndef TUNE_H
#define TUNE_H

#include <QString>

class Tune
{
public:
	Tune();
	QString artist;
	QString title;
	QString trackNumber;
	QString album;
	QString duration;
	QString url;
	QString file;

	int id() const;

	bool operator==(const Tune& other);

private:
	static int lastId_;
	int id_;
};

typedef QList<Tune> TuneList;

#endif // TUNE_H
