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

// Code adaptiert aus lundump.c und ldebug.c mit folgender Lizenz:
/*
** $Id: lundump.c,v 2.7.1.4 2008/04/04 19:51:41 roberto Exp $
** load precompiled Lua chunks
** See Copyright Notice in lua.h
*/
/*
** $Id: ldebug.c,v 2.29.1.6 2008/05/08 16:56:26 roberto Exp $
** Debug Interface
** See Copyright Notice in lua.h
*/

#include "DisAss.h"
#include <QtDebug>
extern "C" {
#include <Lua/lundump.h>
#include <Lua/lopcodes.h>
#include <Lua/ldebug.h>
}

namespace Lua
{
    class StringVec : public QVector<QString>
    {
    public:
        StringVec() {}
        QString join( QChar sep )
        {
            // Original aus QtPrivate::QStringList_join
            QString res;
            for (int i = 0; i < size(); ++i) {
                if (i)
                    res += sep;
                res += at(i);
            }
            return res;
        }
        void setLine( int line, int level, const QString& s, bool prepend = false )
        {
            line--; // Es werden in Lua Binaries alle Zeilennummern eins zu gross angegeben
            if( line < 0 )
                return;
            if( !prepend )
            {
                if( level > 0 && at(line).isEmpty() )
                    operator[](line) = QString(level,QChar('\t'));
                operator[](line) += s;
            }else
            {
                const QString indent = ( level > 0 ) ? QString(level,QChar('\t')) : QString();
                operator[](line) = indent + s + operator[](line).trimmed();
            }
        }
    };
}

using namespace Lua;


DisAss::DisAss():d_funcReady(false),d_useNames(true),d_useConsts(true)
{
}

bool DisAss::disassemble(QIODevice *in)
{
    Q_ASSERT( in != 0 );
    d_in = in;
    d_error.clear();
    d_swap = false;
    d_topLevel = Function();
    d_funcReady = false;

    if( !readHeader() )
        return false;
    if( !readFunction(d_topLevel) )
        return false;
    d_funcReady = true;
    return true;
}

QString DisAss::toString(bool meetLineNumbers, bool useNames, bool useConsts ) const
{
    if( !d_funcReady )
        return QString();
    d_useNames = useNames;
    d_useConsts = useConsts;
    if( meetLineNumbers && !d_topLevel.d_codeLines.isEmpty() )
    {
        StringVec lines;
        lines.resize( d_topLevel.d_lastLine ); // Alle Zeilennummern sind um eins zu gross

        formatFunc( d_topLevel, lines, 0 );

        return lines.join(QChar('\n'));
    }else
    {
        return formatFunc( d_topLevel, 0 );
    }
}

bool DisAss::isLuaBinary(QIODevice *in)
{
    Q_ASSERT( in != 0 );
    return ( in->peek( ::strlen(LUA_SIGNATURE) ) == LUA_SIGNATURE );
}

bool DisAss::readHeader()
{
    char h[LUAC_HEADERSIZE];
    luaU_header(h);
    QByteArray s = d_in->read(LUAC_HEADERSIZE);
    d_swap = ( s[6] != h[6] );
    s[6] =h[6];
    s[10] = h[10];
    if( ::memcmp( h,s.constData(),LUAC_HEADERSIZE) != 0 )
        return error("bad header");
    else
        return true;
}

bool DisAss::readFunction(Function &f)
{
    if( !readString( f.d_source ) )
        return false;
    if( !readInt( f.d_firstLine ) )
        return false;
    if( !readInt( f.d_lastLine ) )
        return false;
    if( !readByte( f.d_numOfUpVals ) )
        return false;
    if( !readByte( f.d_numOfParams ) )
        return false;
    if( !readByte( f.d_varArg ) )
        return false;
    if( !readByte( f.d_maxStackSize ) )
        return false;
    if( !readCode( f ) )
        return false;
    if( !readConstants( f ) )
        return false;
    if( !readDebug( f ) )
        return false;
    if( !checkCode( f ) )
        return false;
    return true;
}

bool DisAss::readBlock(char *data, quint32 size)
{
    if( d_in->read( data, size ) != size )
        return error("unexpected end");
    else
        return true;
}

