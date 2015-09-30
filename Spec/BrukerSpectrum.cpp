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

#include "BrukerSpectrum.h"
#include <QFileInfo>
#include <QFile>
#include <Spec/Buffer.h>
#include <Root/Host.h>
using namespace Spec; 
using namespace Spec::Private; 
using namespace Root;

static const Amplitude s_scf = 100.0;

BrukerSpectrum::BrukerSpectrum(const char* filePath):
    d_turnSex( false ), d_factor(1.0)
{
	d_word = 4;
	d_specFile = filePath; 
	QFileInfo info( filePath );
	
	QByteArray  ext = info.extension( false ).toLatin1();
	Dimension dim = 0;
	QByteArray  path = info.filePath().toLatin1();
	bool ok = true;
	if( ext.isEmpty() )
	{
		// File-Name: digit ( "r" | "i" ) ( "r" | "i" )*
		// Param-File-Name: "proc" ( digit )? "s"

		ext = info.baseName().toLatin1();
		ok = !ext.isEmpty();
		if( ok )
			dim = info.baseName().left( 1 ).toUShort( &ok );
		if( ok )
			ok = ext.size() == dim + 1;
		if( ok )
			for( int i = 1; i < ext.size(); i++ )
				ok = ok && ( ext[ i ] == 'r' || ext[ i ] == 'i' || 
					ext[ i ] == 'R' || ext[ i ] == 'I' );
		path.truncate( path.size() - ext.size() );
		if( !ok )
			throw Root::Exception( "File name not like 'digit ( 'r' | 'i' ) ( 'r' | 'i' )*': ",
				ext.data() );
	}else
	{
		// Spektrum-File-Extension: . digit ( "r" | "i" ) ( "r" | "i" )*
		// Param-File-Extension: . "proc" ( digit )? "s"
		// digit 1..9
		// Zu jedem Spektrum-File existieren D Param-Files.

		dim = info.extension( false ).left( 1 ).toUShort( &ok );
		if( ok && ext.size() < dim + 1 )
			ok = false;
		for( int i = 1; i < ext.size(); i++ )
			if( ext[ i ] != 'r' && ext[ i ] != 'i' && 
				ext[ i ] != 'R' && ext[ i ] != 'I' )
				ok = false;
		if( !ok )
			throw Root::Exception( "File extension not like 'digit ( 'r' | 'i' ) ( 'r' | 'i' )*': ",
				ext.data() );
		path.truncate( path.size() - ext.size() );
	}

	PpmPoint::checkDimCount( dim );
	d_scales.assign( dim, Scale() );
	d_label.assign( dim, "" );
	d_tileSize.assign( dim, 0 );
	d_tileCount.assign( dim, 0 );
	d_rf.assign( dim, 0 );

	readParams( path.data() );
	adjustNumOfSamples();
	init();
}

static bool readInt( QIODevice& in, const QByteArray& id, int& res )
{ 
    in.reset();
    while( !in.atEnd() )
	{
        const QByteArray line = in.readLine().trimmed();
        if( line.startsWith( id ) )
        {
            res = line.mid( id.size() ).toInt();
            return true;
        }
	}
	return false;
}

static bool readFloat( QIODevice& in, const QByteArray& id, float& res )
{ 
    in.reset();
    while( !in.atEnd() )
    {
        const QByteArray line = in.readLine().trimmed();
        if( line.startsWith( id ) )
        {
            res = line.mid( id.size() ).toFloat();
            return true;
        }
    }
    return false;
}

static bool readSymbol( QIODevice& in, const QByteArray& id, QByteArray& res )
{ 
    in.reset();
    while( !in.atEnd() )
    {
        const QByteArray line = in.readLine().trimmed();
        if( line.startsWith( id ) )
        {
            res = line.mid( id.size() );
            return true;
        }
    }
    return false;
}

