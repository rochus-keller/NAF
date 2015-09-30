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

#ifndef _ROOT_ACTIONCOMMAND
#define _ROOT_ACTIONCOMMAND

#include <Root/Action.h>
#include <QVector>

namespace Root
{

	class ActionCommand : public Action
	{
	public:
		void clear();
		Any::Type getType( Index ) const;
		const QVector<Any::Type>& getTypes() const { return d_types; }
		const char* getName( Index i ) const { return d_names[ i ]; }
		void setParam( Index, const Any& );
		void registerParameter( Any::Type, bool mandatory = true, const char* name = "" );
		const char* getSymbol() const;
		short getMandatory() const { return d_mandatory; }
		bool isImmediate() const { return d_immediate; }

		//* Overrides
		const Any& getParam( Index ) const;
		Index getParamCount() const;
		bool operator==( CmdStr ) const;
		bool isEnabled() const;
		void update();
		void execute();
		const char* getPrettyName() const;
		bool isUpdate() const;
		CmdSym getCommand() const;
		void setEnabled( bool );

		ActionCommand( Action::CmdSym, const char* mnemonic = "", 
			const char* name = "", bool immediate = false );
		ActionCommand( Action::CmdSym, const Params&, bool updating );
	private:
		Action::CmdSym d_command;
		QByteArray d_prettyName;
		QByteArray d_mnemonic;
		QVector<Any::Type> d_types;
		Params d_params;
		QVector<QByteArray> d_names; 
		bool d_immediate;	// Ausführen ohne Enter, sobald ganzes Mnemonic eingetippt.
		bool d_enabled;		// Für Update-Zyklus, ob Mnemonic zur Verfügung steht.
		bool d_updating;	// Definiert, ob es sich um Update-Zyklus handelt.
		short d_mandatory;	// Anzahl obligatorischer Parameter.
	};
}

#endif 
