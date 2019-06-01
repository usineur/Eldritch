#include "core.h"
#include "eldsaveload.h"
#include "fileutil.h"
#include "zlib.h"
#include "filestream.h"
#include "dynamicmemorystream.h"
#include "eldgame.h"
#include "eldpersistence.h"
#include "eldworld.h"
#include "eldframework.h"
#include "datapipe.h"
#include "memorystream.h"
#include "wbeventmanager.h"

EldSaveLoad::EldSaveLoad()
:	m_WorldFiles()
{
}

EldSaveLoad::~EldSaveLoad()
{
}

SimpleString EldSaveLoad::GetMasterFile() const
{
	const SimpleString SaveLoadPath	= EldFramework::GetInstance()->GetSaveLoadPath();
	const SimpleString MasterFile	= SaveLoadPath + SimpleString( "main.eldritchmastersave" );
	return MasterFile;
}

SimpleString EldSaveLoad::GetLegacyMasterFile() const
{
	const SimpleString LegacySaveLoadPath	= EldFramework::GetInstance()->GetLegacySaveLoadPath();
	const SimpleString LegacyMasterFile		= LegacySaveLoadPath + SimpleString( "main.eldritchmastersave" );
	return LegacyMasterFile;
}

void EldSaveLoad::FlushWorldFiles()
{
	XTRACE_FUNCTION;

	PRINTF( "S/L: Flushing world files:\n" );

	const SimpleString		SaveLoadPath	= EldFramework::GetInstance()->GetSaveLoadPath();

	FOR_EACH_ARRAY( WorldFileIter, m_WorldFiles, SimpleString )
	{
		const SimpleString&	WorldFile		= WorldFileIter.GetValue();
		const SimpleString	FullWorldFile	= SaveLoadPath + WorldFile;

		PRINTF( "     Removing %s\n", FullWorldFile.CStr() );
		FileUtil::RemoveFile( FullWorldFile.CStr() );
	}

	m_WorldFiles.Clear();
}

bool EldSaveLoad::TryLoadMaster()
{
	const SimpleString MasterFile = GetMasterFile();
	const SimpleString UsingMasterFile = FileUtil::Exists( MasterFile.CStr() ) ? MasterFile : GetLegacyMasterFile();
	return TryLoadMaster( UsingMasterFile );
}

void EldSaveLoad::SaveMaster()
{
	SaveMaster( GetMasterFile() );
}

bool EldSaveLoad::TryLoadMaster( const SimpleString& MasterFile )
{
	XTRACE_FUNCTION;

	if( MasterFile == "" )
	{
		return false;
	}

	const bool MasterFileExists = FileUtil::Exists( MasterFile.CStr() );
	if( !MasterFileExists )
	{
		return false;
	}

	FlushWorldFiles();

	PRINTF( "S/L: Restoring master file %s\n", MasterFile.CStr() );
	FileStream MasterFileStream( MasterFile.CStr(), FileStream::EFM_Read );
	const uint UncompressedSize = MasterFileStream.ReadUInt32();
	const uint CompressedSize = MasterFileStream.ReadUInt32();

	Array<byte> CompressedBuffer;
	CompressedBuffer.Resize( CompressedSize );

	Array<byte> UncompressedBuffer;
	UncompressedBuffer.Resize( UncompressedSize );

	MasterFileStream.Read( CompressedSize, CompressedBuffer.GetData() );

	uLong DestinationSize = static_cast<uLong>( UncompressedSize );
	uncompress( UncompressedBuffer.GetData(), &DestinationSize, CompressedBuffer.GetData(), CompressedSize );

	MemoryStream MasterMemoryStream( UncompressedBuffer.GetData(), UncompressedSize );
	return LoadMaster( MasterMemoryStream );
}

void EldSaveLoad::SaveMaster( const SimpleString& MasterFile )
{
	XTRACE_FUNCTION;

	DynamicMemoryStream MasterMemoryStream;
	SaveMaster( MasterMemoryStream );

	const Array<byte>& UncompressedBuffer = MasterMemoryStream.GetArray();
	const uint UncompressedSize = UncompressedBuffer.MemorySize();

	uLong CompressedSize = compressBound( UncompressedSize );
	Array<byte> CompressedBuffer;
	CompressedBuffer.Resize( static_cast<c_uint32>( CompressedSize ) );

	compress( CompressedBuffer.GetData(), &CompressedSize, UncompressedBuffer.GetData(), UncompressedSize );
	CompressedBuffer.Resize( static_cast<c_uint32>( CompressedSize ) );

	PRINTF( "S/L: Creating master file %s\n", MasterFile.CStr() );
	FileStream MasterFileStream( MasterFile.CStr(), FileStream::EFM_Write );
	MasterFileStream.WriteUInt32( UncompressedSize );
	MasterFileStream.WriteUInt32( CompressedSize );
	MasterFileStream.Write( CompressedBuffer.MemorySize(), CompressedBuffer.GetData() );
}

