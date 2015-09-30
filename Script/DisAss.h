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

#ifndef DISASS_H
#define DISASS_H

#include <QIODevice>
#include <QTextDocument>
#include <QTextCursor>

namespace Lua
{
    class StringVec;

    class DisAss : public QObject
    {
    public:
        struct Vars
        {
            QByteArray d_name;
            int d_start; // startpc, first point where variable is active, index in d_code
            int d_end;   // first point where variable is dead, index in d_code
        };

        struct Function
        {
            QByteArray d_source;         // Proto.source
            int d_firstLine;             // Proto.linedefined
            int d_lastLine;              // Proto.lastlinedefined
            quint8 d_numOfUpVals;        // Proto.nups, number of upvalues
            quint8 d_numOfParams;        // Proto.numparams
            quint8 d_varArg;            // Proto.is_vararg
            quint8 d_maxStackSize;       // Proto.maxstacksize
            QVector<quint32> d_code;
            QVector<QVariant> d_consts;  // Proto.k, constants used by the function
            QVector<Function> d_funcs;   // Proto.p, functions defined inside the function
            QVector<int> d_codeLines;    // Proto.lineinfo, map from opcodes to source lines
            QVector<Vars> d_localVars;   // Proto.ocvars, information about local variables
            QVector<QByteArray> d_upValNames; // Proto.upvalues, upvalue names
            int d_nr; // Index der Funktion im übergeordneten d_funcs;
            QByteArray d_name; // ermittelter Name der Funktion
        };

        // NOTE zu d_varArgs:
        // bei VARARG_HASARG wird noch ein parameter dazugezählt
        // VARARG_ISVARARG ist immer gesetzt sobald variable Argumente, egal in welcher Art
        // VARARG_NEEDSARG und VARARG_HASARG schliessen sich gegenseitig aus.

        explicit DisAss();
        bool disassemble( QIODevice* in );
        QString toString( bool meetLineNumbers = true, bool useNames = true, bool useConsts = true ) const;
        const QByteArray& getError() const { return d_error; }
        const Function& getTopLevel() const { return d_topLevel; }
        bool isReady() const { return d_funcReady; }
        static bool isLuaBinary( QIODevice* in );
    protected:
        bool readHeader();
        bool readFunction(Function& f);
        bool readString(QByteArray&);
        bool readBlock(char* data, quint32 size );
        bool readSwappedBlock(char *data, int n, quint32 size );
        bool readChar( char& c );
        bool readByte( quint8& b ) { return readChar( (char&) b ); }
        bool readInt( int& x );
        bool readDouble( double& x );
        bool readCode( Function &f );
        bool readConstants( Function &f );
        bool readDebug( Function &f );
        bool error( const char* );
        bool checkCode( Function &f );
        bool precheck(Function &f);
        QString formatOp( quint32 i ) const;
        void formatFunc( const Function&, StringVec&, int level ) const;
        QString formatFunc( const Function&, int level ) const;
        QString formatOp( const Function&, int pc ) const;
        QString formatArg( const Function & f, int pc, char abc ) const;
        QByteArray localName( const DisAss::Function & f, int local_number, int pc ) const;
        QString localNameOrR( const DisAss::Function & f, int local_number, int pc ) const;
        bool guessName( Function &f, int subFunction );
    private:
        QIODevice* d_in;
        QByteArray d_error;
        bool d_swap;
        bool d_funcReady; // topLevel ist eine erfolgreich disassemblierte Funktion
        mutable bool d_useNames;
        mutable bool d_useConsts;
        Function d_topLevel;
    };
}

#endif // DISASS_H
