/*
 * filename :	texmanager.h
 *
 * programmer :	Cao Jiayin
 */

#ifndef	SORT_TEXMANAGER
#define	SORT_TEXMANAGER

// include header file
#include "../../utility/singleton.h"
#include "../../utility/enum.h"
#include <vector>
#include <map>
#include "../../spectrum/spectrum.h"

class Texture;
class ImageTexture;
class TexIO;

struct ImgMemory
{
	Spectrum*	m_ImgMem;
	unsigned	m_iWidth;
	unsigned	m_iHeight;
};

//////////////////////////////////////////////////////////////////
//	defination of texture manager
class TexManager : public Singleton<TexManager>
{
// public method
public:
	// create TexManager
	static void CreateTexManager();

	// destructor
	~TexManager();

	// output the img
	// para 'str' :	name of the output entity
	// para 'tex' : texture of the output
	// para 'type': the way we output the texture
	// result     : 'true' if the texture is output successfully
	bool Write( const string& str , const Texture* tex , TEX_TYPE type );

	// load the image from file , if the specific image is already existed in the current system , just return the pointer
	// para 'str'  : name of the image file
	// para 'tex'  : output to the texture
	// para 'type' : the type of the image file
	// result      : 'true' if loading is successful
	bool Read( const string& str , ImageTexture* tex , TEX_TYPE type );

// private data
private:
	// a vector saving texture io
	vector<TexIO*>	m_TexIOVec;

	// map a string to the image memory
	map< std::string , ImgMemory* > m_ImgContainer;

// private method
private:
	// private default constructor
	TexManager();

	// initialize texture manager data
	void _init();

	// release texture manager data
	void _release();

	// find correct texio
	TexIO*	FindTexIO( TEX_TYPE tt );
};

#endif