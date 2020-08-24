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
#ifndef _CONFIGURE_H_
#define _CONFIGURE_H_

#include <map>
#include <string>

#define INVALID_INTEGER	0
#define INVALID_DOUBLE	0.0
#define NULL_STRING		""

class ConfigSet
{
private:
	ConfigSet( const ConfigSet & c );
	ConfigSet & operator = ( const ConfigSet & c );
		
public:
	ConfigSet();
	~ConfigSet();

	void Clear();

	int LoadFromFile( const char * filename, int * errline = NULL );
	int StoreToFile( const char * filename );
	int Dump( void );

public:
	// basic funxtion
	// get value, return NULL when not found.
	// const char * GetValue( const char * sectname, const char * key ) const;
	const char * GetValue(const char * sectname, const char * key, const char * default_val = NULL_STRING) const;

	// set value, return 0 when success;  < 0 when error, 
	// if section key exist, then change it's value, otherwise add a new one
	int SetValue( const char * sectname, const char * key, const char * value );

	// some help function, 
	// get int (double), ATTENTION!! we return 0 (0.0) when error. use GetValue() to test exist or not
	// FIXME : how to deal with default value ??
	// int32_t GetIntVal( const char * sectname, const char * key ) const;
	int GetIntVal( const char * sectname, const char * key, int default_val = INVALID_INTEGER ) const;
	int SetIntVal( const char * sectname, const char * key, int value );

	// double GetDblVal( const char * sectname, const char * key ) const;
	double GetDblVal( const char * sectname, const char * key, double default_val = INVALID_DOUBLE ) const;
	int SetDblVal( const char * sectname, const char * key, double value );

	// int64_t GetInt64Val( const char * sectname, const char * key ) const;
	double GetInt64Val( const char * sectname, const char * key, double default_val = INVALID_INTEGER ) const;
	int SetInt64Val( const char * sectname, const char * key, double value );

	int DelKey( const char * sectname, const char * key );
	int DelSection( const char * sectname );

public:
	// enum all config value, SLOW func, maybe we need an iterator-like interface
	int GetSectionNum() const;
	const char * GetSectionName( int index ) const;
	int GetSectionKeyNum( const char * sectname ) const;
	const char * GetSectionKeyName( const char * sectname, int index ) const;
	// GetFirstSectionName(), GetNextSectionName() etc...
	
protected:
	static char * skip_blank( char * str );
	static char * skip_to_blank( char * str );

protected:

	std::map< std::string, std::map< std::string, std::string > > m_cfg;

	typedef std::map< std::string, std::map< std::string, std::string > >::iterator SectItt;
	typedef std::map< std::string, std::string >::iterator ItemItt;

	typedef std::map< std::string, std::map< std::string, std::string > >::const_iterator const_SectItt;
	typedef std::map< std::string, std::string >::const_iterator const_ItemItt;
	std::string		m_fname;

};

#endif // _CONFIGURE_H_
