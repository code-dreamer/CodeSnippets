#pragma once

namespace QtTools
{
KERNEL_API void SwitchToWindow(QWidget* window);
KERNEL_API void Msleep(unsigned long msecs);
KERNEL_API QString GetQtClassName(const QObject* obj);
KERNEL_API bool CloseAllWindows();
KERNEL_API QWidget* FindWidget(WId wid, bool topLevel = false);
KERNEL_API void ShowLayout(QLayout* layout, bool show = true);

struct KERNEL_API QueuedDeleter
{
	QueuedDeleter() {}
	QueuedDeleter(const QueuedDeleter&) {}
	void operator()(QObject* const ptr) const;
};
}
