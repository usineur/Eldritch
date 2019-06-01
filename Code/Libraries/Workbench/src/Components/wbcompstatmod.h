#ifndef WBCOMPSTATMOD_H
#define WBCOMPSTATMOD_H

#include "wbcomponent.h"
#include "multimap.h"
#include "hashedstring.h"
#include "array.h"

// For use when the static hash already exists
#define WB_MODIFY_FLOAT_X( name, var, statmod )									\
	DEVASSERT( ( statmod ) );													\
	const float name##AutoVar = ( statmod )->ModifyFloat( ( var ), s##name )

#define WB_MODIFY_FLOAT( name, var, statmod )									\
	STATIC_HASHED_STRING( name );												\
	DEVASSERT( ( statmod ) );													\
	const float name##AutoVar = ( statmod )->ModifyFloat( ( var ), s##name )

#define WB_MODIFY_FLOAT_SAFE( name, var, statmod )																\
	STATIC_HASHED_STRING( name );																				\
	WBCompStatMod* const name##StatMod = ( statmod );															\
	const float name##Eval = ( var );																			\
	const float name##AutoVar = name##StatMod ? name##StatMod->ModifyFloat( name##Eval, s##name ) : name##Eval

#define WB_MODDED( name ) name##AutoVar

class WBCompStatMod : public WBComponent
{
public:
	WBCompStatMod();
	virtual ~WBCompStatMod();

	DEFINE_WBCOMP( StatMod, WBComponent );

	virtual int		GetTickOrder() { return ETO_NoTick; }

	virtual void	HandleEvent( const WBEvent& Event );

	void			TriggerEvent( const HashedString& Event );
	void			UnTriggerEvent( const HashedString& Event );
	void			SetEventActive( const HashedString& Event, bool Active );

	float			ModifyFloat( const float Value, const HashedString& StatName );

#if BUILD_DEV
	virtual void	Report() const;
#endif

	virtual uint	GetSerializationSize();
	virtual void	Save( const IDataStream& Stream );
	virtual void	Load( const IDataStream& Stream );

protected:
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	enum EModifierFunction
	{
		EMF_None,
		EMF_Add,
		EMF_Multiply,
		EMF_Set,
	};

	void			AddStatMods( const SimpleString& DefinitionName );

	uint			GetNumActiveEvents() const;
	void			GetActiveEvents( Array<HashedString>& OutEvents ) const;

	static EModifierFunction	GetModifierFunctionFromString( const HashedString& Function );
	static const char*			GetFunctionString( EModifierFunction Function );

	struct SStatModifier
	{
		SStatModifier();

		bool				m_Active;
		HashedString		m_Event;
		HashedString		m_Stat;
		EModifierFunction	m_Function;
		float				m_Value;
	};

	bool									m_Serialize;	// Because I really only want to serialize player stat mods right now
	Multimap<HashedString, SStatModifier>	m_StatModMap;	// Map of stat names to structure, for fastest lookup when modifying value
};

#endif // WBCOMPSTATMOD_H