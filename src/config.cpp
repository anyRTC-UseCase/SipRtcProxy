/*
 *  Copyright (c) 2020 The anyRTC project authors. All Rights Reserved.
 *
 *  Website: https://www.anyrtc.io for more details.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "config.h"
#include <stdlib.h> // strtol
#include <limits.h>	// LLONG_MIN LLONG_MAX
#include <math.h>	
#include <stdio.h>
#include <string.h>
#include <errno.h>

using std::map;
using std::string;

ConfigSet::ConfigSet()
{

}

ConfigSet::~ConfigSet()
{
	Clear();
}

void ConfigSet::Clear()
{
	m_cfg.clear();

	// vector< cfg_sect_t * >::iterator mit;
	/*
	   SectItt sit;
	   for( sit = m_cfg.begin(); sit != m_cfg.end(); ++ sit )
	   delete *sit, *sit = NULL;
	   */

	return;
}

int ConfigSet::LoadFromFile( const char * filename, int * errline )
{
	//ASSERT_RET( ( filename != NULL ), -1 );

	m_fname.assign( filename );

	FILE * fp = NULL;
	if( (fp = fopen( filename, "r" )) == NULL ) {
		if( errline != NULL )
			*errline = -1;
		return -1;
	}

	// remove old data
	Clear();

	SectItt sect_it = m_cfg.end();

	char * sect, * key, * value, * p;
	int linenum = 0;
	char buf[4096];

	while( fgets( buf, sizeof(buf), fp ) != NULL ) {
		linenum ++;
		if( errline != NULL )
			*errline = linenum;

		// parse line to get item and key
		p = skip_blank( buf );

		// <1> blank line or common
		if( *p == '#' || *p == '\r' || *p == '\n' || *p == '\0' )
			continue;

		// <2> new section : [ sect_name ]
		if( *p == '[' ) {
			p = skip_blank( p + 1 );
			sect = p;
			while( *p != ']' && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && *p != '\0' )
				p++;
			if( *p == '\r' || *p == '\n' || *p == '\0' )
				return -1;
			// test other word
			if( *p == ' ' || *p == '\t' ) {
				*p = '\0';
				p++;
				while( *p == ' ' || *p == '\t' )
					p++;
				if( *p != ']' )
					return -1;
			}
			else {
				// just ']', remove it;
				*p = '\0';
			}
			// test empty 
			if( *sect == '\0' )
				return -1;

			// create section, just use insert, if exist, it will failed.
			std::pair< SectItt, bool > ins_ret = 
				m_cfg.insert( std::pair< string, map< string, string > >( sect, map< string, string >() ) );

			if( ins_ret.first == m_cfg.end() )
				return -1;

			sect_it = ins_ret.first;

			continue;
		}

		// <3> parse item
		key = p;
		while( *p != '=' && *p != ' ' && *p != '\t' && *p != '\0' && *p != '\r' && *p != '\n' )
			p++;
		if( *p == '\0' || *p == '\r' || *p == '\n' ) {
			return -1;
		}

		if( *p == ' ' || *p == '\t' ) {
			*p = '\0';
			p = skip_blank( p + 1 );
			if( *p != '=' )
				return -1;
		}

		*p = '\0'; // '=', set it to '\0', sometimes not need, but not harm
		p = skip_blank( p + 1 );

		// find value, value can be empty, to override first one
		value = p;

		// if value begin with ["], then we find a match end ["], 
		// FIXME : we don't allow [\"] or other ["] in middle now
		if( *p == '"' ) {
			p++;
			value = p;
			while( *p != '"' && *p != '\0' && *p != '\r' && *p != '\n' )
				p++;
			if( *p != '"' )
				return -1;
			*p = '\0';	
		}
		else {
			p = skip_to_blank( p );
			*p = '\0';
		}

		// if current section not exist, we create an new "default" section
		if( sect_it == m_cfg.end() ) {
			std::pair< SectItt, bool > ins_ret = 
				m_cfg.insert( std::pair< string, map< string, string > >( "default", map< string, string >() ) );

			if( ins_ret.first == m_cfg.end() )
				return -1;

			sect_it = ins_ret.first;
		}

		// insert key/value pair, if exist, overload it
		sect_it->second[ key ] = value;

		continue;
	}

	fclose( fp );
	fp = NULL;

	return 0;
}

