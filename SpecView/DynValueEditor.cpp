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

#include "DynValueEditor.h"
#include <q3multilineedit.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3Frame>
#include <QKeyEvent>
#include <QEvent>
#include <QHeaderView>
#include <Q3VBoxLayout>
#include <QItemDelegate>
#include <QItemEditorFactory>
#include <QPainter>
using namespace Spec;
using namespace Root;

static Root::Any s_any;
static Root::Any s_any2;

//////////////////////////////////////////////////////////////////////

class DynValueCellDelegate : public QItemDelegate 
{
public:
	DynValueCellDelegate( QObject* o ):QItemDelegate(o) {}

	QWidget * createEditor ( QWidget * parent, const QStyleOptionViewItem & option, 
		const QModelIndex & index ) const
	{
		QVariant d = index.data( DynValueCell::TypeRole );
		if( d.toInt() == Any::Memo )
		{
			Q3MultiLineEdit* ed = new Q3MultiLineEdit( parent );
			ed->setFrameShape( Q3Frame::NoFrame );
			ed->setWordWrap( Q3MultiLineEdit::WidgetWidth );
			if( index.data( DynValueCell::ReadOnlyRole ).toBool() )
				ed->setReadOnly( true );
			return ed;
		}else if( !index.data( DynValueCell::ReadOnlyRole ).toBool() )
			return QItemDelegate::createEditor( parent, option, index );
		else
			return 0;
	}
	void setEditorData ( QWidget * editor, const QModelIndex & index ) const
	{
		if( Q3MultiLineEdit* e = dynamic_cast<Q3MultiLineEdit*>( editor ) )
		{
			e->setText( index.data().toString() );
		}else 
			QItemDelegate::setEditorData( editor, index );

	}
	void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
	{
		if( Q3MultiLineEdit* e = dynamic_cast<Q3MultiLineEdit*>( editor ) )
		{
			model->setData( index, e->text() );
		}else 
			QItemDelegate::setModelData( editor, model, index );
	}
	void drawDisplay ( QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect, const QString & text ) const
	{
		painter->save();
		painter->setClipRect( rect );
		// QStyleOptionViewItem o = option;
		// o.displayAlignment = Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap;
		QItemDelegate::drawDisplay( painter, option, rect, text );
		painter->restore();
	}
	bool eventFilter(QObject *object, QEvent *event)
	{
        if( dynamic_cast<Q3MultiLineEdit*>( object ) )
		{
			if (event->type() == QEvent::KeyPress) 
			{
				switch (static_cast<QKeyEvent *>(event)->key()) 
				{
				case Qt::Key_Enter:
				case Qt::Key_Return:
				case Qt::Key_Up:
				case Qt::Key_Down:
				case Qt::Key_Right:
				case Qt::Key_Left:
				case Qt::Key_Home:
				case Qt::Key_End:
					return false;
				}
			}
		}
		return QItemDelegate::eventFilter( object, event );
	}
};

DynValueCell::DynValueCell( Root::Object* o, Root::SymbolString a, Root::Any::Type t ):
	QTableWidgetItem( UserType + t ), d_obj( o ), d_att( a )
{
	setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable );
}

DynValueCell::~DynValueCell()
{
}

QVariant DynValueCell::data( int role ) const
{
	if( role == TypeRole )
		return type() - UserType;
	if( role == ReadOnlyRole )
		return ( d_obj->getFieldInfo( d_att ) == Root::Object::ReadOnlyField );
	if( role == Qt::DisplayRole || role == Qt::EditRole )
	{
		if( !d_obj->getFieldValue( d_att, s_any ) )
			return QVariant();
		switch( type() - UserType )
		{
		case Any::Null:
			return QVariant();
		case Any::Boolean:
			return s_any.getBoolean();
		case Any::Char: 
		case Any::UChar: 
		case Any::Short: 
		case Any::UShort: 
		case Any::Long: 
		case Any::ULong: 
		case Any::Float: 
		case Any::Double: 
		case Any::DateTime: 
		case Any::Date: 
		case Any::Time: 
		case Any::CStr: 
		case Any::OID:	// TODO
			return QByteArray( s_any.getCStr() );
		case Any::Memo:
			return QString::fromLatin1( s_any.getCStr() );
		}
	}
	return QVariant();
}