bool DisAss::readSwappedBlock(char *data, int n, quint32 size)
{
    if( !readBlock( data, n * size ) )
        return false;
    if( d_swap )
    {
        char* p = data;
        char c;
        switch( size )
        {
        case 1:
            break;
        case 2:
            while( n-- )
            {
                c = p[0]; p[0] = p[1]; p[1] = c;
                p += 2;
            }
            break;
        case 4:
            while( n-- )
            {
                c = p[0]; p[0] = p[3]; p[3] = c;
                c = p[1]; p[1] = p[2]; p[2] = c;
                p += 4;
            }
            break;
        case 8:
            while( n-- )
            {
                c=p[0]; p[0]=p[7]; p[7]=c;
                c=p[1]; p[1]=p[6]; p[6]=c;
                c=p[2]; p[2]=p[5]; p[5]=c;
                c=p[3]; p[3]=p[4]; p[4]=c;
                p += 8;
            }
            break;
        default:
            return error("bad size");
        }
    }
    return true;
}

bool DisAss::readChar(char &c)
{
    return readSwappedBlock( &c, 1, 1 );
}

bool DisAss::readInt(int &x)
{
    if( !readSwappedBlock( (char*)&x, 1, sizeof(int) ) )
        return false;
    if( x < 0 )
        return error( "bad integer" );
    else
        return true;
}

bool DisAss::readDouble(double &x)
{
    return readSwappedBlock( (char*)&x, 1, sizeof(x) );
}

bool DisAss::readCode(Function &f)
{
    int n;
    if( !readInt( n ) )
        return false;
    f.d_code.resize(n);
    return readSwappedBlock( (char*)f.d_code.data(), n, sizeof(quint32) );
}

bool DisAss::readConstants(Function &f)
{
    int n;
    if( !readInt( n ) )
        return false;
    f.d_consts.resize( n );
    for( int i = 0; i < n; i++ )
    {
        char t;
        if( !readChar( t ) )
            return false;
        switch( t )
        {
        case LUA_TNIL:
            break;
        case LUA_TBOOLEAN:
            {
                char c;
                if( !readChar( c ) )
                    return false;
                f.d_consts[i] = ( c != 0 );
            }
            break;
        case LUA_TNUMBER:
            {
                double d;
                if( !readDouble( d ) )
                    return false;
                f.d_consts[i] = d;
            }
            break;
        case LUA_TSTRING:
            {
                QByteArray s;
                if( !readString( s ) )
                    return false;
                f.d_consts[i] = s;
            }
            break;
        default:
            return error( "bad constant" );
        }
    }
    if( !readInt( n ) )
        return false;
    f.d_funcs.resize( n );
    for( int i = 0; i < n; i++ )
    {
        if( !readFunction( f.d_funcs[i] ) )
            return false;
        if( f.d_funcs[i].d_lastLine > f.d_lastLine )
            f.d_lastLine = f.d_funcs[i].d_lastLine;
        f.d_funcs[i].d_nr = i;
        // hier sind localVars noch nicht eingelesen, darum noch nicht guessName( f, i );
    }
    return true;
}

bool DisAss::readDebug(Function &f)
{
    int n;
    if( !readInt( n ) )
        return false;
    if( n != 0 && n != f.d_code.size() )
        return error( "invalid lineinfo length" );
    f.d_codeLines.resize( n );
    if( !readSwappedBlock( (char*)f.d_codeLines.data(), n , sizeof(int) ) )
        return false;
    foreach( int l, f.d_codeLines )
    {
        if( l > f.d_lastLine )
            f.d_lastLine = l; // Dieser Wert ist regelmässig null.
    }
    if( !readInt( n ) )
        return false;
    f.d_localVars.resize(n);
    for( int i = 0; i < n; i++ )
    {
        if( !readString( f.d_localVars[i].d_name ) )
            return false;
        if( f.d_localVars[i].d_name.startsWith('(') )
        {
            // "(for generator/state/control)" aus lparser.c, und weitere
            f.d_localVars[i].d_name.replace( '(', '[' );
            f.d_localVars[i].d_name.replace( ')', ']' );
            f.d_localVars[i].d_name.replace( ' ', '_' );
        }
        if( !readInt( f.d_localVars[i].d_start ) )
            return false;
        if( !readInt( f.d_localVars[i].d_end ) )
            return false;
    }
    for( int i = 0; i < f.d_funcs.size(); i++ )
    {
        guessName( f, i );
    }
    if( !readInt( n ) )
        return false;
    f.d_upValNames.resize(n);
    for( int i = 0; i < n; i++ )
    {
        if( !readString(f.d_upValNames[i]) )
            return false;
    }
    return true;
}

