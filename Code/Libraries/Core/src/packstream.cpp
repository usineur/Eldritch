#include "core.h"
#include "packstream.h"

#include "fileutil.h"
#include "filestream.h"
#include "memorystream.h"

#include "zlib.h"

Array<SPackageFile> PackStream::sm_PackageFiles;

SPackageFileHeader::SPackageFileHeader()
:	m_MagicID( 0 )
,	m_NumFiles( 0 )
,	m_OffsetToFiles( 0 )
,	m_FilesSize( 0 )
{
}

SPackageFileEntry::SPackageFileEntry()
:	m_Filename( "" )
,	m_OffsetToFile( 0 )
,	m_CompressedSize( 0 )
,	m_DecompressedSize( 0 )
,	m_Compressed( 0 )
{
}

SPackageFile::SPackageFile()
:	m_Filename( NULL )
,	m_Stream( NULL )
,	m_Header()
,	m_Entries()
{
}

PackStream::PackStream( const char* Filename )
:	m_Filename( Filename )
,	m_PackageFile( NULL )
,	m_FileEntry( NULL )
,	m_FileStream( NULL )
,	m_MemoryStream( NULL )
,	m_PackageFileBuffer( NULL )
,	m_PackedMode( false )
{
#if BUILD_DEV
	// In dev builds ONLY, try opening the loose file first.
	if( FileUtil::Exists( Filename ) )
	{
		m_FileStream = new FileStream( Filename, FileStream::EFM_Read );
		return;
	}
#endif

	if( FindFileEntry( Filename, m_PackageFile, m_FileEntry ) )
	{
		m_PackedMode	= true;

		byte* CompressedBuffer = new byte[ m_FileEntry->m_CompressedSize ];

		m_PackageFile->m_Stream->SetPos( m_PackageFile->m_Header.m_OffsetToFiles + m_FileEntry->m_OffsetToFile );
		m_PackageFile->m_Stream->Read( m_FileEntry->m_CompressedSize, CompressedBuffer );

		if( m_FileEntry->m_Compressed )
		{
			m_PackageFileBuffer = new byte[ m_FileEntry->m_DecompressedSize ];
			uLong DecompressedSize = static_cast<uLong>( m_FileEntry->m_DecompressedSize );
			uncompress( m_PackageFileBuffer, &DecompressedSize, CompressedBuffer, m_FileEntry->m_CompressedSize );
			m_FileEntry->m_DecompressedSize = static_cast<c_uint32>( DecompressedSize );
			SafeDeleteArray( CompressedBuffer );
		}
		else
		{
			m_PackageFileBuffer = CompressedBuffer;
		}

		m_MemoryStream = new MemoryStream( m_PackageFileBuffer, m_FileEntry->m_DecompressedSize );

		return;
	}

	// If the package file doesn't exist or the file
	// wasn't found, try opening the file on disk
	if( !m_MemoryStream )
	{
		m_FileStream = new FileStream( Filename, FileStream::EFM_Read );
	}
}

PackStream::~PackStream()
{
	SafeDelete( m_FileStream );
	SafeDelete( m_MemoryStream );
	SafeDeleteArray( m_PackageFileBuffer );
}

bool PackStream::FindFileEntry( const char* Filename, SPackageFile*& pOutPackageFile, SPackageFileEntry*& pOutFileEntry )
{
	PROFILE_SCOPE( MultiPackageFileLookup );

	for( uint PackageFileIndex = 0; PackageFileIndex < sm_PackageFiles.Size(); ++PackageFileIndex )
	{
		SPackageFile& PackageFile = sm_PackageFiles[ PackageFileIndex ];

		for( uint PackedFileIndex = 0; PackedFileIndex < PackageFile.m_Entries.Size(); ++PackedFileIndex )
		{
			SPackageFileEntry* const pFileEntry = PackageFile.m_Entries[ PackedFileIndex ];
			if( FileUtil::Compare( pFileEntry->m_Filename.CStr(), Filename ) )
			{
				pOutPackageFile	= &PackageFile;
				pOutFileEntry	= pFileEntry;
				return true;
			}
		}
	}

	return false;
}

