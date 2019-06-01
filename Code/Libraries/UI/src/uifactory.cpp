#include "core.h"
#include "uifactory.h"
#include "configmanager.h"
#include "uiscreens.h"
#include "uiwidgets.h"

static Map<HashedString, UIScreenFactoryFunc>	sScreenFactoryFuncMap;
static Map<HashedString, UIWidgetFactoryFunc>	sWidgetFactoryFuncMap;

void UIFactory::RegisterUIScreenFactory( const HashedString& TypeName, UIScreenFactoryFunc Factory )
{
	ASSERT( sScreenFactoryFuncMap.Search( TypeName ).IsNull() );
	ASSERT( Factory );
	sScreenFactoryFuncMap[ TypeName ] = Factory;
}

void UIFactory::RegisterUIWidgetFactory( const HashedString& TypeName, UIWidgetFactoryFunc Factory )
{
	ASSERT( sWidgetFactoryFuncMap.Search( TypeName ).IsNull() );
	ASSERT( Factory );
	sWidgetFactoryFuncMap[ TypeName ] = Factory;
}

void UIFactory::InitializeBaseFactories()
{
#define ADDUISCREENFACTORY( type ) UIFactory::RegisterUIScreenFactory( #type, UIScreen##type::Factory )
#include "uiscreens.h"
#undef ADDUISCREENFACTORY
#define ADDUIWIDGETFACTORY( type ) UIFactory::RegisterUIWidgetFactory( #type, UIWidget##type::Factory )
#include "uiwidgets.h"
#undef ADDUIWIDGETFACTORY
}

void UIFactory::ShutDown()
{
	sScreenFactoryFuncMap.Clear();
	sWidgetFactoryFuncMap.Clear();
}

UIScreen* UIFactory::CreateScreen( const SimpleString& DefinitionName )
{
	// HACKHACK for Eldritch remaster, redirect definition names
	MAKEHASH( DefinitionName );
	STATICHASH( Rm_Override );
	const SimpleString UsingDefinitionName = ConfigManager::GetString( sRm_Override, DefinitionName.CStr(), sDefinitionName );

	MAKEHASH( UsingDefinitionName );
	STATICHASH( UIScreenType );
	STATIC_HASHED_STRING( Base );
	HashedString UIScreenType = ConfigManager::GetInheritedHash( sUIScreenType, sBase, sUsingDefinitionName );

	Map<HashedString, UIScreenFactoryFunc>::Iterator FactoryIter = sScreenFactoryFuncMap.Search( UIScreenType );
	if( FactoryIter.IsNull() )
	{
		PRINTF( "Invalid type requested for UIScreen %s.\n", UsingDefinitionName.CStr() );
		WARNDESC( "Invalid UIScreen type requested." );
		return NULL;
	}

	UIScreenFactoryFunc pFactory = ( *FactoryIter );
	ASSERT( pFactory );

	UIScreen* const pUIScreen = pFactory();
	ASSERT( pUIScreen );

	pUIScreen->InitializeFromDefinition( UsingDefinitionName );

	return pUIScreen;
}

UIWidget* UIFactory::CreateWidget( const SimpleString& DefinitionName, UIScreen* const pOwnerScreen, UIWidget* const pParentWidget )
{
	if( DefinitionName == "" )
	{
		return NULL;
	}

	// HACKHACK for Eldritch remaster, redirect definition names
	MAKEHASH( DefinitionName );
	STATICHASH( Rm_Override );
	const SimpleString UsingDefinitionName = ConfigManager::GetString( sRm_Override, DefinitionName.CStr(), sDefinitionName );

	MAKEHASH( UsingDefinitionName );

#if BUILD_FINAL
	STATICHASH( DevOnly );
	const bool DevOnly = ConfigManager::GetInheritedBool( sDevOnly, false, sUsingDefinitionName );
	if( DevOnly )
	{
		return NULL;
	}
#endif

	STATICHASH( UIWidgetType );
	HashedString UIWidgetType = ConfigManager::GetInheritedHash( sUIWidgetType, "", sUsingDefinitionName );

	Map<HashedString, UIWidgetFactoryFunc>::Iterator FactoryIter = sWidgetFactoryFuncMap.Search( UIWidgetType );
	if( FactoryIter.IsNull() )
	{
		PRINTF( "Invalid type requested for UIWidget %s.\n", UsingDefinitionName.CStr() );
		WARNDESC( "Invalid UIWidget type requested." );
		return NULL;
	}

	UIWidgetFactoryFunc pFactory = ( *FactoryIter );
	ASSERT( pFactory );

	UIWidget* const pUIWidget = pFactory();
	ASSERT( pUIWidget );

	pUIWidget->SetOwnerScreen( pOwnerScreen );
	pUIWidget->SetParentWidget( pParentWidget );
	pUIWidget->InitializeFromDefinition( UsingDefinitionName, DefinitionName );

	return pUIWidget;
}