bool DisAss::error(const char * str)
{
    d_error = str;
    qWarning() << d_error;
    return false;
}

bool DisAss::readString(QByteArray & str)
{
    size_t size;
    if( !readSwappedBlock( (char*)&size,1,sizeof(size) ) )
        return false;
    if( size == 0 )
        return true;
    else
    {
        str.resize( size );
        return readBlock( str.data(), size );
    }
}

static bool checkArgMode(DisAss::Function &f, int r, enum OpArgMask mode)
{
    switch (mode)
    {
    case OpArgN:
        if( r != 0 )
            return false;
        else
            break;
    case OpArgU:
        break;
    case OpArgR:
        if( r >= f.d_maxStackSize )
            return false;
        else
            break;
    case OpArgK:
        if( ISK(r) )
        {
            if( INDEXK(r) >= f.d_consts.size() )
                return false;
        }else
        {
            if( r >= f.d_maxStackSize )
                return false;
        }
        break;
    }
    return true;
}

bool DisAss::precheck(DisAss::Function &f)
{
    if( f.d_maxStackSize > MAXSTACK )
        return error("stack too large");
    if( f.d_numOfParams + ( f.d_varArg & VARARG_HASARG ) > f.d_maxStackSize )
        return error("too many arguments");
    if( ( f.d_varArg & VARARG_NEEDSARG ) && !( f.d_varArg & VARARG_HASARG ) )
        return error("invalid variable arguments");
    if( f.d_upValNames.size() > f.d_numOfUpVals )
        return error("too many upvalues");
    if( f.d_codeLines.size() != f.d_code.size() && f.d_codeLines.size() != 0)
        return error("invalid line numbers");
    foreach( int l, f.d_codeLines )
    {
        if( l < f.d_firstLine || l > f.d_lastLine )
            return error("line number out of range");
    }
    if( f.d_code.isEmpty() )
        return error("no code");
    if( GET_OPCODE( f.d_code.last() ) != OP_RETURN )
        return error("last opcode must be RETURN");
    if( f.d_localVars.size() != 0 && f.d_localVars.size() < f.d_numOfParams )
        return error("more params than local variables");
    return true;
}

static inline QString _opName( int op )
{
    return QString::fromLatin1( luaP_opnames[op] ).toLower();
}

#ifdef __unused__
QString DisAss::formatOp(quint32 i) const
{
    const OpCode op = GET_OPCODE(i);
    QStringList args;
    args << QString::number( GETARG_A(i) );
    switch( getOpMode(op) )
    {
    case iABC:
        args << QString::number( GETARG_B(i) ) << QString::number( GETARG_C(i) );
        break;
    case iABx:
        args << QString::number( GETARG_Bx(i) );
        break;
    case iAsBx:
        args << QString::number( GETARG_sBx(i) );
        break;
    }

    return QString("%1(%2) ").arg( _opName(op) ).arg( args.join(QChar(',')) );
}
#endif

static inline void _dump( const QVector<DisAss::Vars>& vars )
{
    for( int i = 0; i < vars.size(); i++ )
        qDebug() << "[" << i << "]" << vars[i].d_name.data() << vars[i].d_start << vars[i].d_end;
}

QByteArray DisAss::localName( const DisAss::Function & f, int local_number, int pc ) const
{
    // Adaptiert aus luaF_getlocalname und dann empirisch solange angepasst, bis vernünftiges Ergebnis.
    //_dump( f.d_localVars ); // TEST
    pc++;
    local_number++;
    for( int i = 0; i < f.d_localVars.size() && f.d_localVars[i].d_start <= pc; i++)
    {
        if( pc < f.d_localVars[i].d_end ) /* is variable active? */
        {
            local_number--;
            if( local_number == 0 )
                return f.d_localVars[i].d_name;
        }
    }
    return QByteArray();
}