bool EldSaveLoad::TryLoadWorld( const SimpleString& WorldFile )
{
	XTRACE_FUNCTION;

	const SimpleString SaveLoadPath		= EldFramework::GetInstance()->GetSaveLoadPath();
	const SimpleString FullWorldFile	= SaveLoadPath + WorldFile;

	const bool WorldFileExists = FileUtil::Exists( FullWorldFile.CStr() );
	if( !WorldFileExists )
	{
		return false;
	}

	if( !m_WorldFiles.Find( WorldFile ) )
	{
		// If this fails, it means the world file exists but we don't know about it.
		// It's probably stale data from a crashed run. Don't load it.
		return false;
	}

	FileStream WorldFileStream( FullWorldFile.CStr(), FileStream::EFM_Read );
	LoadWorld( WorldFileStream );

	return true;
}

void EldSaveLoad::SaveWorld( const SimpleString& WorldFile )
{
	XTRACE_FUNCTION;

	const SimpleString SaveLoadPath		= EldFramework::GetInstance()->GetSaveLoadPath();
	const SimpleString FullWorldFile	= SaveLoadPath + WorldFile;

	PRINTF( "S/L: Creating world file %s\n", FullWorldFile.CStr() );
	FileStream WorldFileStream( FullWorldFile.CStr(), FileStream::EFM_Write );
	SaveWorld( WorldFileStream );

	m_WorldFiles.PushBackUnique( WorldFile );
}

bool EldSaveLoad::ShouldSaveCurrentWorld() const
{
	// Don't save if the player is dead
	if( !EldGame::IsPlayerAlive() )
	{
		return false;
	}

	// Don't save if we have initiated the ending sequence (or are disabling pause for any other reason)
	if( EldGame::IsPlayerDisablingPause() )
	{
		return false;
	}

	return true;
}

#define MASTER_VERSION_EMPTY		0
#define MASTER_VERSION_PERSISTENCE	1
#define MASTER_VERSION_WORLD		2
#define MASTER_VERSION_WORLDMEMORY	3
#define MASTER_VERSION_CURRENT		3

void EldSaveLoad::SaveMaster( const IDataStream& Stream )
{
	XTRACE_FUNCTION;

	EldFramework* const	pFramework		= EldFramework::GetInstance();
	ASSERT( pFramework );

	EldWorld* const		pWorld			= pFramework->GetWorld();
	ASSERT( pWorld );

	EldGame* const			pGame			= pFramework->GetGame();
	ASSERT( pGame );

	EldPersistence* const	pPersistence	= pGame->GetPersistence();
	ASSERT( pPersistence );

	// Write version
	Stream.WriteUInt32( MASTER_VERSION_CURRENT );

	// Write persistence
	pPersistence->Save( Stream );

	// Write current world
	const bool WorldSaved = ShouldSaveCurrentWorld();
	Stream.WriteBool( WorldSaved );
	if( WorldSaved )
	{
		Stream.WriteString( pGame->GetCurrentLevelName() );
		pWorld->Save( Stream );
	}
	else
	{
		// If we're not saving this world, we shouldn't be saving any other worlds either.
		FlushWorldFiles();
	}

	// Write world memories, bundling loose files
	const SimpleString		SaveLoadPath	= EldFramework::GetInstance()->GetSaveLoadPath();
	Stream.WriteUInt32( m_WorldFiles.Size() );
	FOR_EACH_ARRAY( WorldFileIter, m_WorldFiles, SimpleString )
	{
		const SimpleString& WorldFile		= WorldFileIter.GetValue();
		const SimpleString	FullWorldFile	= SaveLoadPath + WorldFile;

		const bool WorldFileExists = FileUtil::Exists( FullWorldFile.CStr() );
		Stream.WriteBool( WorldFileExists );

		if( WorldFileExists )
		{
			FileStream WorldFileStream( FullWorldFile.CStr(), FileStream::EFM_Read );
			const uint WorldFileSize = WorldFileStream.Size();

			Stream.WriteString( WorldFile );
			Stream.WriteUInt32( WorldFileSize );

			DataPipe WorldFilePipe( WorldFileStream, Stream );
			WorldFilePipe.Pipe( WorldFileSize );
		}
	}
}

