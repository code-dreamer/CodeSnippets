#pragma once

class KERNEL_API ConfigPlaces : public QObject
{
	Q_OBJECT

public:
	ConfigPlaces(const QString& moduleName, QObject* parent = nullptr);
	virtual ~ConfigPlaces();

Q_SIGNALS:
	void UserPathAdded();

public:
	const QString& GetLogDir() const;
	const QString& GetConfigDir() const;
	const QString& GetConfigDataDir() const;
	const QString& GetConfigTempDir() const;

	const QString& GetUserConfigDir() const;
	const QString& GetUserConfigDataDir() const;
	const QString& GetUserConfigTempDir() const;
	void AddUserPaths(const QString& userId);
	void RemoveCurrentUserPaths();

private:
	enum ConfigDirs
	{
		LogDir = 0,
		ConfigDir,
		ConfigDataDir,
		ConfigTempDir,
		UserConfigDir,
		UserConfigDataDir,
		UserConfigTempDir,
		ConfigDirsCount = 7
	};

private:
	static void MakeDirs(const QList<QString>& dirs);
	const QString& GetConfigPath(ConfigPlaces::ConfigDirs key) const;

private:
#pragma warning(push)
#pragma warning(disable : 4251)	

	QHash<ConfigDirs, QString> dirPaths_;

#pragma warning(pop)

	QString rootConfigDir_;
	QString moduleName_;
};

