#include "stdafx.h"
#include "ConfigPlaces.h"
#include "FileSystemTools.h"

//const QString rootConfigDir_ = FileSystemTools::MergePath(QDir::currentPath(), STR("Config"));
//const QString s_rootConfigDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
const QString s_dataDirName = STR("Data");
const QString s_tempDirName = STR("Temp");
const QString s_logDirName = STR("Logs");

ConfigPlaces::ConfigPlaces(const QString& moduleName, QObject* const parent)
	: QObject(parent)
	, moduleName_(moduleName)
	//, rootConfigDir_( QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) )
{
	QSTRING_NOT_EMPTY(moduleName_);

#ifdef DEBUG
	const QDir appDirPath(QCoreApplication::applicationDirPath());
	rootConfigDir_ = QDir::toNativeSeparators(FileSystemTools::MergePath( appDirPath.canonicalPath(), STR("Config") ));
#else
	rootConfigDir_ = FileSystemTools::MergePath(FileSystemTools::GetProgramDataFolder(), QCoreApplication::organizationName(), QCoreApplication::applicationName()); 
#endif	

	static_assert(ConfigPlaces::ConfigDirsCount == 7, "Unsupported items count in enum ConfigPlaces::ConfigPlaces");

	const QString moduleConfigDir = FileSystemTools::MergePath(rootConfigDir_, moduleName);
	dirPaths_[ConfigPlaces::ConfigDir] = moduleConfigDir;
	dirPaths_[ConfigPlaces::LogDir] = FileSystemTools::MergePath(moduleConfigDir, s_logDirName);
	dirPaths_[ConfigPlaces::ConfigDataDir] = FileSystemTools::MergePath(moduleConfigDir, s_dataDirName);
	dirPaths_[ConfigPlaces::ConfigTempDir] = FileSystemTools::MergePath(moduleConfigDir, s_tempDirName);

	MakeDirs(dirPaths_.values());
}

ConfigPlaces::~ConfigPlaces()
{
	G_ASSERT(dirPaths_.count() == ConfigTempDir + 1);
}

void ConfigPlaces::MakeDirs(const QList<QString>& dirs)
{
	Q_FOREACH(const QString& dirName, dirs) {
		QSTRING_NOT_EMPTY(dirName);
		FileSystemTools::CreateDirIfNeeded(dirName);
	}
}

const QString& ConfigPlaces::GetLogDir() const
{
	return GetConfigPath(ConfigPlaces::LogDir);
}

const QString& ConfigPlaces::GetConfigDir() const
{
	return GetConfigPath(ConfigPlaces::ConfigDir);
}

const QString& ConfigPlaces::GetConfigDataDir() const
{
	return GetConfigPath(ConfigPlaces::ConfigDataDir);
}

const QString& ConfigPlaces::GetConfigTempDir() const
{
	return GetConfigPath(ConfigPlaces::ConfigTempDir);
}

const QString& ConfigPlaces::GetUserConfigDir() const
{
	return GetConfigPath(ConfigPlaces::UserConfigDir);
}

const QString& ConfigPlaces::GetUserConfigDataDir() const
{
	return GetConfigPath(ConfigPlaces::UserConfigDataDir);
}

const QString& ConfigPlaces::GetUserConfigTempDir() const
{
	return GetConfigPath(ConfigPlaces::UserConfigTempDir);
}

const QString& ConfigPlaces::GetConfigPath(const ConfigPlaces::ConfigDirs key) const
{
	const auto it = dirPaths_.find(key);
	G_ASSERT(it != dirPaths_.constEnd());
	QSTRING_NOT_EMPTY((*it));
	
	return *it;
}

void ConfigPlaces::AddUserPaths(const QString& userId)
{
	QSTRING_NOT_EMPTY(userId);
	static_assert(ConfigPlaces::ConfigDirsCount == 7, "Unsupported items count in enum ConfigPlaces::ConfigPlaces");

	G_ASSERT(!dirPaths_.contains(ConfigPlaces::UserConfigDir));
	dirPaths_[ConfigPlaces::UserConfigDir] = FileSystemTools::MergePath(dirPaths_[ConfigPlaces::ConfigDir], userId);
	
	G_ASSERT(!dirPaths_.contains(ConfigPlaces::UserConfigDataDir));
	dirPaths_[ConfigPlaces::UserConfigDataDir] = FileSystemTools::MergePath(dirPaths_[ConfigPlaces::UserConfigDir], s_dataDirName);

	G_ASSERT(!dirPaths_.contains(ConfigPlaces::UserConfigTempDir));
	dirPaths_[ConfigPlaces::UserConfigTempDir]  = FileSystemTools::MergePath(dirPaths_[ConfigPlaces::UserConfigDir], s_tempDirName);

	MakeDirs(dirPaths_.values());

	Q_EMIT UserPathAdded();
}

void ConfigPlaces::RemoveCurrentUserPaths()
{
	static_assert(ConfigPlaces::ConfigDirsCount == 7, "Unsupported items count in enum ConfigPlaces::ConfigPlaces");

	int removedItems = dirPaths_.remove(ConfigPlaces::UserConfigDir);
	G_ASSERT(removedItems == 1);

	removedItems = dirPaths_.remove(ConfigPlaces::UserConfigDataDir);
	G_ASSERT(removedItems == 1);

	removedItems = dirPaths_.remove(ConfigPlaces::UserConfigTempDir);
	G_ASSERT(removedItems == 1);
}