bool EldSaveLoad::LoadMaster( const IDataStream& Stream )
{
	XTRACE_FUNCTION;

	ASSERT( m_WorldFiles.Empty() );

	EldFramework* const	pFramework		= EldFramework::GetInstance();
	ASSERT( pFramework );

	EldWorld* const		pWorld			= pFramework->GetWorld();
	ASSERT( pWorld );

	EldGame* const			pGame			= pFramework->GetGame();
	ASSERT( pGame );

	EldPersistence* const	pPersistence	= pGame->GetPersistence();
	ASSERT( pPersistence );

	bool WorldLoaded = false;

	// Read version
	const uint Version = Stream.ReadUInt32();

	// Read persistence
	if( Version >= MASTER_VERSION_PERSISTENCE )
	{
		pPersistence->Load( Stream );
	}

	// Read current world
	if( Version >= MASTER_VERSION_WORLD )
	{
		WorldLoaded = Stream.ReadBool();
		if( WorldLoaded )
		{
			pGame->SetCurrentLevelName( Stream.ReadString() );
			pWorld->Load( Stream );
			pFramework->InitializeTools();
			pGame->RefreshUIReturnToHubEnabled();
		}
	}

	// Read world memories, restore loose files
	PRINTF( "S/L: Restoring world files:\n" );
	const SimpleString		SaveLoadPath	= EldFramework::GetInstance()->GetSaveLoadPath();
	const uint NumWorldFiles = Stream.ReadUInt32();
	for( uint WorldFileIndex = 0; WorldFileIndex < NumWorldFiles; ++WorldFileIndex )
	{
		const bool WorldFileExisted = Stream.ReadBool();
		if( !WorldFileExisted )
		{
			continue;
		}

		const SimpleString	OldWorldFile	= Stream.ReadString();
		const SimpleString	WorldFile		= FileUtil::StripLeadingFolders( OldWorldFile.CStr() );	// ELDRITCH HACK: Trim the leading path (if any) for compatibility.

		const SimpleString	FullWorldFile	= SaveLoadPath + WorldFile;
		const uint WorldFileSize = Stream.ReadUInt32();

		PRINTF( "     Restoring %s\n", FullWorldFile.CStr() );

		FileStream WorldFileStream( FullWorldFile.CStr(), FileStream::EFM_Write );
		DataPipe WorldFilePipe( Stream, WorldFileStream );
		WorldFilePipe.Pipe( WorldFileSize );

		m_WorldFiles.PushBack( WorldFile );
	}

	WB_MAKE_EVENT( OnMasterFileLoaded, NULL );
	WB_DISPATCH_EVENT( WBWorld::GetInstance()->GetEventManager(), OnMasterFileLoaded, NULL );

	return WorldLoaded;
}

#define WORLD_VERSION_EMPTY		0
#define WORLD_VERSION_WORLD		1
#define WORLD_VERSION_CURRENT	1

void EldSaveLoad::SaveWorld( const IDataStream& Stream )
{
	XTRACE_FUNCTION;

	// We assume we should save this world if we've made it this far.
	DEVASSERT( ShouldSaveCurrentWorld() );

	EldFramework* const	pFramework		= EldFramework::GetInstance();
	ASSERT( pFramework );

	EldWorld* const		pWorld			= pFramework->GetWorld();
	ASSERT( pWorld );

	Stream.WriteUInt32( WORLD_VERSION_CURRENT );

	pWorld->Save( Stream );
}

void EldSaveLoad::LoadWorld( const IDataStream& Stream )
{
	XTRACE_FUNCTION;

	EldFramework* const	pFramework		= EldFramework::GetInstance();
	ASSERT( pFramework );

	EldWorld* const		pWorld			= pFramework->GetWorld();
	ASSERT( pWorld );

	pFramework->PrepareForLoad();

	const uint Version = Stream.ReadUInt32();

	if( Version >= WORLD_VERSION_WORLD )
	{
		pWorld->Load( Stream );
		pFramework->InitializeTools();
	}
}