void BrukerSpectrum::readParams(const char * name)
{
    int numOfSamp;
    int tileSize;
    PPM idx0;
    PPM widthHz, widthPpm;
    PPM specFreq;
    int fold;
    int maxY, minY, ncProc;
    int byteSex;	// 0..least significant first, 1..most significant first

	for( int d = 0; d < d_scales.size(); d++ )
	{
        QString filePath;
        if( d == 0 )
            filePath = QString( "%1procs" ).arg( name );
		else
            filePath = QString( "%1proc%2s" ).arg( name ).arg( d + 1 );

        QFile file( filePath );
        if( !file.open(QIODevice::ReadOnly) )
            throw Root::Exception( "Cannot open parameter file: ", filePath.toLatin1() );

        // TODO: Proc-File um Custom-Label erweitern, um Dim-Label bzw. AtomType.
        bool ok = true;
        ok &= readInt( file, "##$SI=", numOfSamp );
        ok &= readInt( file, "##$XDIM=", tileSize );
        ok &= readFloat( file, "##$OFFSET=", idx0 );
        ok &= readFloat( file, "##$SW_p=", widthHz );
        ok &= readFloat( file, "##$SF=", specFreq );
        ok &= readInt( file, "##$MC2=", fold );
		if( d == 0 )
		{
            ok &= readInt( file, "##$YMAX_p=", maxY );
            ok &= readInt( file, "##$YMIN_p=", minY );
            ok &= readInt( file, "##$BYTORDP=", byteSex );
            ok &= readInt( file, "##$NC_proc=", ncProc );
		}
		if( !ok )
            throw Root::Exception( "Error reading parameter file: ", filePath.toLatin1() );

        QByteArray atom;
        QByteArray value;
        if( readSymbol( file, "##$NUC=", value ) )
		{
            if( value == "<1H>" )
				atom = "H";
            else if( value == "<13C>" )
				atom = "C";
            else if( value == "<15N>" )
				atom = "N";
            else
                qWarning() << "BrukerSpectrum::readParams: unknown nucleus" << value;
		}
		d_label[ d ] = "";
        if( readSymbol( file, "##$DIMID=", value ) )
            d_label[ d ] = value;

        if( numOfSamp == 0 )
            throw Root::Exception( "Invalid number of samples in ", filePath.toLatin1() );
		if( specFreq == 0.0 )
            throw Root::Exception( "Invalid spectrometer frequency in ", filePath.toLatin1() );
		if( widthHz == 0.0 )
            throw Root::Exception( "Invalid sweep width in ", filePath.toLatin1() );

		if( widthHz <= 0.0 )
			throw Root::Exception( "Invalid spectral width <= 0.0" );
		if( numOfSamp <= 0.0 )
			throw Root::Exception( "Invalid number of samples <= 0.0" );
		if( specFreq <= 0.0 )
			throw Root::Exception( "Invalid spectrometer frequency <= 0.0" );

		widthPpm = widthHz / specFreq;

		if( true ) // g_useXeasyN ) // 1.9.1: false macht keinen Sinn
		{
			const double delta = widthPpm / double( numOfSamp );
			idx0 -= delta / 2.0;
			widthPpm -= delta;
		}

		d_scales[ d ] = Scale( idx0, idx0 - widthPpm,
			AtomType::parse( atom, idx0 - widthPpm / 2.0 ), Scale::RSH, numOfSamp );
		if( d_scales.size() == 1 )
			tileSize = numOfSamp;	// RISK
		if( tileSize <= 0 || tileSize > numOfSamp )
            throw Root::Exception( "Invalid submatrix size in ", filePath.toLatin1() );

        if( fold == 2 )
			d_scales[ d ].setFolding( Scale::TPPI ); // RISK
		d_tileSize[ d ] = tileSize;
		d_tileCount[ d ] = numOfSamp / tileSize;
		d_rf[ d ] = specFreq;
    }
    d_factor = ::pow( 2.0, ncProc ); // Email Fred 10.8.2014; Update 20.8.2015; Formel korrigiert nach Tests
    // I(true) = I(2rr) x 2^(NC_proc)
    // NC_proc = -3 : data were scaled up (multiplied by 2) three times
    // NC_proc = 4 : the data were scaled down (divided by 2) four times
    d_turnSex = bool(byteSex) != Host::isBigEndian();
    d_levels.d_nMax = minY / s_scf * d_factor; // Email von Fred 2014-08-10; factor korrigiert nach Tests
    d_levels.d_pMax = maxY / s_scf * d_factor;
    if( d_levels.d_nMax < 250.0 )
        d_levels.d_nNoise = -250.0;
    if( d_levels.d_pMax > 250.0 )
        d_levels.d_pNoise = 250.0; // see PhD page 119
}

static Amplitude toAmp( const Root::Byte* data, bool turn, float factor )
{
	// 4-Byte-Integer: -2'147'483'647 .. 2'147'483'647
	// Float: max 3.40282347 e38
    long val = *(long*)data;
	if( turn )
		Host::turnSex( (Root::Byte*) &val, 4 );
    return val / s_scf * factor;
}

Amplitude BrukerSpectrum::convert(const Root::Byte * data) const
{
    return toAmp( data, d_turnSex, d_factor );
}

const char* BrukerSpectrum::getLabel( Dimension d ) const 
{ 
	if( !d_label[ d ].isEmpty() )
		return d_label[ d ].data(); 
	else
        return Spectrum::getLabel( d );
}

Spectrum::Levels BrukerSpectrum::getLevels(bool force) const
{
    // force = true; // TEST
    if( ( d_levels.d_nMax == 0.0 && d_levels.d_pMax == 0.0 ) || force )
    {
        const Levels l = TileSpectrum::getLevels(true);
        // qDebug() << "d_levels.d_pMax" << d_levels.d_pMax << "l.d_pMax" << l.d_pMax;
        return l;
    }else
        return d_levels;
}

