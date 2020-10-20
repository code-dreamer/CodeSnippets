#include "stdafx.h"

#include "Systray.h"
#include "CppTools.h"

using namespace KernelExceptions;

Systray::Systray(QWidget* const parentWidget) 
	: QObject(parentWidget)
	, parentWidget_(parentWidget)
    , trayIcon_(nullptr)
	, trayIconMenu_(nullptr)
{
	CHECK_PTR(parentWidget);
}

Systray::~Systray() 
{
}

void Systray::InitTray() 
{
	CHECK_PTR(parentWidget_);

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		throw SystrayUnavailableException();
	}

	G_ASSERT(trayIcon_ == nullptr);
	if (trayIcon_ != nullptr) {
		CppTools::SafeDelete(trayIcon_);
	}
	trayIcon_ = new QSystemTrayIcon(this);

	G_ASSERT(trayIconMenu_ == nullptr);
	if (trayIconMenu_ != nullptr) {
		CppTools::SafeDelete(trayIconMenu_);
	}
    trayIconMenu_ = new QMenu(parentWidget_);

    trayIcon_->setContextMenu(trayIconMenu_);
    CHECKED_CONNECT(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SIGNAL(IconActivated(QSystemTrayIcon::ActivationReason)));
}

void Systray::AddAction(QAction* const action)
{
	CHECK_PTR(action);
	CHECK_PTR(trayIconMenu_);
	G_ASSERT( !trayIconMenu_->actions().contains(action) );

	trayIconMenu_->addAction(action);
}

void Systray::AddSeparator()
{
	CHECK_PTR(trayIconMenu_);
	trayIconMenu_->addSeparator();
}

void Systray::SetIcon(const QIcon& icon, const QString& iconTooltip)
{
	G_ASSERT( !icon.isNull() );
	CHECK_PTR(trayIcon_);

	trayIcon_->setIcon(icon);
	trayIcon_->setToolTip(iconTooltip);
	trayIcon_->show();
}

void Systray::HideIcon()
{
	CHECK_PTR(trayIcon_);
	if (trayIcon_ != nullptr) {
		trayIcon_->hide();
	}
}

inline const QWidget* Systray::GetParentWidget() const
{
	CHECK_PTR(parentWidget_);
	return parentWidget_;
}

void Systray::ShowMessage(const QString& title, const QString& message,	QSystemTrayIcon::MessageIcon icon, int timeoutHintMs)
{
	trayIcon_->showMessage(title, message, icon, timeoutHintMs);
}

////////////////////////////////// SystrayUnavailableException ////////////////////////////////////////

SystrayUnavailableException::SystrayUnavailableException()
	: KernelException(QCoreApplication::translate("SystrayUnavailableException", "Couldn't detect any system tray on this system."))
{
}