/*static*/ void PackStream::StaticAddPackageFile( const char* PackageFilename, const bool PreemptExistingPackages /*= false*/ )
{
	// Ignore if we're re-adding a package file that's already opened.
	// (It shouldn't be possible to modify an open package, so there's
	// no reason to try hotloading them.)
	for( uint PackageFileIndex = 0; PackageFileIndex < sm_PackageFiles.Size(); ++PackageFileIndex )
	{
		SPackageFile& PackageFile = sm_PackageFiles[ PackageFileIndex ];
		if( *PackageFile.m_Filename == PackageFilename )
		{
			return;
		}
	}

	if( PackageFilename && FileUtil::Exists( PackageFilename ) )
	{
		const uint InsertIndex = PreemptExistingPackages ? 0 : sm_PackageFiles.Size();
		sm_PackageFiles.Insert( SPackageFile(), InsertIndex );

		SPackageFile& NewPackageFile = PreemptExistingPackages ? sm_PackageFiles.First() : sm_PackageFiles.Last();

		NewPackageFile.m_Filename = new SimpleString( PackageFilename );

		NewPackageFile.m_Stream = new FileStream( PackageFilename, FileStream::EFM_Read );

		NewPackageFile.m_Stream->Read( sizeof( SPackageFileHeader ), &NewPackageFile.m_Header );
		ASSERT( NewPackageFile.m_Header.m_MagicID == 'KPCD' );

		// If not a valid package file, early out (this makes it a bit
		// safer to make the package file a config var).
		if( NewPackageFile.m_Header.m_MagicID != 'KPCD' )
		{
			WARN;
			SafeDelete( NewPackageFile.m_Stream );
			sm_PackageFiles.PopBack();
			return;
		}

		for( uint i = 0; i < NewPackageFile.m_Header.m_NumFiles; ++i )
		{
			SPackageFileEntry* FileEntry = new SPackageFileEntry;

			FileEntry->m_Filename			= NewPackageFile.m_Stream->ReadString();
			FileEntry->m_OffsetToFile		= NewPackageFile.m_Stream->ReadUInt32();
			FileEntry->m_CompressedSize		= NewPackageFile.m_Stream->ReadUInt32();
			FileEntry->m_DecompressedSize	= NewPackageFile.m_Stream->ReadUInt32();
			FileEntry->m_Compressed			= NewPackageFile.m_Stream->ReadUInt32();

			NewPackageFile.m_Entries.PushBack( FileEntry );
		}
	}
}

/*static*/ void PackStream::StaticShutDown()
{
	for( uint PackageFileIndex = 0; PackageFileIndex < sm_PackageFiles.Size(); ++PackageFileIndex )
	{
		SPackageFile& PackageFile = sm_PackageFiles[ PackageFileIndex ];
		SafeDelete( PackageFile.m_Filename );
		SafeDelete( PackageFile.m_Stream );
		for( uint PackedFileIndex = 0; PackedFileIndex < PackageFile.m_Entries.Size(); ++PackedFileIndex )
		{
			SafeDelete( PackageFile.m_Entries[ PackedFileIndex ] );
		}
		PackageFile.m_Entries.Clear();
	}
	sm_PackageFiles.Clear();
}

/*static*/ bool PackStream::StaticFileExists( const char* Filename )
{
	for( uint PackageFileIndex = 0; PackageFileIndex < sm_PackageFiles.Size(); ++PackageFileIndex )
	{
		SPackageFile& PackageFile = sm_PackageFiles[ PackageFileIndex ];
		if( PackageFile.m_Stream )
		{
			for( uint PackedFileIndex = 0; PackedFileIndex < PackageFile.m_Entries.Size(); ++PackedFileIndex )
			{
				SPackageFileEntry* FileEntry = PackageFile.m_Entries[ PackedFileIndex ];
				if( FileUtil::Compare( FileEntry->m_Filename.CStr(), Filename ) )
				{
					return true;
				}
			}
		}
	}
	return FileUtil::Exists( Filename );
}

