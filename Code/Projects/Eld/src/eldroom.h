#ifndef ELDROOM_H
#define ELDROOM_H

// Small (hopefully) class that contains a random room definition.

#include "eldworld.h"	// For voxel types

class IDataStream;

class EldRoom
{
public:
	EldRoom();
	~EldRoom();

	struct SSpawner
	{
		SSpawner()
		:	m_SpawnerDef()
		,	m_Location( 0 )
		,	m_Orientation( 0 )
		{
		}

		SimpleString	m_SpawnerDef;
		vidx_t			m_Location;
		c_uint8			m_Orientation;	// See ESpawnerOrientation
	};

	void	Save( const IDataStream& Stream );
	void	Load( const IDataStream& Stream );

	void					GetRoomScalars( uint& X, uint& Y, uint& Z ) const;
	const Array<vval_t>&	GetVoxelMap() const { return m_VoxelMap; }
	const Array<SSpawner>&	GetSpawners() const { return m_Spawners; }

private:
	friend class EldTools;

	uint			m_RoomScalarX;
	uint			m_RoomScalarY;
	uint			m_RoomScalarZ;
	Array<vval_t>	m_VoxelMap;

	Array<SSpawner>	m_Spawners;
};

#endif // ELDROOM_H