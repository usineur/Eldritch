#include "core.h"
#include "wbpegetconfigvar.h"
#include "configmanager.h"

WBPEGetConfigVar::WBPEGetConfigVar()
:	m_VarContext()
,	m_VarContextPE()
,	m_VarName()
,	m_VarNamePE()
,	m_DefaultPE()
{
}

WBPEGetConfigVar::~WBPEGetConfigVar()
{
}

/*virtual*/ void WBPEGetConfigVar::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	MAKEHASH( DefinitionName );

	STATICHASH( VarContext );
	m_VarContext = ConfigManager::GetHash( sVarContext, HashedString::NullString, sDefinitionName );

	STATICHASH( VarContextPE );
	const SimpleString VarContextPEDef = ConfigManager::GetString( sVarContextPE, "", sDefinitionName );
	m_VarContextPE.InitializeFromDefinition( VarContextPEDef );

	STATICHASH( VarName );
	m_VarName = ConfigManager::GetHash( sVarName, HashedString::NullString, sDefinitionName );

	STATICHASH( VarNamePE );
	const SimpleString VarNamePEDef = ConfigManager::GetString( sVarNamePE, "", sDefinitionName );
	m_VarNamePE.InitializeFromDefinition( VarNamePEDef );

	STATICHASH( DefaultPE );
	m_DefaultPE.InitializeFromDefinition( ConfigManager::GetString( sDefaultPE, "", sDefinitionName ) );
}

/*virtual*/ void WBPEGetConfigVar::Evaluate( const WBParamEvaluator::SPEContext& Context, WBParamEvaluator::SEvaluatedParam& EvaluatedParam ) const
{
	m_VarContextPE.Evaluate( Context );
	const HashedString	VarContext	= m_VarContextPE.HasRoot() ? m_VarContextPE.GetString() : m_VarContext;

	m_VarNamePE.Evaluate( Context );
	const HashedString	VarName		= m_VarNamePE.HasRoot() ? m_VarNamePE.GetString() : m_VarName;

	m_DefaultPE.Evaluate( Context );

	const ConfigVar::EVarType VarType = ConfigManager::GetType( VarName, VarContext );
	if( ConfigVar::EVT_Bool == VarType )
	{
		EvaluatedParam.m_Type	= WBParamEvaluator::EPT_Bool;
		EvaluatedParam.m_Bool	= ConfigManager::GetBool(	VarName, m_DefaultPE.GetBool(),				VarContext );
	}
	else if( ConfigVar::EVT_Int == VarType )
	{
		EvaluatedParam.m_Type	= WBParamEvaluator::EPT_Int;
		EvaluatedParam.m_Int	= ConfigManager::GetInt(	VarName, m_DefaultPE.GetInt(),				VarContext );
	}
	else if( ConfigVar::EVT_Float == VarType )
	{
		EvaluatedParam.m_Type	= WBParamEvaluator::EPT_Float;
		EvaluatedParam.m_Float	= ConfigManager::GetFloat(	VarName, m_DefaultPE.GetFloat(),			VarContext );
	}
	else if( ConfigVar::EVT_String == VarType )
	{
		EvaluatedParam.m_Type	= WBParamEvaluator::EPT_String;
		EvaluatedParam.m_String	= ConfigManager::GetString(	VarName, m_DefaultPE.GetString().CStr(),	VarContext );
	}
}