char * ConfigSet::skip_blank( char * str )
{
	//ASSERT_RET( ( str != NULL ), NULL );
	char * p = str;
	while( *p == ' ' || *p == '\t' )
		p++;

	return p;
}

char * ConfigSet::skip_to_blank( char * str )
{
	//ASSERT_RET( ( str != NULL ), NULL );
	char * p = str;
	while( *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && *p != '\0' )
		p++;

	return p;
}

int ConfigSet::StoreToFile( const char * filename )
{
	//ASSERT_RET( ( filename != NULL ), -1 );

	FILE * fp = NULL;
	if( (fp = fopen( filename, "w" )) == NULL )
		return -1;

	SectItt sit, sit_end;
	ItemItt mit, mit_end;
	for( sit = m_cfg.begin(), sit_end = m_cfg.end(); sit != sit_end; ++ sit ) {
		fprintf( fp, "[%s]\n", sit->first.c_str() );
		for( mit = sit->second.begin(), mit_end = sit->second.end(); mit != mit_end; ++ mit ) {
			fprintf( fp, "%s = %s\n", mit->first.c_str(), mit->second.c_str() );
		}

		fprintf( fp, "\n" );
	}

	fclose( fp );
	fp = NULL;
	return 0;
}

int ConfigSet::Dump( void )
{
	return StoreToFile( m_fname.c_str() );
}

// get value, return NULL when not found.
const char * ConfigSet::GetValue( const char * sectname, const char * key, const char * default_val ) const
{
	//ASSERT_RET( ( sectname != NULL && key != NULL ), NULL );

	const_SectItt sit = m_cfg.find( sectname );
	if( sit != m_cfg.end() ) {
		const_ItemItt mit = sit->second.find( key );
		if( mit != sit->second.end() )
			return mit->second.c_str();
	}

	return default_val;
}

// set value, return 0 when success;  < 0 when error, 
int ConfigSet::SetValue( const char * sectname, const char * key, const char * value )
{
	//ASSERT_RET( ( sectname != NULL && key != NULL && value != NULL ), -1 );

	m_cfg[sectname][key] = value;

	return 0;
}

// get int (double), ATTENTION!! we return 0 (0.0) when error.
int ConfigSet::GetIntVal( const char * sectname, const char * key, int default_val ) const
{
	//ASSERT_RET( ( sectname != NULL && key != NULL ), -1 );

	const char * value = NULL;


	if( (value = GetValue( sectname, key )) == NULL )
		return default_val;

	char * endptr = NULL;
	int val = 0;

	val = strtol( value, &endptr, 0 );			// accept base of 8, 10 or 16
	if ( val == INT_MIN || val == INT_MAX ) {		// underflow or overflow
		if ( errno == ERANGE )
			return default_val;
	}
	if ( endptr == value || *endptr != '\0' )	// not an integer at all
		return default_val;

	return val;
}

int ConfigSet::SetIntVal( const char * sectname, const char * key, int value )
{
	//ASSERT_RET( ( sectname != NULL && key != NULL ), -1 );

	char buf[256];
#ifdef WIN32
	_snprintf( buf, sizeof(buf), "%d", (int)value );
#else
	snprintf(buf, sizeof(buf), "%d", (int)value);
#endif
	return SetValue( sectname, key, buf );
}

double ConfigSet::GetDblVal( const char * sectname, const char * key, double default_val ) const
{
	//ASSERT_RET( ( sectname != NULL && key != NULL ), 0.0 );

	const char * value = NULL;

	if( (value = GetValue( sectname, key )) == NULL )
		return default_val;

	char * endptr = NULL;
	double val;
	// TODO how big double float can gcc hold?
	val = strtod( value, &endptr );
	// TODO I wrote '==' here, right?
	if ( val == HUGE_VAL || val == -HUGE_VAL ) {
		if ( errno == ERANGE )
			return default_val;
	}
	if ( endptr == value || *endptr != '\0' )
		return default_val;

	return val;
}

