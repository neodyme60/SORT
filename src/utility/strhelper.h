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

#ifndef	SORT_STRHELPER
#define	SORT_STRHELPER

#include "sort.h"
#include "enum.h"
#include "math/transform.h"
#include "spectrum/spectrum.h"

// pre-decleration
struct VertexIndex;

////////////////////////////////////////////////////////////////////////
// there are some global functions for parsing string

// convert string to vertex index
// para 'str' : the string to parse
// result     : a vertex index
VertexIndex	VertexIndexFromStr( const string& str );

// get the type of file according to the file extension
// para 'str' : string to parse
// result     : a cooresponding mesh type
MESH_TYPE	MeshTypeFromStr( const string& str );

// get the type of image file from file extension
// para 'str' : string to parse
// result     : a cooresponding mesh type
TEX_TYPE	TexTypeFromStr( const string& str );

class ToLower{
public:
     char operator()(char val){   
        return tolower( val );
    }
};

// transform from string
Transform TransformFromStr( const string& str );

// spectrum from string
Spectrum SpectrumFromStr( const string& str );

// point from string
Point PointFromStr( const string& str );

// direction from string
Vector VectorFromStr( const string& str );

// get the next token
string NextToken( string& str , char t );

#endif
