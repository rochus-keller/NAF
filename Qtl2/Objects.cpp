/*
 * Copyright 2000-2015 Rochus Keller <mailto:rkeller@nmr.ch>
 *
 * This file is part of the CARA (Computer Aided Resonance Assignment,
 * see <http://cara.nmr.ch/>) NMR Application Framework (NAF) library.
 *
 * The following is the license that applies to this copy of the
 * library. For a license to use the library under conditions
 * other than those described here, please email to rkeller@nmr.ch.
 *
 * GNU General Public License Usage
 * This file may be used under the terms of the GNU General Public
 * License (GPL) versions 2.0 or 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in
 * the packaging of this file. Please review the following information
 * to ensure GNU General Public Licensing requirements will be met:
 * http://www.fsf.org/licensing/licenses/info/GPLv2.html and
 * http://www.gnu.org/copyleft/gpl.html.
 */

#include "Objects.h"
#include <Script2/QtObject.h>
#include <Qtl2/Variant.h>

#include <QAction>
#include <QActionGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDial>
#include <QDoubleSpinBox>
#include <QFocusFrame>
#include <QLCDNumber>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSizeGrip>
#include <QSlider>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QToolButton>
#include <QPluginLoader>
#include <QInputContext>
#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QCoreApplication>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QSessionManager>
#include <QSizeGrip>
#include <QStatusBar>
#include <QToolBar>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLayout>
#include <QLayoutItem>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QCompleter>
#include <QColorDialog>
#include <QDialog>
#include <QErrorMessage>
#include <QFileDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QProgressDialog>

#include <QSplitter>
#include <QSplitterHandle>
#include <QStackedWidget>
#include <QTabWidget>

#include <QToolTip>
#include <QWhatsThis>

#include <QDesktopWidget>

#include <QTabBar>
#include <QToolBox>

#include <QDoubleValidator>
#include <QIntValidator>
#include <QProcess>
#include <QRegExpValidator>
#include <QRubberBand>
#include <QSettings>
#include <QSplashScreen>

#include <QListView>
#include <QListWidget>
#include <QTableView>
#include <QTreeView>
#include <QTreeWidget>
#include <QHeaderView>
#include <QItemDelegate>
#include <QDirModel>
#include <QProxyModel>
#include <QStandardItemModel>
#include <QAbstractListModel>
#include <QAbstractTableModel>
#include <QAbstractItemModel>

#include <QTextBlock>
#include <QTextBlockFormat>
#include <QStringList>
#include <QTime>
#include <QTextCursor>
#include <QTextDocument>
#include <QShortcut>
#include <QProgressBar>

#include "QtlWidget.h"
#include "QtlAction.h"
#include "QtlActionGroup.h"
#include "QtlAbstractButton.h"
#include "QtlShortcut.h"
#include "QtlLCDNumber.h"
#include "QtlMenu.h"
#include "QtlPushButton.h"
#include "QtlMenuBar.h"
#include "QtlMessageBox.h"
#include "QtlToolBar.h"
#include "QtlToolBox.h"
#include "QtlToolButton.h"
#include "QtlAbstractSlider.h"
#include "QtlCheckBox.h"
#include "QtlButtonGroup.h"
#include "QtlStringList.h"
#include "QtlTime.h"
#include "QStringList"
#include "QtlLineEdit.h"
#include "QtlTabBar.h"
#include "QtlTextEdit.h"
#include "QtlProgressBar.h"
#include "QtlStackedWidget.h"
#include "QtlStatusBar.h"
#include "QtlScrollArea.h"
#include "QtlDial.h"
#include "QtlDialog.h"
#include "QtlGroupBox.h"
#include "QtlFileDialog.h"
#include "QtlTreeView.h"
#include "QtlHeaderView.h"
#include "QtlTreeWidgetItem.h"
#include "QtlTreeWidget.h"
#include "QtlFrame.h"
#include "QtlTabWidget.h"
#include "QtlPrinter.h"
#include "QtlPainter.h"
#include "QtlTableView.h"
#include "QtlAbstractItemView.h"
#include "QtlTableWidget.h"
#include "QtlTableWidgetItem.h"
#include "QtlShortcut.h"
#include "QtlCustomWidget.h"
using namespace Qtl;
using namespace Lua;

