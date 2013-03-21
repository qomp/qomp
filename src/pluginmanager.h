#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QStringList>

class QompPlugin;
class Tune;

class PluginManager : public QObject
{
	Q_OBJECT
public:
	static PluginManager* instance();
	QStringList availablePlugins() const;
	QList<Tune> getTune(const QString& pluginName);

	
private:
	PluginManager();
	void loadStaticPlugins();
	void loadPlugins();

private:
	static PluginManager* instance_;
	QList<QompPlugin*> plugins_;
	
};

#endif // PLUGINMANAGER_H
