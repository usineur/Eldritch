#ifndef ANIMATION_H
#define ANIMATION_H

#include "array.h"
#include "hashedstring.h"
#include "vector.h"
#include "angles.h"

#define ANIM_FRAMERATE		30.0f
#define ANIM_NAME_LENGTH	32

class AnimEvent;
class SimpleString;

// This is the part of Animation that can be saved in resource files
struct SAnimData
{
	SAnimData()
	:	m_HashedName()
	,	m_StartFrame( 0 )
	,	m_Length( 0 )
	{
		m_Name[0] = '\0';
	}

	char				m_Name[ ANIM_NAME_LENGTH ];	// Only needed for matching entries in animations.config anymore
	HashedString		m_HashedName;				// Used to get animation by name
	c_uint16			m_StartFrame;				// In bone array's frame list
	c_uint16			m_Length;					// In frames
};

struct SAnimDataPadded
{
	SAnimData			m_AnimData;

	// HACKHACK: Padding to match how Eldritch animations were originally serialized
	// Hopefully this doesn't need to be aligned or anything
	c_uint32			m_PADDING_Array_ArrayPtr;
	c_uint32			m_PADDING_Array_Size;
	c_uint32			m_PADDING_Array_Capacity;
	c_uint32			m_PADDING_Array_Reserved;
	bool				m_PADDING_Array_Deflate;
	c_uint32			m_PADDING_Array_AllocatorPtr;
	float				m_PADDING_Vector_X;
	float				m_PADDING_Vector_Y;
	float				m_PADDING_Vector_Z;
	float				m_PADDING_Angles_Pitch;
	float				m_PADDING_Angles_Roll;
	float				m_PADDING_Angles_Yaw;
};

class Animation
{
public:
	Animation();
	~Animation();

	void				InitializeFromDefinition( const SimpleString& QualifiedAnimationName );

	uint				GetLengthFrames() const;
	float				GetLengthSeconds() const;
	float				GetNonLoopingLengthSeconds() const;

	void				GetVelocity( Vector& OutVelocity, Angles& OutRotationalVelocity ) const;
	Vector				GetDisplacement( float Time ) const;

	SAnimData			m_AnimData;
	Array< AnimEvent* >	m_AnimEvents;

	// Cheap substitute for root motion
	Vector				m_Velocity;
	Angles				m_RotationalVelocity;
};

#endif // ANIMATION_H