QString DisAss::localNameOrR( const DisAss::Function & f, int local_number, int pc ) const
{
    if( d_useNames )
    {
        const QByteArray name = localName( f, local_number, pc );
        if( !name.isEmpty() )
            return QString::fromLatin1( name );
    }
    // else
    return tr("R[%1]").arg(local_number);
}

static QString _toStr( const QVariant& v )
{
    if( v.isNull() )
        return QLatin1String("nil");
    else if( v.type() == QVariant::ByteArray )
        return QString("\"%1\"").arg( v.toString().simplified() );
    else
        return v.toString();
}

static bool _isConstIdx( OpCode op, int& idx, char abc )
{
    switch( abc )
    {
    case 'a':
        return false;
    case 'b':
        if(op == OP_LOADK || op == OP_GETGLOBAL || op == OP_SETGLOBAL )
            return true; // alle Kst(Bx), also iABx mit OpArgK
        else if( getBMode(op) == OpArgK && ISK(idx) ) // // iABC mit OpArgK
        {
            idx = INDEXK(idx);
            return true;
        }
        break;
    case 'c':
        if( getCMode(op) == OpArgK && ISK(idx) ) // // iABC mit OpArgK
        {
            idx = INDEXK(idx);
            return true;
        }
        break;
    }
    return false;
}

bool DisAss::guessName(DisAss::Function &f, int subFunction)
{
    // finde Funktionsname anhand verschiedener Tricks
    for( int pc = 0; pc < f.d_code.size(); pc++ )
    {
        if( GET_OPCODE(f.d_code[pc]) == OP_CLOSURE && GETARG_Bx(f.d_code[pc]) == subFunction )
        {
            QByteArray name = localName( f, GETARG_A(f.d_code[pc]), pc );
            if( !name.isEmpty() )
            { // Einfachster Fall closure(Name,F[x]) bei local function Name()
                f.d_funcs[subFunction].d_name = name;
                return true;
            }else if( pc + 1 < f.d_code.size() && GET_OPCODE(f.d_code[pc+1]) == OP_SETGLOBAL &&
                    GETARG_A(f.d_code[pc]) == GETARG_A(f.d_code[pc+1]) )
            { // closure(R[0],F[x]) setglobal(R[0],"Name")
                int kid = GETARG_Bx(f.d_code[pc+1]);
                if( _isConstIdx( GET_OPCODE(f.d_code[pc+1]), kid, 'b' ) )
                {
                    f.d_funcs[subFunction].d_name = f.d_consts[ kid ].toByteArray();
                    return true;
                }else
                    return false;
            }else if( pc + 1 < f.d_code.size() && GET_OPCODE(f.d_code[pc+1]) == OP_SETTABLE &&
                      GETARG_A(f.d_code[pc]) == GETARG_C(f.d_code[pc+1])  )
            { // closure(R[y],F[x]) settable(R[0],"Name",R[y])
                int kid = GETARG_B(f.d_code[pc+1]);
                if( _isConstIdx( GET_OPCODE(f.d_code[pc+1]), kid, 'b' ) )
                {
                    f.d_funcs[subFunction].d_name = f.d_consts[ kid ].toByteArray();
                    return true;
                }else
                    return false;
            }
        }
    }
    return false;
}