void DynValueCell::setData( int role, const QVariant & value )
{
	if( role != Qt::EditRole )
		return;
	if( d_obj->getFieldInfo( d_att ) == Root::Object::ReadOnlyField )
		return;
	QByteArray  val;
	Root::Any::Type t = Root::Any::Type( type() - UserType );
	switch( t )
	{
	case Any::Null:
		break;
	case Any::Boolean:
		{
			if( value.toBool() )
				val = "true";
			else
				val = "false";
		}
		break;
	default:
		{
			val = value.toString().toLatin1().data();
		}
		break;
	}
	if( s_any.parse( val.data(), t ) )
	{
		s_any2.setNull();
		d_obj->getFieldValue( d_att, s_any2 );
		if( !s_any.equals( s_any2 ) )
			d_obj->setFieldValue( d_att, s_any );
	}else
	{
		QString str;
		str.sprintf( "Invalid %s: '%s'", Any::getTypeName( t ), 
			val.data() );
		QMessageBox::critical( tableWidget(), "Set Attribute", str, "&OK" );
	}
}


//////////////////////////////////////////////////////////////////////

QAbstractItemDelegate* DynValueEditor::createDelegate(QObject* o)
{
	return new DynValueCellDelegate( o );
}


DynValueEditor::DynValueEditor(QWidget* p, Spec::ObjectDef* def, Root::Object* obj):
	QTableWidget( p ), d_obj( obj )
{
	assert( obj );
	horizontalHeader()->hide();
	setColumnCount( 1 );
	setAlternatingRowColors( true );
	setSelectionMode( QAbstractItemView::NoSelection );
    horizontalHeader()->setMovable(false);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setMovable(false);
	verticalHeader()->setResizeMode ( QHeaderView::Interactive );
	setEditTriggers( QAbstractItemView::AllEditTriggers );
	/*
	setTopMargin( 0 );
	*/


	typedef std::map<QByteArray ,Root::Any::Type> Order;
	Order order;
	if( def )
	{
		const ObjectDef::Attributes& atts = def->getAttributes();
		ObjectDef::Attributes::const_iterator p1;
		for( p1 = atts.begin(); p1 != atts.end(); ++p1 )
			order[ (*p1).first.data() ] = (*p1).second.d_type;
	}else
	{
		Root::Object::FieldValues f = d_obj->getFieldValues();
		Root::Object::FieldValues::const_iterator p1;
		for( p1 = f.begin(); p1 != f.end(); ++p1 )
		{
			order[ (*p1).first.data() ] = (*p1).second.getType();
		}
	}
	setRowCount( order.size() );
	int i = 0;
	QString str;
	int w = 0, tmp;
	QFontMetrics fm = verticalHeader()->fontMetrics();
	Order::const_iterator p2;
	QStringList labels;
	for( p2 = order.begin(); p2 != order.end(); ++p2, i++ )
	{
		str.sprintf( "%s (%s):", (*p2).first.data(), 
			Root::Any::getTypeName( (*p2).second ) );
		labels << str;
		tmp = fm.width( str );
		if( tmp > w )
			w = tmp;
		setItem( i, 0, new DynValueCell( d_obj, (*p2).first, (*p2).second ) );
		if( (*p2).second == Any::Memo )
			setRowHeight( i, rowHeight( i ) * 4 );	// RISK
	}
	setVerticalHeaderLabels( labels );

	// verticalHeader()->setResizeMode( QHeaderView::ResizeToContents );
	// setLeftMargin( w + w * 0.3 );
	setItemDelegate( new DynValueCellDelegate( this ) );
}

DynValueEditor::~DynValueEditor()
{
	// Stop running edit
	setCurrentIndex( QModelIndex() ); 
}

bool DynValueEditor::edit(QWidget * parent, Spec::ObjectDef * def, Root::Object * obj)
{
	if( obj == 0 )
		return false;
	QDialog dlg( parent, "", true );
	QString str;
	if( def )
		str.sprintf( "Edit Attributes of %s '%s'", def->getClass().data(),
			obj->getInstanceName().data() );
	else
		str.sprintf( "Edit Attributes of %s '%s'", obj->getClassName(),
			obj->getInstanceName().data() );
	dlg.setCaption( str );


	Q3VBoxLayout* top = new Q3VBoxLayout( &dlg, 5 );
	Q3HBoxLayout* buttons = new Q3HBoxLayout();

	DynValueEditor* tv = new DynValueEditor( &dlg, def, obj );
	top->addWidget( tv );

	QPushButton* ok = new QPushButton( "&Close", &dlg );
	QObject::connect( ok, SIGNAL( clicked() ), &dlg, SLOT( accept() ) );
	buttons->addWidget( ok );
	ok->setDefault( true );

	top->addLayout( buttons );

	dlg.resize( 600, 450 ); // RISK
	dlg.exec();
	ok->setFocus();
	return true;
}

