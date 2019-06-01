#ifndef REVERSEHASH_H
#define REVERSEHASH_H

// I don't have a real use case for this yet, but it lets me get strings out
// of anything that I hash at runtime, and that seems potentially useful.

class HashedString;
class SimpleString;

namespace ReverseHash
{
	void			Initialize();
	void			ShutDown();

	bool			IsEnabled();

	void			RegisterHash( const HashedString& Hash, const SimpleString& String );
	SimpleString	ReversedHash( const HashedString& Hash );

	void			ReportSize();
}

#if BUILD_DEV
// For viewing in watch window
const char* ReversedHash( HashedString& Hash );
const char* ReversedHash( uint Hash );
#endif

#endif