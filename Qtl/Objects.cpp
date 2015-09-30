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
#include <Script/ObjectInstaller.h>
#include <Qtl/Variant.h>
#include <Qtl/Values.h>

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
#include <QSpinBox>
#include <QToolButton>
#include <QPluginLoader>

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
//#include <QStackedWidget>
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
#include <QTableWidget>
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
#include "Bitops.h"
#include "QtlAction.h"
#include "QtlActionGroup.h"
#include "QtlLayout.h"
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
#include "QtlListView.h"
#include "QtlTableView.h"
#include "QtlHeaderView.h"
#include "QtlTreeView.h"
#include "QtlTableWidget.h"
#include "QtlTextBlock.h"
#include "QtlTextBlockFormat.h"
#include "QtlStringList.h"
#include "QtlTime.h"
#include "QtlTextCursor.h"
#include "QtlTextDocument.h"
#include "QtlTableWidgetSelectionRange.h"
#include "QStringList"

#include "QtlTabBar.h"
#include "QtlWidgetAction.h"
#include "QtlTreeWidget.h"
#include "QtlTextEdit.h"
#include "QtlProgressBar.h"
#include "QtlStackedWidget.h"
#include "QtlStatusBar.h"
#include "QtlObject.h"
#include "QtlPluginLoader.h"
#include "QtlScrollArea.h"
#include "QtlStackedLayout.h"
#include "QtlDial.h"
#include "QtlDialog.h"
#include "QtlGroupBox.h"
#include "QtlFileDialog.h"



#include "QtlShortcut.h"
using namespace Qtl;
using namespace Lua;

class _RectRubberBand : public QRubberBand
{
public:
	_RectRubberBand( QWidget* p ):
	  QRubberBand( QRubberBand::Rectangle, p ) {}
};