QString DisAss::formatArg( const DisAss::Function & f, int pc, char abc ) const
{
    // Thanks to the very good documentation in http://luaforge.net/docman/83/98/ANoFrillsIntroToLua51VMInstructions.pdf
    quint32 i = f.d_code[pc];
    const OpCode op = GET_OPCODE(i);
    switch( abc )
    {
    case 'a':
        {
            if( op == OP_JMP )
                return QString();
            const int a = GETARG_A(i);
            switch( op )
            {
            case OP_EQ:
            case OP_LT:
            case OP_LE:
                return QString::number(a);
            default:
                break;
            }
            return localNameOrR( f, a, pc );
        }
        break;
    case 'b':
        {
            int b = 0;
            switch( getOpMode(op) )
            {
            case iABC:
                b = GETARG_B(i);
                break;
            case iABx:
                b = GETARG_Bx(i);
                break;
            case iAsBx:
                b = GETARG_sBx(i);
                break;
            }
            if( _isConstIdx( op, b, abc ) )
            {
                if( d_useConsts )
                    return _toStr( f.d_consts[ b ] );
                else
                    return tr("K[%1]").arg(b);
            } // else
            switch( getBMode(op) )
            {
            case OpArgU:
                if( op == OP_CLOSURE )
                    return tr("F[%1]").arg(b);
                else
                    return QString::number(b);
            case OpArgK: // hier nur noch die R-Varianten davon
            case OpArgR:
                if( getOpMode(op) == iAsBx )
                    return QString::number(b);
                Q_ASSERT( getOpMode(op) == iABC );
                return localNameOrR( f, b, pc );
            case OpArgN:
                break;
            }
        }
        break;
    case 'c':
        {
            Q_ASSERT( getOpMode(op) == iABC );
            int c = GETARG_C(i);
            if( _isConstIdx( op, c, abc ) )
            {
                if( d_useConsts )
                    return _toStr( f.d_consts[ c ] );
                else
                    return tr("K[%1]").arg(c);
            } // else
            switch( getCMode(op) )
            {
            case OpArgU:
                return QString::number(c);
            case OpArgK: // hier nur noch die R-Varianten davon
            case OpArgR:
                return localNameOrR( f, c, pc );
            case OpArgN:
                break;
            }
        }
        break;
    default:
        break;
    }
    return QString();
}

QString DisAss::formatOp(const Function & f, int pc) const
{
    const OpCode op = GET_OPCODE(f.d_code[pc]);
    QStringList args;
    QString arg = formatArg( f, pc, 'a');
    if( !arg.isEmpty() )
        args << arg;
    switch( getOpMode(op) )
    {
    case iABC:
        arg = formatArg( f, pc, 'b');
        if( !arg.isEmpty() )
            args << arg;
        arg = formatArg( f, pc, 'c');
        if( !arg.isEmpty() )
            args << arg;
        break;
    case iABx:
    case iAsBx:
        arg = formatArg( f, pc, 'b');
        if( !arg.isEmpty() )
            args << arg;
        break;
    }

    return QString("%1(%2) ").arg( _opName(op) ).arg( args.join(QChar(',')) );
}

void DisAss::formatFunc(const Function & f, StringVec & out, int level) const
{
    Q_ASSERT( f.d_code.size() == f.d_codeLines.size() );

    if( level != 0 )
    {
        QStringList params;
        for( int i = 0; i < f.d_numOfParams; i++ )
        {
            if( !d_useNames )
                params << tr("R%1").arg(i);
            else if( i >= f.d_localVars.size() )
                params << QString::fromLatin1( "?" );
            else
                params << QString::fromLatin1( f.d_localVars[i].d_name );
        }
        if( f.d_varArg )
            params << QString::fromLatin1( "..." );
        out.setLine( f.d_firstLine, level - 1, tr("function F%2 (%1) ").arg(params.join(QChar(',') ) ).arg( f.d_nr ) );
    }

    QSet<int> funcLines;
    for( int i = 0; i < f.d_funcs.size(); i++ )
    {
        formatFunc( f.d_funcs[i], out, level + 1 );
        out.setLine( f.d_funcs[i].d_lastLine, level, "end " );
        funcLines.insert( f.d_funcs[i].d_firstLine );
    }

    for( int pc = 0; pc < f.d_code.size(); pc++ )
        out.setLine( f.d_codeLines[pc], level, formatOp( f, pc ), funcLines.contains(f.d_codeLines[pc]) );
    // StringVec sorgt dafür, dass bei Zeilenkollisionen code vor "function" gesetzt wird

    if( level == 0 )
    {
        out.setLine( 1, 0, tr("-- Disassembled binary Lua file; line numbers corresponding to debug information") );
        for( int line = 1; line < out.size(); line++ )
        {
            if( out[line].isEmpty() )
                out[line] = tr("-- empty" );
        }
    }
}

static inline QString _tab( int level )
{
    if( level > 0 )
        return QString( level, QChar('\t') );
    else
        return QString();
}