void PackStream::GetFilesInFolderInternal( const SimpleString& Path, const SimpleString& Package, const SimpleString& Extension, Array<SimpleString>& OutFiles )
{
	XTRACE_FUNCTION;

	const bool IgnorePackage	= ( Package == "" );
	const bool IgnorePath		= ( Path == "." || Path == "" || Path == "./" );
	const bool IgnoreExtension	= ( Extension == "" );

	FOR_EACH_ARRAY( PackageFileIter, sm_PackageFiles, SPackageFile )
	{
		const SPackageFile& PackageFile = PackageFileIter.GetValue();

		// Skip files not in our specified package
		if( !IgnorePackage && *PackageFile.m_Filename != Package )
		{
			continue;
		}

		FOR_EACH_ARRAY( FileEntryIter, PackageFile.m_Entries, SPackageFileEntry* )
		{
			const SPackageFileEntry* const pFileEntry = FileEntryIter.GetValue();
			DEVASSERT( pFileEntry );

			// Skip files outside the specified path
			if( !IgnorePath && !pFileEntry->m_Filename.BeginsWith( Path ) )
			{
				continue;
			}

			// Skip files without specified extension
			if( !IgnoreExtension && !pFileEntry->m_Filename.EndsWith( Extension ) )
			{
				continue;
			}

			OutFiles.PushBack( pFileEntry->m_Filename );
		}
	}
}

/*static*/ void	PackStream::StaticGetFilesInFolder( const SimpleString& Path, const SimpleString& Package, const SimpleString& Extension, Array<SimpleString>& OutFiles )
{
	XTRACE_FUNCTION;

	GetFilesInFolderInternal( FileUtil::NormalizeDirectoryPath( Path.CStr() ), Package, Extension, OutFiles );
}

/*static*/ void PackStream::StaticGetFilesInFolder_Master( const SimpleString& Path, const bool Recursive, const SimpleString& Extension, Array<SimpleString>& OutFiles )
{
	XTRACE_FUNCTION;

	StaticGetFilesInFolder( Path, "", Extension, OutFiles );
	FileStream::StaticGetFilesInFolder( Path, Recursive, Extension, OutFiles );
}

int PackStream::Read( int NumBytes, void* Buffer ) const
{
	if( m_PackedMode )
	{
		return m_MemoryStream->Read( NumBytes, Buffer );
	}
	else
	{
		return m_FileStream->Read( NumBytes, Buffer );
	}
}

int	PackStream::Write( int NumBytes, const void* Buffer ) const
{
	WARNDESC( "Can't write to a PackStream" );
	Unused( NumBytes );
	Unused( Buffer );
	return 0;
}

int PackStream::PrintF( const char* Str, ... ) const
{
	WARNDESC( "Can't write to a PackStream" );
	Unused( Str );
	return 0;
}

int PackStream::SetPos( int Position ) const
{
	if( m_PackedMode )
	{
		return m_MemoryStream->SetPos( Position );
	}
	else
	{
		return m_FileStream->SetPos( Position );
	}
}

int	PackStream::GetPos() const
{
	if( m_PackedMode )
	{
		return m_MemoryStream->GetPos();
	}
	else
	{
		return m_FileStream->GetPos();
	}
}

int PackStream::EOS() const
{
	if( m_PackedMode )
	{
		return m_MemoryStream->EOS();
	}
	else
	{
		return m_FileStream->EOS();
	}
}

int PackStream::Size() const
{
	if( m_PackedMode )
	{
		return m_MemoryStream->Size();
	}
	else
	{
		return m_FileStream->Size();
	}
}

const char* PackStream::GetPhysicalFilename() const
{
	if( m_PackedMode )
	{
		return m_PackageFile->m_Filename->CStr();
	}
	else
	{
		return m_FileStream->GetQualifiedFilename().CStr();
	}
}

uint PackStream::GetSubfileOffset() const
{
	if( m_PackedMode )
	{
		return m_PackageFile->m_Header.m_OffsetToFiles + m_FileEntry->m_OffsetToFile;
	}
	else
	{
		return 0;
	}
}
uint PackStream::GetSubfileLength() const
{
	if( m_PackedMode )
	{
		return m_FileEntry->m_CompressedSize;
	}
	else
	{
		return m_FileStream->Size();
	}
}