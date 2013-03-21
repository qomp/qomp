#ifndef PROSTOPLEERPLUGINSETTINGS_H
#define PROSTOPLEERPLUGINSETTINGS_H

#include <QWidget>

namespace Ui {
class ProstopleerPluginSettings;
}

class ProstopleerPluginSettings : public QWidget
{
	Q_OBJECT
	
public:
	explicit ProstopleerPluginSettings(QWidget *parent = 0);
	~ProstopleerPluginSettings();

public slots:
	void apply();
	void restore();
	
private:
	Ui::ProstopleerPluginSettings *ui;
};

#endif // PROSTOPLEERPLUGINSETTINGS_H
