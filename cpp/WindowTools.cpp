#include "stdafx.h"

#include "WindowTools.h"
#include "QtTools.h"

//TODO: function very large, may be it must be splitted?
void WindowTools::ProcessOpacity(QWidget* const window, const bool show /*= true*/, const int msec /*= 1000*/,
	const double lowTreshold /*= 0.0*/, const double highTreshold /*= 1.0*/) 
{
		G_ASSERT(window != NULL);
		G_ASSERT(window->isWindow());
		G_ASSERT(msec > 0);
		G_ASSERT(lowTreshold < highTreshold);

		if ( !window->isWindow() )
			return;

		const double sleepStep = 5;
		double opacityStep = (highTreshold - lowTreshold) / std::min<double>(msec, msec/sleepStep);
		double currOpacity;
		if (show) {
			currOpacity = lowTreshold;
		}
		else {
			opacityStep = -opacityStep;
			currOpacity = highTreshold;
		}

		window->setWindowOpacity(currOpacity);
		if (!window->isVisible())
			window->show();

		const double finalTreshold = show ? highTreshold : lowTreshold;

		while ( show ? (currOpacity < finalTreshold) : (currOpacity > finalTreshold) ) {
			window->setWindowOpacity(currOpacity);
			window->repaint();
			QtTools::Msleep( qRound(sleepStep) );
			//window->update();
			qApp->processEvents();
			currOpacity += opacityStep;
		}

		if (window->windowOpacity() != finalTreshold) {
			window->setWindowOpacity(finalTreshold);
			window->repaint();
		}

		/*
		if (!show && lowTreshold == 0.0) {
			window->hide();
		}*/
}

void WindowTools::MoveToScreenCenter(QWidget* const window) 
{
	CHECK_PTR(window);
	G_ASSERT( window->isWindow() );

	const QRect screen = QApplication::desktop()->screenGeometry();
	window->move( screen.center() - window->rect().center() );
}

void WindowTools::MoveToRightBottom(QWidget* const window) 
{
	CHECK_PTR(window);
	G_ASSERT(window->isWindow());

	const QRect screen = QApplication::desktop()->availableGeometry();
	const QRect wndRect = window->rect();
	window->move( QPoint(screen.right() - wndRect.width() - 5, screen.bottom() - wndRect.height()) );
}

void WindowTools::MoveUderAllAppWindows(QWidget* targetWidget) 
{
	CHECK_PTR(targetWidget);

	const QDesktopWidget* desktopWidget = QApplication::desktop();
	const QRect screen = desktopWidget->availableGeometry();
	QPoint maxTopLeft(screen.bottomRight());

	bool found = false;
	const QWidgetList widgets = qApp->allWidgets();
	Q_FOREACH(QWidget* widget, widgets) {
		if (widget->isWindow() && widget->isVisible() && widget != desktopWidget && widget != targetWidget) {
			const QPoint topLeft = widget->geometry().topLeft();
			if (topLeft.y() < maxTopLeft.y()) {
				maxTopLeft = topLeft;
				found = true;
			}
		}
	}

	if (found) {
		const QRect targetWidgetRect = targetWidget->geometry();
		maxTopLeft.setX(targetWidgetRect.left());
		maxTopLeft.setY( maxTopLeft.y() - targetWidgetRect.height() - 5 );
		targetWidget->move(maxTopLeft);
	}
}
