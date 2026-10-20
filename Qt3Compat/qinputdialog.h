/*
 * Qt3 compatibility wrapper for <qinputdialog.h>.
 * Pulls in the real Qt5 QInputDialog plus Q3InputDialog, which adds the
 * Qt3-order convenience-function overloads (title first, parent last).
 * QInputDialog is redefined to Q3InputDialog so legacy call sites compile
 * unchanged; Qt5-order calls resolve to the inherited overloads.
 */

#ifndef QT3COMPAT_QINPUTDIALOG_WRAPPER_H
#define QT3COMPAT_QINPUTDIALOG_WRAPPER_H

#include <QtWidgets/QInputDialog>
#include "Qt3Compat.h"

#define QInputDialog Q3InputDialog

#endif // QT3COMPAT_QINPUTDIALOG_WRAPPER_H