QString DisAss::formatFunc(const DisAss::Function & f, int level) const
{
    QString res;
	QTextStream out( &res );
    if( level == 0 )
        out << tr("-- Disassembled binary Lua file; line numbers not corresponding to debug information") << endl;
    else
    {
        QStringList params;
        for( int i = 0; i < f.d_numOfParams; i++ )
        {
            if( !d_useNames )
                params << tr("R%1").arg(i);
            else if( i >= f.d_localVars.size() )
                params << QString::fromLatin1( "?" );
            else
                params << QString::fromLatin1( f.d_localVars[i].d_name );
        }
        if( f.d_varArg )
            params << QString::fromLatin1( "..." );
        out << _tab(level-1) << tr("function F%2 (%1) ").arg(params.join(QChar(',') ) ).arg( f.d_nr );
        if( d_useNames )
            out << tr("-- ") << ( ( f.d_name.isEmpty() ) ? tr("<anonymous>") :
                                                          QString::fromLatin1( f.d_name ) ) << endl;
        else
            out << endl;
    }
    for( int pc = 0; pc < f.d_code.size(); pc++ )
        out << _tab(level) << formatOp( f, pc ) << ( ( f.d_codeLines.isEmpty() ) ?
                   QString() : tr("  -- line %1").arg( f.d_codeLines[pc]) ) << endl;
    foreach( const Function& sub, f.d_funcs )
        out << formatFunc( sub, level + 1 ) << endl;

    if( level!= 0 )
        out << _tab(level-1) << tr("end") << endl;
    return res;
}

