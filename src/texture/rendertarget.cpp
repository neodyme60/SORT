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

// include the header file
#include "rendertarget.h"
#include "managers/logmanager.h"

// set the color
void RenderTarget::SetColor( int x , int y , float r , float g , float b )
{
	// check if there is memory
	if( m_pData == 0 )
		LOG_ERROR<<"There is no data in the render target , can't set color."<<CRASH;

	// use filter first
	_texCoordFilter( x , y );

	// get the offset
	unsigned offset = y * m_iTexWidth + x;

	// set the color
	m_pData[offset].SetColor( r , g , b );
}