int Objects::install(lua_State * L)
{
	Widget::install( L );
	Shortcut::install( L );
	QtlFrame::install( L );
	AbstractButton::install( L );
	AbstractSlider::install( L );
    QtObject<QAbstractScrollArea,QFrame,NotCreatable>::install( L, "QAbstractScrollArea" );
	AbstractItemView::install( L );
    QtObject<QAbstractSpinBox,QWidget,NotCreatable>::install(L,"QAbstractSpinBox" );
	ActionGroup::install( L );
    CheckBox::install( L );
    QtObject<QComboBox,QWidget>::install( L, "QComboBox" );
    QtObject<QDateTimeEdit,QAbstractSpinBox>::install( L, "QDateTimeEdit");
    Dial::install( L );
    QtObject<QDoubleSpinBox,QAbstractSpinBox>::install( L, "QDoubleSpinBox" );
    QtObject<QFocusFrame,QWidget>::install( L, "QFocusFrame" );
	LCDNumber::install( L );
    QtObject<QLabel,QFrame>::install( L,"QLabel" );
    LineEdit::install( L );
    Menu::install( L );
	PushButton::install( L );
    QtObject<QRadioButton,QAbstractButton>::install( L, "QRadioButton" );
    ScrollArea::install( L );
    QtObject<QScrollBar,QAbstractSlider>::install( L, "QScrollBar" );
    QtObject<QSlider,QAbstractSlider>::install( L, "QSlider" );
    QtObject<QSpinBox>::install( L, "QSpinBox" );
	ToolButton::install( L );
    QtObject<QAbstractEventDispatcher,QObject,NotCreatable>::install( L, "QAbstractEventDispatcher" );
	Action::install( L );
    QtObject<QCoreApplication, QObject,HasCustomInit>::install( L, "QCoreApplication" );
    QtObject<QApplication,QCoreApplication,HasCustomInit>::install( L, "QApplication" );
    QtObject<QDockWidget,QWidget>::install( L, "QDockWidget" );
    QtObject<QMainWindow,QWidget>::install( L, "QMainWindow" );
	MenuBar::install( L );
#ifndef QT_NO_SESSIONMANAGER
    QtObject<QSessionManager,QObject,HasCustomInit>::install( L, "QSessionManager" );
#endif
    QtObject<QSizeGrip,QWidget,HasCustomInit>::install( L, "QSizeGrip" );
    StatusBar::install( L );
    ToolBar::install( L );
//	QtObject<QBoxLayout,AbstractObjectFactory>::install( L, 0,
//		QtObject<QLayout>::className() );
//	QtObject<QHBoxLayout,DefaultCtorObjectFactory>::install( L, 0,
//		QtObject<QBoxLayout>::className() );
//	QtObject<QVBoxLayout,DefaultCtorObjectFactory>::install( L, 0,
//		QtObject<QBoxLayout>::className() );
//	QtObject<QGridLayout,DefaultCtorObjectFactory>::install( L, 0,
//		QtObject<QLayout>::className() );
    //QtObject<QLayoutItem>::install( L );
    //QtObject<QStackedLayout>::install( L );
    StackedWidget::install( L );
    Dialog::install( L );
    QtObject<QColorDialog, QDialog,HasCustomInit>::install( L, "QColorDialog" );
    QtObject<QErrorMessage,QDialog>::install( L, "QErrorMessage" );
    FileDialog::install( L );
    QtObject<QFontDialog,QDialog,HasCustomInit>::install( L, "QFontDialog" );
	MessageBox::install( L );
	InputDialog::install( L );
    QtObject<QAbstractPrintDialog,QDialog,NotCreatable>::install( L,"QAbstractPrintDialog" );
    QtObject<QPrintDialog,QAbstractPrintDialog,HasCustomInit>::install( L, "QPrintDialog" );
	ButtonGroup::install( L );
    GroupBox::install( L );
    QtObject<QSplitter,QFrame>::install( L, "QSplitter" );
	TabWidget::install( L );
    // TODO Kein QObject QtObject<QToolTip,AbstractObjectFactory<> >::install( L );
    // TODO Kein QObject QtObject<QWhatsThis,AbstractObjectFactory<> >::install( L );
    // TODO: eigentlich ein Value-Objekt QtObject<QDesktopWidget,QWidget>::install( L, "QDesktopWidget" );
    ProgressBar::install( L );
	ProgressDialog::install( L );
    TabBar::install( L );
	ToolBox::install( L );
    QtObject<QValidator,QObject,NotCreatable>::install( L,"QValidator"  );
    QtObject<QDoubleValidator,QValidator,HasCustomInit>::install( L, "QDoubleValidator" );
    QtObject<QIntValidator,QValidator,HasCustomInit>::install( L, "QIntValidator" );
    QtObject<QRegExpValidator,QValidator,HasCustomInit>::install( L,"QRegExpValidator" );
    QtObject<QIODevice,QObject,NotCreatable>::install( L, "QIODevice" );
    QtObject<QProcess,QIODevice>::install( L, "QProcess" );
    QtObject<QRubberBand,QWidget,HasCustomInit>::install( L, "QRubberBand" );
    QtObject<QSettings,QObject>::install( L, "QSettings" );
    QtObject<QSplashScreen,QWidget>::install( L, "QSplashScreen" );
    QtObject<QAbstractItemDelegate,QObject,NotCreatable>::install( L, "QAbstractItemDelegate" );
    QtObject<QItemDelegate,QAbstractItemDelegate>::install( L, "QItemDelegate" );
    QtObject<QAbstractItemModel,QObject,NotCreatable>::install( L, "QAbstractItemModel" );
    QtObject<QDirModel,QAbstractItemModel>::install( L, "QDirModel" );
    QtObject<QProxyModel,QAbstractItemModel>::install( L, "QProxyModel" );
    QtObject<QStandardItemModel,QAbstractItemModel>::install( L, "QStandardItemModel" );
    QtObject<QAbstractListModel, QAbstractItemModel,NotCreatable>::install( L, "QAbstractListModel" );
    QtObject<QAbstractTableModel, QAbstractItemModel,NotCreatable>::install( L, "QAbstractTableModel" );
	TextEdit::install( L );
    QtObject<QInputContext,QObject,HasCustomInit>::install( L, "QInputContext" );
    QtObject<QStyle,QObject,HasCustomInit>::install( L, "QStyle" );
    QtObject<QTextDocument,QObject>::install(L,"QTextDocument");
    QtObject<QCompleter,QObject>::install( L, "QCompleter" );
    QtObject<QPlainTextEdit,QAbstractScrollArea>::install( L, "QPlainTextEdit" );

    HeaderView::install( L );
    TreeView::install( L );
    TreeWidgetItem::install( L );
    TreeWidget::install( L );
	TableView::install( L );
	TableWidget::install( L );
	TableWidgetItem::install( L );
	Printer::install( L );
	Painter::install( L );
	CustomWidget::install( L );

    return 0;
}
