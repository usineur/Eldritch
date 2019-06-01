#include "core.h"
#include "wbcompreactions.h"
#include "wbpatternmatching.h"
#include "wbaction.h"
#include "configmanager.h"
#include "wbactionfactory.h"
#include "wbrule.h"
#include "wbactionstack.h"
#include "wbeventmanager.h"

WBCompReactions::WBCompReactions()
:	m_Rules()
,	m_Actions()
,	m_DoMultiCompare( false )
,	m_ObserveEvents()
{
}

WBCompReactions::~WBCompReactions()
{
	const uint NumReactions = m_Actions.Size();
	FOR_EACH_INDEX( ReactionsIndex, NumReactions )
	{
		TActions& Actions = m_Actions[ ReactionsIndex ];
		WBActionFactory::ClearActionArray( Actions );
	}

	WBEventManager* const pEventManager = GetEventManager();
	if( pEventManager )
	{
		FOR_EACH_ARRAY( ObserveEventIter, m_ObserveEvents, HashedString )
		{
			const HashedString ObserveEvent = ObserveEventIter.GetValue();
			pEventManager->RemoveObserver( ObserveEvent, this );
		}
	}
}

/*virtual*/ void WBCompReactions::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	Super::InitializeFromDefinition( DefinitionName );

	MAKEHASH( DefinitionName );

	STATICHASH( DoMultiCompare );
	m_DoMultiCompare = ConfigManager::GetInheritedBool( sDoMultiCompare, false, sDefinitionName );

	STATICHASH( NumObserveEvents );
	const uint NumObserveEvents = ConfigManager::GetInheritedInt( sNumObserveEvents, 0, sDefinitionName );
	FOR_EACH_INDEX( ObserveEventIndex, NumObserveEvents )
	{
		const HashedString ObserveEvent = ConfigManager::GetInheritedSequenceHash( "ObserveEvent%d", ObserveEventIndex, HashedString::NullString, sDefinitionName );
		m_ObserveEvents.PushBack( ObserveEvent );
		GetEventManager()->AddObserver( ObserveEvent, this );
	}

	// Add local reactions first
	AddReactions( DefinitionName );

	// Optionally add reactions from other sets (or other reaction components, ignoring their other properties)
	STATICHASH( NumReactionSets );
	const uint NumReactionSets = ConfigManager::GetInheritedInt( sNumReactionSets, 0, sDefinitionName );
	FOR_EACH_INDEX( ReactionSetIndex, NumReactionSets )
	{
		const SimpleString ReactionSet = ConfigManager::GetInheritedSequenceString( "ReactionSet%d", ReactionSetIndex, "", sDefinitionName );
		AddReactions( ReactionSet );
	}
}

void WBCompReactions::AddReactions( const SimpleString& DefinitionName )
{
	MAKEHASH( DefinitionName );

	STATICHASH( NumReactions );
	const uint NumReactions = ConfigManager::GetInheritedInt( sNumReactions, 0, sDefinitionName );

	FOR_EACH_INDEX( ReactionIndex, NumReactions )
	{
		const SimpleString ReactionDef = ConfigManager::GetInheritedSequenceString( "Reaction%d", ReactionIndex, "", sDefinitionName );
		MAKEHASH( ReactionDef );
		ASSERT( ReactionDef != "" );

		STATICHASH( Rule );
		const SimpleString Rule = ConfigManager::GetString( sRule, "", sReactionDef );

#if BUILD_DEV
		if( Rule == "" )
		{
			PRINTF( "Reaction %s has no rule.\n", ReactionDef.CStr() );
			WARN;
		}
#endif

		m_Rules.PushBack().InitializeFromDefinition( Rule );

		TActions& Actions = m_Actions.PushBack();

		WBActionFactory::InitializeActionArray( sReactionDef, Actions );
	}
}

/*virtual*/ void WBCompReactions::HandleEvent( const WBEvent& Event )
{
	XTRACE_FUNCTION;

	Super::HandleEvent( Event );

	// Create a context for pattern matching rules to evaluate their PEs.
	WBParamEvaluator::SPEContext PEContext;
	PEContext.m_Entity = GetEntity();

	if( m_DoMultiCompare )
	{
		Array<uint> MatchingIndices;
		if( WBPatternMatching::MultiCompare( m_Rules, Event, PEContext, MatchingIndices ) )
		{
			const uint NumMatchingIndices = MatchingIndices.Size();
			FOR_EACH_INDEX( MatchingIndexIndex, NumMatchingIndices )
			{
				const uint MatchingIndex = MatchingIndices[ MatchingIndexIndex ];
				ExecuteActions( m_Actions[ MatchingIndex ], Event );
			}
		}
	}
	else
	{
		uint MatchingIndex;
		if( WBPatternMatching::SingleCompare( m_Rules, Event, PEContext, MatchingIndex ) )
		{
			ExecuteActions( m_Actions[ MatchingIndex ], Event );
		}
	}
}

void WBCompReactions::ExecuteActions( const WBCompReactions::TActions& Actions, const WBEvent& ContextEvent ) const
{
	XTRACE_FUNCTION;

	// HACKHACK: Set our entity as a "reactions self" to get around SelfPE bugs.
	WBActionStack::PushReactionsSelf( GetEntity() );

	WBActionFactory::ExecuteActionArray( Actions, ContextEvent, GetEntity() );

	// HACKHACK
	WBActionStack::PopReactionsSelf();
}