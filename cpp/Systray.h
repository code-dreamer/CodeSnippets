#pragma once

#include "KernelException.h"

class KERNEL_API Systray : public QObject
{
    Q_OBJECT

public:
    explicit Systray(QWidget* parentWidget);
    virtual ~Systray();

Q_SIGNALS:
    void IconActivated(QSystemTrayIcon::ActivationReason reason) const;

public:
	void InitTray();
	void SetIcon(const QIcon& icon, const QString& iconTooltip = EMPTY_STR);
	void HideIcon();
	void ShowMessage(const QString& title, const QString& message,
		QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int timeoutHintMs = 10000);

	void AddAction(QAction* action);
	void AddSeparator();
	const QWidget* GetParentWidget() const;

private:
    QSystemTrayIcon* trayIcon_;
	QMenu* trayIconMenu_;
	QWidget* parentWidget_;
};

namespace KernelExceptions
{

class SystrayUnavailableException : public KernelExceptions::KernelException
{
public:
	SystrayUnavailableException();
};

} // KernelExceptions