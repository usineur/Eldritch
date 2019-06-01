#include "core.h"
#include "eldmesh.h"
#include "eldworld.h"
#include "eldframework.h"

EldMesh::EldMesh()
:	m_IrradianceCube()
,	m_Entity( NULL )
{
}

EldMesh::~EldMesh()
{
}

void EldMesh::SetIrradianceCube( const SVoxelIrradiance& Irradiance )
{
	m_IrradianceCube = Irradiance + EldFramework::GetInstance()->GetWorld()->GetGlobalLight();
}

const SVoxelIrradiance& EldMesh::GetIrradianceCube() const
{
	return m_IrradianceCube;
}
