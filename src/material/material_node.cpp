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

#include "material_node.h"
#include "managers/memmanager.h"
#include "bsdf/bsdf.h"
#include "bsdf/lambert.h"
#include "thirdparty/tinyxml/tinyxml.h"
#include "geometry/intersection.h"

// get node property
MaterialNodeProperty* MaterialNode::getProperty( const string& name )
{
	auto it = m_props.find( name );
    return ( it != m_props.end() )? it->second : nullptr;
}

// set node property
void MaterialNodeProperty::SetNodeProperty( const string& prop )
{
	string str = prop;
	string x = NextToken( str , ' ' );
	string y = NextToken( str , ' ' );
	string z = NextToken( str , ' ' );
	string w = NextToken( str , ' ' );

	value.x = (float)atof( x.c_str() );
	value.y = (float)atof( y.c_str() );
	value.z = (float)atof( z.c_str() );
	value.w = (float)atof( w.c_str() );
}

// get node property
MaterialPropertyValue MaterialNodeProperty::GetPropertyValue( Bsdf* bsdf )
{
	if( node )
		return node->GetNodeValue( bsdf );
	return value;
}

// update bsdf, for layered brdf
void MaterialNodeProperty::UpdateBsdf( Bsdf* bsdf , Spectrum weight ){
    if( node )
        node->UpdateBSDF( bsdf , weight );
}

// set node property
void MaterialNodePropertyString::SetNodeProperty( const string& prop )
{
	str = prop;
}

// parse property or socket
void MaterialNode::ParseProperty( TiXmlElement* element , MaterialNode* node )
{
	TiXmlElement* prop = element->FirstChildElement( "Property" );
	while(prop)
	{
		string prop_name = prop->Attribute( "name" );
		string prop_type = prop->Attribute( "type" );

		MaterialNodeProperty* node_prop = node->getProperty( prop_name );
		if( node_prop == 0 )
		{
			// output error log
			LOG_WARNING<<"Node property "<<prop_name<<" is ignored."<<ENDL;

			// get next property
			prop = prop->NextSiblingElement( "Property" );

			// proceed to the next property
			continue;
		}

		// add socket input
		if( prop_type == "node" )
		{
			node_prop->node = ParseNode( prop , node );
		}else
		{
			string node_value = prop->Attribute( "value" );
			node_prop->SetNodeProperty( node_value );
		}
			
		// get next property
		prop = prop->NextSiblingElement( "Property" );
	}
}

// parse a new node
MaterialNode* MaterialNode::ParseNode( TiXmlElement* element , MaterialNode* node )
{
	string node_type = element->Attribute( "node" );
	// create new material node
	MaterialNode* mat_node = CREATE_TYPE( node_type , MaterialNode );
	if( mat_node == 0 )
	{
		LOG_WARNING<<"Node type "<<node_type<<" is unknown!!"<<ENDL;
		return mat_node;
	}

	// parse node properties
	ParseProperty( element , mat_node );

	return mat_node; 
}

// check validation
bool MaterialNode::CheckValidation()
{
	// get subtree node type
	getNodeType();

	m_node_valid = true;
    for( auto it : m_props ){
		if( it.second->node )
			m_node_valid &= it.second->node->CheckValidation();
	}

	return m_node_valid;
}

// get sub tree node type
MAT_NODE_TYPE MaterialNode::getNodeType()
{
	MAT_NODE_TYPE type = MAT_NODE_NONE;

	map< string , MaterialNodeProperty* >::const_iterator it = m_props.begin();
	while( it != m_props.end() )
	{
		if( it->second->node )
			type |= it->second->node->getNodeType();
		++it;
	}

	// setup sub-tree type
	subtree_node_type = type;

	return type;
}

// post process
void MaterialNode::PostProcess()
{
	if( m_post_processed )
		return;

	map< string , MaterialNodeProperty* >::const_iterator it = m_props.begin();
	while( it != m_props.end() )
	{
		if( it->second->node )
			it->second->node->PostProcess();
		++it;
	}

	m_post_processed = true;
}

// update bsdf
void MaterialNode::UpdateBSDF( Bsdf* bsdf , Spectrum weight )
{
	if( weight.IsBlack() )
		return;

	map< string , MaterialNodeProperty* >::const_iterator it = m_props.begin();
	while( it != m_props.end() )
	{
		if( it->second->node )
			it->second->node->UpdateBSDF(bsdf , weight);
		++it;
	}
}

MaterialNode::~MaterialNode()
{
	map< string , MaterialNodeProperty* >::const_iterator it = m_props.begin();
	while( it != m_props.end() )
	{
		delete it->second->node;
		++it;
	}
}

OutputNode::OutputNode()
{
	// register node property
	m_props.insert( make_pair( "Surface" , &output ) );
}

// update bsdf
void OutputNode::UpdateBSDF( Bsdf* bsdf , Spectrum weight )
{
	// return a default one for invalid material
	if( !m_node_valid )
	{
		Spectrum default_spectrum( 0.3f , 0.0f , 0.0f );
		Lambert* lambert = SORT_MALLOC(Lambert)( default_spectrum );
		lambert->m_weight = weight;
		bsdf->AddBxdf( lambert );
		return;
	}

	if( output.node )
		output.node->UpdateBSDF( bsdf );
}

// check validation
bool OutputNode::CheckValidation()
{
	// it is invalid if there is no node attached
	if( output.node == 0 )
		return false;

	// get node type
	MAT_NODE_TYPE type = output.node->getNodeType();

	// make sure there is bxdf attached !!
	if( ( output.node == 0 ) || !(type & MAT_NODE_BXDF) )
	{
		m_node_valid = false;
		return false;
	}

	m_node_valid = MaterialNode::CheckValidation();

	return m_node_valid;
}