bool DisAss::checkCode(Function &f)
{
    int reg = NO_REG;
    int last;  /* stores position of last instruction that changed `reg' */
    last = f.d_code.size() - 1;  /* points to final return (a `neutral' instruction) */
    if( !precheck(f) )
        return false;
    for( int pc = 0; pc < f.d_code.size(); pc++ )
    {
        Instruction i = f.d_code[pc];
        OpCode op = GET_OPCODE(i);
        int a = GETARG_A(i);
        int b = 0;
        int c = 0;
        if( op >= NUM_OPCODES )
            return error( "invalid opcode" );
        if( a >= f.d_maxStackSize )
            return error( "invalid register" );
        switch( getOpMode(op) )
        {
        case iABC:
            {
                b = GETARG_B(i);
                c = GETARG_C(i);
                if( !checkArgMode(f, b, getBMode(op) ) || !checkArgMode(f, c, getCMode(op) ) )
                    return error( "invalid argument mode" );
                break;
            }
        case iABx:
            {
                b = GETARG_Bx(i);
                if( getBMode(op) == OpArgK && b >= f.d_consts.size() )
                    return error( "invalid argument");
                break;
            }
        case iAsBx:
            {
                b = GETARG_sBx(i);
                if( getBMode(op) == OpArgR)
                {
                    int dest = pc + 1 + b;
                    if( !( 0 <= dest && dest < f.d_code.size() ) )
                        return error("invalid destination");
                    if( dest > 0 )
                    {
                        /* check that it does not jump to a setlist count; this
               is tricky, because the count from a previous setlist may
               have the same value of an invalid setlist; so, we must
               go all the way back to the first of them (if any) */
                        int j;
                        for( j = 0; j < dest; j++ )
                        {
                            quint32 d = f.d_code[ dest - 1 - j ];
                            if( !(GET_OPCODE(d) == OP_SETLIST && GETARG_C(d) == 0) )
                                break;
                        }
                        /* if 'j' is even, previous value is not a setlist (even if
               it looks like one) */
                        if( ( j & 1) != 0 )
                            return error("jump to a setlist count");
                    }
                }
                break;
            }
        }
        if( testAMode(op) )
        {
            if( a == reg )
                last = pc;  /* change register `a' */
        }
        if( testTMode(op) )
        {
            if( !( pc + 2 < f.d_code.size() ) )
                return error("invalid skip");  /* check skip */
            if( GET_OPCODE( f.d_code[pc+1] ) != OP_JMP )
                return error("no JMP");
        }
        switch (op)
        {
        case OP_LOADBOOL:
            {
                if (c == 1)
                {  /* does it jump? */
                    if( pc + 2 >= f.d_code.size() )
                        return error("invalid jump address for LOADBOOL");  /* check its jump */
                    if( GET_OPCODE( f.d_code[pc+1]) == OP_SETLIST && GETARG_C( f.d_code[pc+1]) == 0 )
                        return error("invalid argument for SETLIST");
                }
                break;
            }
        case OP_LOADNIL:
            {
                if( a <= reg && reg <= b )
                    last = pc;  /* set registers from `a' to `b' */
                break;
            }
        case OP_GETUPVAL:
        case OP_SETUPVAL:
            {
                if( b >= f.d_numOfUpVals )
                    return error("invalid upvalue index in GET/SETUPVAL");
                break;
            }
        case OP_GETGLOBAL:
        case OP_SETGLOBAL:
            {
                if( f.d_consts[b].type() != QVariant::ByteArray )
                    return error("invalid string argument for GET/SETGLOBAL" );
                break;
            }
        case OP_SELF:
            {
                if( a+1 >= f.d_maxStackSize )
                    return error("invalid stack reference for SELF");
                if( reg == a+1 )
                    last = pc;
                break;
            }
        case OP_CONCAT:
            {
                if(b >= c)
                    return error("not enough operands in CONCAT");  /* at least two operands */
                break;
            }
        case OP_TFORLOOP:
            {
                if(c < 1)
                    return error("at least one result expected for TFORLOOP");  /* at least one result (control variable) */
                if( a+2+c >= f.d_maxStackSize )
                    return error("not enough space for results of TFORLOOP");  /* space for results */
                if( reg >= a+2 )
                    last = pc;  /* affect all regs above its base */
                break;
            }
        case OP_FORLOOP:
        case OP_FORPREP:
            if( a+3 >= f.d_maxStackSize )
                return error("invalid stack size in FORLOOP/PREP");
            /* go through */
        case OP_JMP:
            {
                int dest = pc+1+b;
                /* not full check and jump is forward and do not skip `lastpc'? */
                if( reg != NO_REG && pc < dest && dest <= f.d_code.size() )
                    pc += b;  /* do the jump */
                break;
            }
        case OP_CALL:
        case OP_TAILCALL:
            {
                if( b != 0 )
                {
                    if( a+b-1 >= f.d_maxStackSize )
                        return error("invalid stack size in CALL");
                }
                c--;  /* c = num. returns */
                if (c == LUA_MULTRET)
                {
                    if( !luaG_checkopenop(f.d_code[ pc+1 ] ) )
                        return error("invalid instruction after open call");
                }else if(c != 0)
                {
                    if( a+c-1 >= f.d_maxStackSize )
                        return error("invalid stack size");
                }
                if( reg >= a )
                    last = pc;  /* affect all registers above base */
                break;
            }
        case OP_RETURN:
            {
                b--;  /* b = num. returns */
                if( b > 0 )
                {
                    if( a+b-1 >= f.d_maxStackSize )
                        return error("invalid stack size in RETURN");
                }
                break;
            }
        case OP_SETLIST:
            {
                if (b > 0)
                {
                    if( a + b >= f.d_maxStackSize )
                        return error("invalid stack size in SETLIST");
                }
                if (c == 0)
                {
                    pc++;
                    if( pc >= f.d_code.size() - 1)
                        return error("code list too short");
                }
                break;
            }
        case OP_CLOSURE:
            {
                int nup, j;
                if( b >= f.d_funcs.size() )
                    return error("invalid function index");
                nup = f.d_funcs[b].d_numOfUpVals;
                if( pc + nup >= f.d_code.size() )
                    return error("invalid code index");
                for (j = 1; j <= nup; j++)
                {
                    OpCode op1 = GET_OPCODE( f.d_code[pc + j] );
                    if( op1 != OP_GETUPVAL && op1 != OP_MOVE)
                        return error("invalid instructions in CLOSURE");
                }
                if( reg != NO_REG )  /* tracing? */
                    pc += nup;  /* do not 'execute' these pseudo-instructions */
                break;
            }
        case OP_VARARG:
            {
                if( !(f.d_varArg & VARARG_ISVARARG) || (f.d_varArg & VARARG_NEEDSARG) )
                    return error("invalid arguments for VARARG");
                b--;
                if (b == LUA_MULTRET)
                {
                    if( !luaG_checkopenop(f.d_code[pc+1]) )
                        return error("invalid instruction after open call");
                }
                if( a+b-1 >= f.d_maxStackSize )
                    return error("invalid stack size in VARARG");
                break;
            }
        default:
            break;
        }
    }
    if( f.d_code[last] == 0 )
        return false;
    return true;
}


