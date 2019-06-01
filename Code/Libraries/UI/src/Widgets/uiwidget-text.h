#ifndef UIWIDGETTEXT_H
#define UIWIDGETTEXT_H

#include "uiwidget.h"
#include "vector2.h"
#include "simplestring.h"

class Font;
class ITexture;
class Mesh;

class UIWidgetText : public UIWidget
{
public:
	UIWidgetText();
	UIWidgetText( const SimpleString& DefinitionName );
	virtual ~UIWidgetText();

	DEFINE_UIWIDGET_FACTORY( Text );

	virtual void	Render( bool HasFocus );
	virtual void	UpdateRender();
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName, const SimpleString& LegacyName );
	virtual void	GetBounds( SRect& OutBounds );
	virtual float	GetWidth();
	virtual float	GetHeight();
	virtual void	Refresh();
	virtual void	Tick( const float DeltaTime );

	Mesh*			GetMesh()			{ return m_Mesh; }
	Mesh*			GetDropShadowMesh()	{ return m_DropShadowMesh; }

	void			UpdatePosition();
	void			UpdateRenderPosition();
	void			GetFontPrintFlags( const HashedString& Alignment );

	uint			GetOriginalNumIndices() const { return m_OriginalNumIndices; }

	Mesh*			CreateShadowBoxMesh() const;

	Font*			m_Font;
	bool			m_IsLiteral;			// Won't look up in string table; only use for numbers!!
	bool			m_IsDynamicPosition;	// Reupdate the position every tick for dynamic strings
	SimpleString	m_String;
	SimpleString	m_DynamicString;
	Mesh*			m_Mesh;
	SimpleString	m_Material;
	uint			m_FontPrintFlags;
	uint			m_TextColor;
	float			m_Scale;				// Scale of displayed font relative to size used in font sheet (new for Eld)
	float			m_WrapWidth;
	uint			m_OriginalNumIndices;

	bool			m_HasDropShadow;
	Vector2			m_DropShadowOffset;
	Vector4			m_DropShadowColor;
	Mesh*			m_DropShadowMesh;

	bool			m_HasShadowBox;
	ITexture*		m_ShadowBoxTexture;
	float			m_ShadowBoxBorder;
	float			m_ShadowBoxMargin;
	Mesh*			m_ShadowBoxMesh;
};

#endif // UIWIDGETTEXT_H