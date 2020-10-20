#pragma once

namespace WindowTools 
{
KERNEL_API void ProcessOpacity(QWidget* const window, bool show = true, const int msec = 1000, const double lowTreshold = 0.0, const double highTreshold = 1.0);

KERNEL_API void MoveToScreenCenter(QWidget* const window);
KERNEL_API void MoveToRightBottom(QWidget* const window);
KERNEL_API void MoveUderAllAppWindows(QWidget* targetWidget);
}