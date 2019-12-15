#include "core.h"
#include "eldframework.h"
#include "filestream.h"
#include "configmanager.h"
#include "stringmanager.h"
#include "allocator.h"

#if BUILD_WINDOWS
#include <Windows.h>
#include <crtdbg.h>
#endif

#if BUILD_SDL
#include "SDL2/SDL.h"
#endif

#if BUILD_MAC
#include "fileutil.h"
#include "objcjunk.h"
#include <CoreFoundation/CoreFoundation.h>
#endif

// Can easily disable my memory manager here if needed
#define ELD_USE_ALLOCATOR ( 1 && USE_ALLOCATOR )

#if BUILD_MAC
// TODO: Fix PrintManager to just keep a SimpleString so I don't have to do this gross hack.
void Initialize( SimpleString& LogFilename )
#else
void Initialize()
#endif
{
#if ELD_USE_ALLOCATOR
	// Default allocator must be initialized before creating any objects.
	// It should be large enough that I won't need to worry about fragmentation.
	// NOTE: Be aware that 32- and 64-bit builds may have different memory patterns
	// because of pointer size shifting small allocations into different buckets!
	{
		static const uint kMegabyte		= 1024 * 1024;
		static const uint kNumChunks	= 8;
		Allocator::SChunkDef ChunkDefs[ kNumChunks ];

		ChunkDefs[0].m_AllocSize = 16;
		ChunkDefs[0].m_ChunkSize = 12 * kMegabyte;

		ChunkDefs[1].m_AllocSize = 32;
		ChunkDefs[1].m_ChunkSize = 32 * kMegabyte;

		ChunkDefs[2].m_AllocSize = 64;
		ChunkDefs[2].m_ChunkSize = 24 * kMegabyte;

		ChunkDefs[3].m_AllocSize = 128;
		ChunkDefs[3].m_ChunkSize = 20 * kMegabyte;

		ChunkDefs[4].m_AllocSize = 256;
		ChunkDefs[4].m_ChunkSize = 8 * kMegabyte;

		ChunkDefs[5].m_AllocSize = 512;
		ChunkDefs[5].m_ChunkSize = 8 * kMegabyte;

		ChunkDefs[6].m_AllocSize = 1024;
		ChunkDefs[6].m_ChunkSize = 12 * kMegabyte;

		ChunkDefs[7].m_AllocSize = 0;
		ChunkDefs[7].m_ChunkSize = 96 * kMegabyte;

		// Total: 212MB

		Allocator::Enable( true );
		Allocator::GetDefault().Initialize( kNumChunks, ChunkDefs );
	}
#endif

#if BUILD_WINDOWS
	ExceptionHandler::Enable();
#endif

#if BUILD_MAC
	// Change working directory to the Resources path in the .app folder structure
	CFBundleRef MainBundle = CFBundleGetMainBundle();
	CFURLRef ResourceURL = CFBundleCopyResourcesDirectoryURL( MainBundle );
	char ResourcePath[ PATH_MAX ];
	CFURLGetFileSystemRepresentation( ResourceURL, TRUE, reinterpret_cast<UInt8*>( ResourcePath ), PATH_MAX );
	CFRelease( ResourceURL );
	chdir( ResourcePath );
#endif

#if BUILD_MAC
	// HACKHACK: Hard-coded so string table *can't* override this!
	// See also EldFramework::GetUserDataPath()
	const SimpleString UserPathAppName = "Eldritch Reanimated";

	LogFilename = ObjCJunk::GetUserDirectory( UserPathAppName ) + SimpleString( "eld-log.txt" );
	PrintManager::GetInstance()->LogTo( LogFilename.CStr() );
#else
	// Do the default thing and emit log file in working directory.
	PRINTLOGS( eld );
#endif
}

void ShutDown()
{
#if BUILD_WINDOWS
	ExceptionHandler::ShutDown();
#endif

#if ELD_USE_ALLOCATOR
	if( Allocator::IsEnabled() )
	{
#if BUILD_DEV
		Allocator::GetDefault().Report( FileStream( "memory_exit_report.txt", FileStream::EFM_Write ) );
#endif

		ASSERT( Allocator::GetDefault().CheckForLeaks() );
		Allocator::GetDefault().ShutDown();
	}
#endif

#if BUILD_WINDOWS
	DEBUGASSERT( _CrtCheckMemory() );
	DEBUGASSERT( !_CrtDumpMemoryLeaks() );
#endif
}

#if BUILD_WINDOWS_NO_SDL
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#elif BUILD_SDL
extern "C" int main( int argc, char* argv[] )
#endif
{
#if BUILD_SWITCH
	socketInitializeDefault();
	nxlinkStdio();
#endif
#if BUILD_WINDOWS_NO_SDL
	Unused( hPrevInstance );
	Unused( lpCmdLine );
#elif BUILD_SDL
	Unused( argc );
	Unused( argv );
#endif

#if BUILD_MAC
    // TODO: Fix this gross hack.
    // Keep log filename in scope
    SimpleString LogFilename;
    Initialize( LogFilename );
#else
	Initialize();
#endif

	EldFramework* pFramework = new EldFramework;
#if BUILD_WINDOWS_NO_SDL
	pFramework->SetInitializeParameters( hInstance, nCmdShow );
#endif
	pFramework->Main();
	SafeDelete( pFramework );

	ShutDown();

#if BUILD_SWITCH
	SDL_Quit();
	socketExit();
#endif

	return 0;
}
