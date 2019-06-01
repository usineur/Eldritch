#ifndef ELDMESH_H
#define ELDMESH_H

#include "mesh.h"
#include "eldirradiance.h"
#include "wbentity.h"

class EldMesh : public Mesh
{
public:
	EldMesh();
	virtual ~EldMesh();

	void					SetIrradianceCube( const SVoxelIrradiance& Irradiance );
	const SVoxelIrradiance&	GetIrradianceCube() const;

	void					SetEntity( WBEntity* const pEntity ) { m_Entity = pEntity; }
	WBEntity*				GetEntity() const { return m_Entity; }

private:
	SVoxelIrradiance	m_IrradianceCube;
	WBEntity*			m_Entity;
};

#endif // ELDMESH_H
