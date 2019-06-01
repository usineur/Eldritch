#ifndef ELDFACTIONS_H
#define ELDFACTIONS_H

class HashedString;

namespace EldFactions
{
	enum EFactionCon
	{
		EFR_Hostile,
		EFR_Neutral,
		EFR_Friendly,
	};

	// For managing statically allocated memory
	void		AddRef();
	void		Release();

	EFactionCon	GetCon( const HashedString& FactionA, const HashedString& FactionB );
}

#endif // ELDFACTIONS_H
