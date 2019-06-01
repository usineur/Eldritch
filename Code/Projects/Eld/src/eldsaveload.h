#ifndef ELDRITCHSAVELOAD_H
#define ELDRITCHSAVELOAD_H

#include "array.h"
#include "simplestring.h"

class EldGame;
class IDataStream;

class EldSaveLoad
{
public:
	EldSaveLoad();
	~EldSaveLoad();

	void			FlushWorldFiles();

	bool			TryLoadMaster();
	void			SaveMaster();

	bool			TryLoadMaster( const SimpleString& MasterFile );
	void			SaveMaster( const SimpleString& MasterFile );

	bool			TryLoadWorld( const SimpleString& WorldFile );
	void			SaveWorld( const SimpleString& WorldFile );

	bool			ShouldSaveCurrentWorld() const;

private:
	SimpleString	GetMasterFile() const;
	SimpleString	GetLegacyMasterFile() const;

	void			SaveMaster( const IDataStream& Stream );
	bool			LoadMaster( const IDataStream& Stream );

	void			SaveWorld( const IDataStream& Stream );
	void			LoadWorld( const IDataStream& Stream );

	Array<SimpleString>	m_WorldFiles;	// Names of current loose world files
};

#endif // ELDRITCHSAVELOAD_H