int ConfigSet::SetDblVal( const char * sectname, const char * key, double value )
{
	//ASSERT_RET( ( sectname != NULL && key != NULL ), -1 );

	char buf[256];
#ifdef WIN32
	_snprintf(buf, sizeof(buf), "%g", value);
#else
	snprintf(buf, sizeof(buf), "%g", value);
#endif
	return SetValue( sectname, key, buf );
}

double ConfigSet::GetInt64Val( const char * sectname, const char * key, double default_val ) const
{
	//ASSERT_RET( ( sectname != NULL && key != NULL ), -1 );

	const char * value = NULL;

	if( (value = GetValue( sectname, key )) == NULL )
		return default_val;

	char * endptr = NULL;
	double val = 0;

	// NOTE : C99 should use LLONG_MIN and LLONG_MAX, if neither define remove it
	val = strtol( value, &endptr, 0 );			// accept base of 8, 10 or 16
	if (val == LLONG_MIN || val == LLONG_MAX) {		// underflow or overflow
		if ( errno == ERANGE )
			return default_val;
	}
	if ( endptr == value || *endptr != '\0' )	// not an integer at all
		return default_val;

	return val;
}

int ConfigSet::SetInt64Val( const char * sectname, const char * key, double value )
{
	//ASSERT_RET( ( sectname != NULL && key != NULL ), -1 );

	char buf[256];
#ifdef WIN32
	_snprintf(buf, sizeof(buf), "%lld", (long long)value);
#else
	snprintf(buf, sizeof(buf), "%lld", (long long)value);
#endif
	return SetValue( sectname, key, buf );
}

int ConfigSet::DelKey( const char * sectname, const char * key )
{
	//ASSERT_RET( ( sectname != NULL && key != NULL ), -1 );

	SectItt sit = m_cfg.find( sectname );
	if( sit != m_cfg.end() ) {
		ItemItt mit = sit->second.find( key );
		if( mit != sit->second.end() ) {
			sit->second.erase( mit );
			return 0;
		}
	}

	return -1; // not found
}

int ConfigSet::DelSection( const char * sectname )
{
	//ASSERT_RET( ( sectname != NULL ), -1 );

	SectItt sit = m_cfg.find( sectname );
	if( sit != m_cfg.end() ) {
		m_cfg.erase( sit );
		return 0;
	}

	return -1;
}

int ConfigSet::GetSectionNum() const
{
	return (int)m_cfg.size();
}

const char * ConfigSet::GetSectionName( int index ) const
{
	//ASSERT_RET( ( index >= 0 ), NULL );

	int i;
	const_SectItt sit;
	const_SectItt sit_end;
	for( i = 0, sit = m_cfg.begin(), sit_end = m_cfg.end(); sit != sit_end; ++i, ++ sit ) {
		if( i == index )
			return sit->first.c_str();
	}

	return NULL;
}

int ConfigSet::GetSectionKeyNum( const char * sectname ) const
{
	//ASSERT_RET( (sectname != NULL), -1 );

	const_SectItt sit = m_cfg.find( sectname );
	if( sit != m_cfg.end() )
		return (int)sit->second.size();

	return 0;
}

const char * ConfigSet::GetSectionKeyName( const char * sectname, int index ) const
{
	//ASSERT_RET( (sectname != NULL && index >= 0), NULL );

	const_SectItt sit = m_cfg.find( sectname );
	if( sit != m_cfg.end() ) {
		int i;
		const_ItemItt mit;
		const_ItemItt mit_end;
		for( i = 0, mit = sit->second.begin(), mit_end = sit->second.end(); mit != mit_end; ++i, ++ mit ) {
			if( i == index )
				return mit->first.c_str();
		}
	}

	return NULL;
}