int Objects::install(lua_State * L)
{
	Object::install( L );
	Widget::install( L );
	Bitops::install( L );
	Shortcut::install( L );
	ObjectInstaller<QFrame>::install( L, 0, ObjectInstaller<QWidget>::className() );
	AbstractButton::install( L );
	AbstractSlider::install( L );
	ObjectInstaller<QAbstractScrollArea,AbstractObjectFactory>::install( L, 0,
		ObjectInstaller<QFrame>::className() );	
	ObjectInstaller<QAbstractItemView,AbstractObjectFactory>::install( L, 0, ObjectInstaller<QAbstractScrollArea>::className() );
	ActionGroup::install( L );
	//ObjectInstaller<QCheckBox>::install( L, 0, ObjectInstaller<QAbstractButton>::className() );
	CheckBox::install( L );
	ObjectInstaller<QComboBox>::install( L, 0, ObjectInstaller<QWidget>::className() );
	ObjectInstaller<QDateTimeEdit>::install( L );
	//ObjectInstaller<QDial>::install( L );
	ObjectInstaller<QDoubleSpinBox>::install( L );
	ObjectInstaller<QFocusFrame>::install( L );
	LCDNumber::install( L );
	ObjectInstaller<QLabel>::install( L, 0, ObjectInstaller<QFrame>::className() );
	ObjectInstaller<QLineEdit>::install( L, 0, ObjectInstaller<QWidget>::className() );
//	ObjectInstaller<QMenu>::install( L, 0, ObjectInstaller<QWidget>::className() );
	Menu::install( L );
	//ObjectInstaller<QPushButton>::install( L, 0, ObjectInstaller<QAbstractButton>::className() );
	PushButton::install( L );
	ObjectInstaller<QRadioButton>::install( L, 0, ObjectInstaller<QAbstractButton>::className() );
	//ObjectInstaller<QScrollArea>::install( L, 0, ObjectInstaller<QAbstractScrollArea>::className() );
	ObjectInstaller<QScrollBar>::install( L );
	ObjectInstaller<QSlider>::install( L );
	ObjectInstaller<QSpinBox>::install( L );
//	ObjectInstaller<QToolButton>::install( L );
	ToolButton::install( L );
	ObjectInstaller<QAbstractEventDispatcher,AbstractObjectFactory>::install( L );
	Action::install( L );
	ObjectInstaller<QApplication,AbstractObjectFactory>::install( L );
	ObjectInstaller<QCoreApplication, AbstractObjectFactory>::install( L );
	ObjectInstaller<QDockWidget>::install( L );
	ObjectInstaller<QMainWindow>::install( L, 0, ObjectInstaller<QWidget>::className() );
	//ObjectInstaller<QMenuBar>::install( L, 0, ObjectInstaller<QWidget>::className() );
	MenuBar::install( L );
	//	ObjectInstaller<QSessionManager,AbstractObjectFactory<> >::install( L );
	ObjectInstaller<QSizeGrip>::install( L );
	//ObjectInstaller<QStatusBar>::install( L );
	//ObjectInstaller<QToolBar>::install( L );
	ToolBar::install( L );
	Layout::install( L );
	ObjectInstaller<QBoxLayout,AbstractObjectFactory>::install( L, 0, 
		ObjectInstaller<QLayout>::className() );
	ObjectInstaller<QHBoxLayout,DefaultCtorObjectFactory>::install( L, 0, 
		ObjectInstaller<QBoxLayout>::className() );
	ObjectInstaller<QVBoxLayout,DefaultCtorObjectFactory>::install( L, 0, 
		ObjectInstaller<QBoxLayout>::className() );
	ObjectInstaller<QGridLayout,DefaultCtorObjectFactory>::install( L, 0, 
		ObjectInstaller<QLayout>::className() );
	//ObjectInstaller<QLayoutItem>::install( L );
	//ObjectInstaller<QStackedLayout>::install( L );
	//ObjectInstaller<QStackedWidget>::install( L );
	ObjectInstaller<QColorDialog, AbstractObjectFactory>::install( L );
	ObjectInstaller<QDialog>::install( L, 0, ObjectInstaller<QWidget>::className() );
	ObjectInstaller<QErrorMessage>::install( L );
	//ObjectInstaller<QFileDialog>::install( L );
	ObjectInstaller<QFontDialog,AbstractObjectFactory>::install( L );
	ObjectInstaller<QInputDialog,AbstractObjectFactory>::install( L );
//	ObjectInstaller<QMessageBox>::install( L );
	MessageBox::install( L );
	ObjectInstaller<QPrintDialog,AbstractObjectFactory>::install( L );
	ObjectInstaller<QProgressDialog>::install( L );
//	ObjectInstaller<QButtonGroup>::install( L );
	ButtonGroup::install( L );
	//ObjectInstaller<QGroupBox>::install( L );
	ObjectInstaller<QSplitter>::install( L, 0, ObjectInstaller<QFrame>::className() );
	ObjectInstaller<QTabWidget>::install( L, 0, ObjectInstaller<QWidget>::className() );
	// TODO Kein QObject ObjectInstaller<QToolTip,AbstractObjectFactory<> >::install( L );
	// TODO Kein QObject ObjectInstaller<QWhatsThis,AbstractObjectFactory<> >::install( L );
	ObjectInstaller<QDesktopWidget,AbstractObjectFactory>::install( L );
	//ObjectInstaller<QProgressBar>::install( L );
	//ObjectInstaller<QTabBar>::install( L );
//	ObjectInstaller<QToolBox>::install( L );
	ToolBox::install( L );
	ObjectInstaller<QDoubleValidator>::install( L );
	ObjectInstaller<QIntValidator>::install( L );
	ObjectInstaller<QProcess>::install( L );
	ObjectInstaller<QRegExpValidator>::install( L );
	ObjectInstaller<_RectRubberBand>::install( L, 0, 0, "QRubberBand" );
	ObjectInstaller<QSettings>::install( L );
	ObjectInstaller<QSplashScreen>::install( L );
	//ObjectInstaller<QListView>::install( L );
	ListView::install( L );
	ObjectInstaller<QListWidget>::install( L );
	//ObjectInstaller<QTableView>::install( L );
	TableView::install( L );
	//ObjectInstaller<QTableWidget>::install( L );
	TableWidget::install( L );
	//ObjectInstaller<QTreeView>::install( L );
	TreeView::install( L );
	//ObjectInstaller<QTreeWidget>::install( L );
	//ObjectInstaller<QHeaderView, AbstractObjectFactory>::install( L ); // RISK
	HeaderView::install( L );
	TextDocument::install( L );
	TextCursor::install( L );
	ObjectInstaller<QItemDelegate>::install( L );
	ObjectInstaller<QDirModel>::install( L );
	ObjectInstaller<QProxyModel>::install( L );
	ObjectInstaller<QStandardItemModel>::install( L );
	ObjectInstaller<QAbstractListModel, AbstractObjectFactory>::install( L );
	ObjectInstaller<QAbstractTableModel, AbstractObjectFactory>::install( L );
	ObjectInstaller<QAbstractItemModel, AbstractObjectFactory>::install( L );
	StringList::install( L );

	TabBar::install( L );
	WidgetAction::install( L );
	TreeWidget::install( L );
	TextEdit::install( L );
	ProgressBar::install( L );
	StackedWidget::install( L );
	StatusBar::install( L );
	//LineEdit::install( L );
	PluginLoader::install( L );
	ScrollArea::install( L );
	StackedLayout::install( L );
	Dial::install( L );
	//Dialog::install( L );
	GroupBox::install( L );
	FileDialog::install( L );
	return 1;
}
