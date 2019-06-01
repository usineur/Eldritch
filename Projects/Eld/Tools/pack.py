import os
import sys

MOVEPACKAGES	= True
VERBOSE			= True
PACKTESTASSETS	= True #False

# Assets
BAKED			= '../Baked/'
BAKEDCONTENT	= '../BakedContent'
TEXTURES		= 'Textures/'
MESHES			= 'Meshes/'
FONTS			= 'Fonts/'
SHADERS			= 'Shaders/'
CONFIG			= 'Config/'
AUDIO			= 'Audio/'
ROOMS			= 'Rooms/'
MISC			= 'Misc/'

BASEPACKFILE		= 'eldritch-base.cpk'
TEXTURESPACKFILE	= 'eldritch-textures.cpk'
MESHESPACKFILE		= 'eldritch-meshes.cpk'
AUDIOPACKFILE		= 'eldritch-audio.cpk'
WORLDPACKFILE		= 'eldritch-world.cpk'

# Map from folder names to package files
DLCPACKFILES = {	'DLC1' : 'dlc1.cpk',
					'DLC2' : 'dlc2.cpk' }

# Tools folders
CYGWIN_BIN	= 'C:/cygwin64/bin/'	# Updated for Karras
TOOLS_DIR	= '../Tools/'	# Relative to Baked directory

# Tools
FILE_PACKER		= TOOLS_DIR + 'FilePacker.exe'
COPY			= CYGWIN_BIN + 'cp'
MOVE			= CYGWIN_BIN + 'mv'
PACKAGESTORE	= MOVE if MOVEPACKAGES else COPY

#-----------------------------------------------------
def pack():
	if PACKTESTASSETS:
		print 'ALERT: Packing test assets!'

	packdir( CONFIG,	True,	'.ccf',				BASEPACKFILE )
	packdir( CONFIG,	True,	'.pcf',				BASEPACKFILE )
	packdir( FONTS,		True,	'.fnp',				BASEPACKFILE )
	packdir( MESHES,	True,	'.cms',				MESHESPACKFILE )
	packdir( SHADERS,	True,	'.cfx',				BASEPACKFILE )
	packdir( SHADERS,	True,	'.chv2',			BASEPACKFILE )
	packdir( SHADERS,	True,	'.chp2',			BASEPACKFILE )
	packdir( SHADERS,	True,	'.gv12',			BASEPACKFILE )
	packdir( SHADERS,	True,	'.gf12',			BASEPACKFILE )
	packdir( TEXTURES,	True,	'.dds',				TEXTURESPACKFILE )
	packdir( TEXTURES,	True,	'.bmp',				TEXTURESPACKFILE )
	packdir( TEXTURES,	True,	'.tga',				TEXTURESPACKFILE )
	packdir( ROOMS,		True,	'.eldritchroom',	WORLDPACKFILE )
	packdir( AUDIO,		True,	'.wav',				AUDIOPACKFILE )
	packdir( AUDIO,		False,	'.ogg',				AUDIOPACKFILE )	# Don't compress .ogg files, so we can stream direct from pack file.
	packdir( MISC,		True,	'.bmp',				BASEPACKFILE )

	storefiles( '.cpk', PACKAGESTORE )
	storefiles( '.html', COPY )
	storefiles( '.txt', COPY )

	# These aren't distributed with the game, but are stored in BakedContent for versioning with each shipping build number
	# (These should be the PDBs for the Steam build, since the intent is to use with Steam minidumps)
	storefiles( '.pdb', COPY )

#-----------------------------------------------------
def runtool( args ):
	if VERBOSE:
		for arg in args:
			print arg,
		print
	os.spawnv( os.P_WAIT, args[0], args )

#-----------------------------------------------------
def storefiles( ext, storecmd ):
	for path, dirs, files in os.walk( '.' ):
		del dirs[:]	# Don't recurse into any folders

		for file in files:
			if( ext in file ):
				storefile( file, storecmd )

#-----------------------------------------------------
def storefile( packfile, storecmd ):
	copypackfile = os.path.join( BAKEDCONTENT, packfile ).replace( '\\', '/' )
	runtool( [ storecmd, packfile, copypackfile ] )

#-----------------------------------------------------
# If ext is specified, only files matching that extension are baked
# If ext isn't specified, all files in the folder are baked
# This will recurse into any subfolders of the given path
def packdir( dir, compress, ext, packfile ):
	for path, dirs, files in os.walk( dir ):

		# Ignore source control and test content
		if '.svn' in dirs:
			dirs.remove( '.svn' )

		# Ignore test content if we're not building a test package
		if( ( not PACKTESTASSETS ) and ( 'Test' in dirs ) ):
			dirs.remove( 'Test' )

		usepackfile = packfile

		# Override package file for DLC
		for dlcdir, dlcpackfile in DLCPACKFILES.iteritems():
			if dlcdir in path:
				usepackfile = dlcpackfile

		for file in files:
			if( ( not ext ) or ( ext in file ) ):
				infile = os.path.join( path, file )
				compressflag = ''
				if compress:
					compressflag = '-c'
				# NOTE: Switched the order of parameters from the way FilePacker used to work!
				runtool( [ FILE_PACKER, usepackfile, infile, compressflag ] )

#-----------------------------------------------------
# Entry point
#-----------------------------------------------------

for arg in sys.argv:
	if arg == '-t':
		PACKTESTASSETS = True

print 'Deleting pack files...'

os.chdir( BAKED )

if os.path.exists( BASEPACKFILE ):
	os.remove( BASEPACKFILE )

if os.path.exists( TEXTURESPACKFILE ):
	os.remove( TEXTURESPACKFILE )

if os.path.exists( MESHESPACKFILE ):
	os.remove( MESHESPACKFILE )

if os.path.exists( AUDIOPACKFILE ):
	os.remove( AUDIOPACKFILE )

if os.path.exists( WORLDPACKFILE ):
	os.remove( WORLDPACKFILE )

for dlcdir, dlcpackfile in DLCPACKFILES.iteritems():
	if os.path.exists( dlcpackfile ):
		os.remove( dlcpackfile )

print 'Packing assets...'

try:
	pack()
except:
	sys.exit(1)
	
print 'Packing done!'