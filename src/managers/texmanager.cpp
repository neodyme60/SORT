/*
    This file is a part of SORT(Simple Open Ray Tracing), an open-source cross
    platform physically based renderer.
 
    Copyright (c) 2011-2016 by Cao Jiayin - All rights reserved.
 
    SORT is a free software written for educational purpose. Anyone can distribute
    or modify it under the the terms of the GNU General Public License Version 3 as
    published by the Free Software Foundation. However, there is NO warranty that
    all components are functional in a perfect manner. Without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.
 
    You should have received a copy of the GNU General Public License along with
    this program. If not, see <http://www.gnu.org/licenses/gpl-3.0.html>.
 */

// include header file
#include "texmanager.h"
#include "texio/bmpio.h"
#include "texio/exrio.h"
#include "texio/tgaio.h"
#include "texio/pngio.h"
#include "texio/jpgio.h"
#include "texio/hdrio.h"
#include "texture/imagetexture.h"
#include "utility/strhelper.h"
#include "utility/define.h"
#include "utility/path.h"
#include "texture/checkboxtexture.h"
#include "texture/gridtexture.h"
#include "texture/constanttexture.h"
#include "texture/imagetexture.h"

// instance the singleton with tex manager
DEFINE_SINGLETON(TexManager);

// destructor
TexManager::~TexManager()
{
	_release();
}

// default constructor
TexManager::TexManager()
{
	_init();
}

// initialize data
void TexManager::_init()
{
	// push the texture outputer
	m_TexIOVec.push_back( new BmpIO() );
	m_TexIOVec.push_back( new ExrIO() );
	m_TexIOVec.push_back( new TgaIO() );
    m_TexIOVec.push_back( new PngIO() );
	m_TexIOVec.push_back( new JpgIO() );
	m_TexIOVec.push_back( new HdrIO() );
}

// release data
void TexManager::_release()
{
	// release texture outputer
	vector<TexIO*>::const_iterator tex_it = m_TexIOVec.begin();
	while( tex_it != m_TexIOVec.end() )
	{
		delete *tex_it;
		tex_it++;
	}
	m_TexIOVec.clear();

	map< std::string , ImgMemory* >::iterator img_it = m_ImgContainer.begin();
	while( img_it != m_ImgContainer.end() )
	{
		if( img_it->second->reference > 0 )
		{
			string isare = (img_it->second->reference>1)?"is":"are";
			string refer = (img_it->second->reference>1)?" reference":" references";
			LOG_ERROR<<"There "<<isare<<" still "<<(int)(img_it->second->reference)<<refer<<" pointing to \""<<img_it->first<<"\"."<<CRASH;
		}
		SAFE_DELETE_ARRAY( img_it->second->m_ImgMem );
		img_it->second->m_iWidth = 0;
		img_it->second->m_iHeight = 0;

		// delete the memory
		delete img_it->second;

		img_it++;
	}
	m_ImgContainer.clear();
}

// output texture
bool TexManager::Write( const string& filename , const Texture* tex )
{
	// get full path name
	string str = GetFullPath( filename );

	// get the type
	TEX_TYPE type = TexTypeFromStr( str );

	// find the specific texio first
	TexIO* io = FindTexIO( type );

	if( io != 0 )
		io->Write( str , tex );
	
	return true;
}

// load the image from file , if the specific image is already existed in the current system , just return the pointer
bool TexManager::Read( const string& filename , ImageTexture* tex )
{
	// get full path name
	string str = filename;

	// get the type
	TEX_TYPE type = TexTypeFromStr( str );

	// try to find the image first , if it's already existed in the system , just set a pointer
	map< std::string , ImgMemory* >::iterator it = m_ImgContainer.find( str );
	if( it != m_ImgContainer.end() )
	{
		tex->m_pMemory = it->second;
		tex->m_iTexWidth = it->second->m_iWidth;
		tex->m_iTexHeight = it->second->m_iHeight;

		return true;
	}

	// find the specific texio first
	TexIO* io = FindTexIO( type );

	bool read = false;
	if( io != 0 )
	{
		// create a new memory
		ImgMemory* mem = new ImgMemory();

		// read the data
		read = io->Read( str , mem );

		if( read )
		{
			// set the texture
			tex->m_pMemory = mem;
			tex->m_iTexWidth = mem->m_iWidth;
			tex->m_iTexHeight = mem->m_iHeight;
			
			// insert it into the container
			m_ImgContainer.insert( make_pair( str , mem ) );
		}else
		{
			LOG_WARNING<<"Can't load image file \""<<str<<"\"."<<ENDL;
			delete mem;
		}
	}

	return read;
}

// find correct texio
TexIO* TexManager::FindTexIO( TEX_TYPE tt ) const
{
	// find the specific texio first
	TexIO* io = 0;
	vector<TexIO*>::const_iterator it = m_TexIOVec.begin();
	while( it != m_TexIOVec.end() )
	{
		if( (*it)->GetTT() == tt )
		{
			io = *it;
			break;
		}
		it++;
	}

	return io;
}

// get the reference count
unsigned TexManager::GetReferenceCount( const string& str ) const
{
	// try to find the image first , if it's already existed in the system , just set a pointer
	map< std::string , ImgMemory* >::const_iterator it = m_ImgContainer.find( str );
	if( it != m_ImgContainer.end() )
	{
		return it->second->reference;
	}

	return 0;
}
