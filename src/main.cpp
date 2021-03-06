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
#include "sort.h"
#include "system.h"

// the global system
System g_System;

extern bool g_bBlenderMode;

// the main func
#ifdef SORT_IN_WINDOWS
int __cdecl main( int argc , char** argv )
#elif defined(SORT_IN_LINUX) || defined(SORT_IN_MAC)
int main( int argc , char** argv )
#endif
{
	// check if there is file argument
	if( argc < 2 )
	{
		cout<<"Miss file argument."<<endl;
		return 0;
	}

	// enable blender mode if possible
	if (argc > 2)
	{
		if (strcmp(argv[2], "blendermode") == 0)
			g_bBlenderMode = true;
	}

	// setup the system
	if( g_System.Setup( argv[1] ) )
	{
		// do ray tracing
		g_System.Render();

		// output rendering time
		cout<<g_System.GetRenderingTime()/1000.0f<<" second"<<endl;

		// output log information
		g_System.OutputLog();
	}

	// unitialize the system
	g_System.Uninit();
	
	return 0;
}
