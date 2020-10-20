#include "stdafx.h"

#include "QtTools.h"

namespace
{

class SleeperThread : public QThread
{
public:
	static void Msleep(unsigned long msecs)
	{
		G_ASSERT(msecs > 0);

		QThread::msleep(msecs);
	}
};

}

void QtTools::SwitchToWindow(QWidget* const window)
{
	CHECK_PTR(window);
	G_ASSERT(window->isWindow());

	window->showNormal();
	window->activateWindow();
}

void QtTools::Msleep(unsigned long msecs)
{
	G_ASSERT(msecs > 0);

	SleeperThread::Msleep(msecs);
}

QString QtTools::GetQtClassName(const QObject* const obj)
{
	return QLatin1String(obj->metaObject()->className());
}

bool QtTools::CloseAllWindows()
{
	QApplication::closeAllWindows();

	bool allWindowsWasClosed = true;
	const QWidgetList topWidgets =  QApplication::topLevelWidgets();
	Q_FOREACH(QWidget* const widget, topWidgets) {
		if (widget->isWindow() && widget->isVisible()) {
			allWindowsWasClosed = false;
		}
	}

	return allWindowsWasClosed;
}

void QtTools::QueuedDeleter::operator()(QObject* const ptr) const
{
	CHECK_PTR(ptr);
	ptr->deleteLater();
}

QWidget* QtTools::FindWidget(const WId wid, const bool topLevel)
{
	QWidget* result = nullptr;

	const QWidgetList widgets = topLevel ? QApplication::topLevelWidgets() : QApplication::allWidgets();
	Q_FOREACH(QWidget* widget, widgets) {
		if ( widget->winId() == wid) {
			result = widget;
			break;
		}
	}

	return result;
}

void QtTools::ShowLayout(QLayout* layout, bool show)
{
	CHECK_PTR(layout);

	const int itemsCount = layout->count();
	for (int i = 0; i < itemsCount; ++i) {
		QLayoutItem* item = layout->itemAt(i);
		QWidget* widget =  item->widget();
		if(widget != nullptr) {
			widget->setVisible(show);
		}
	}
}
