#include "core.h"
#include "uiinputmapeld.h"
#include "eldframework.h"
#include "keyboard.h"
#include "xinputcontroller.h"
#include "inputsystem.h"

UIInputMapEld::UIInputMapEld( EldFramework* const pFramework )
:	m_Framework( pFramework )
{
}

UIInputMapEld::~UIInputMapEld()
{
}

bool UIInputMapEld::OnNext()
{
	Keyboard* const pKeyboard = m_Framework->GetKeyboard();

	if( pKeyboard->OnRise( Keyboard::EB_Tab ) &&
		!( pKeyboard->IsHigh( Keyboard::EB_LeftControl ) ||
		   pKeyboard->IsHigh( Keyboard::EB_RightControl ) ) &&
		!( pKeyboard->IsHigh( Keyboard::EB_LeftShift ) ||
		   pKeyboard->IsHigh( Keyboard::EB_RightShift ) ) )
	{
		return true;
	}

	return false;
}

bool UIInputMapEld::OnPrevious()
{
	Keyboard* const pKeyboard = m_Framework->GetKeyboard();

	if( pKeyboard->OnRise( Keyboard::EB_Tab ) &&
		( ( pKeyboard->IsHigh( Keyboard::EB_LeftControl ) ||
			pKeyboard->IsHigh( Keyboard::EB_RightControl ) ) ||
		  ( pKeyboard->IsHigh( Keyboard::EB_LeftShift ) ||
			pKeyboard->IsHigh( Keyboard::EB_RightShift ) ) ) )
	{
		return true;
	}

	return false;
}

bool UIInputMapEld::OnUp()
{
	Keyboard* const pKeyboard = m_Framework->GetKeyboard();
	XInputController* const pController = m_Framework->GetController();
	InputSystem* const pInputSystem = m_Framework->GetInputSystem();

	STATIC_HASHED_STRING( Forward );
	const uint Keyboard_Forward = pInputSystem->GetBoundKeyboardSignal( sForward );

	if( Keyboard_Forward > Keyboard::EB_None && pKeyboard->OnRise( Keyboard_Forward ) )
	{
		return true;
	}

	if( pKeyboard->OnRise( Keyboard::EB_Up ) )
	{
		return true;
	}

	if( pController->OnRise( XInputController::EB_Up ) ||
		pController->OnRise( XInputController::EB_LeftThumbUp ) ||
		pController->OnRise( XInputController::EB_RightThumbUp ) )
	{
		return true;
	}

	return false;
}

bool UIInputMapEld::OnDown()
{
	Keyboard* const pKeyboard = m_Framework->GetKeyboard();
	XInputController* const pController = m_Framework->GetController();
	InputSystem* const pInputSystem = m_Framework->GetInputSystem();

	STATIC_HASHED_STRING( Back );
	const uint Keyboard_Back = pInputSystem->GetBoundKeyboardSignal( sBack );

	if( Keyboard_Back > Keyboard::EB_None && pKeyboard->OnRise( Keyboard_Back ) )
	{
		return true;
	}

	if( pKeyboard->OnRise( Keyboard::EB_Down ) )
	{
		return true;
	}

	if( pController->OnRise( XInputController::EB_Down ) ||
		pController->OnRise( XInputController::EB_LeftThumbDown ) ||
		pController->OnRise( XInputController::EB_RightThumbDown ) )
	{
		return true;
	}

	return false;
}

bool UIInputMapEld::OnLeft()
{
	Keyboard* const pKeyboard = m_Framework->GetKeyboard();
	XInputController* const pController = m_Framework->GetController();
	InputSystem* const pInputSystem = m_Framework->GetInputSystem();

	STATIC_HASHED_STRING( Left );
	const uint Keyboard_Left = pInputSystem->GetBoundKeyboardSignal( sLeft );

	if( Keyboard_Left > Keyboard::EB_None && pKeyboard->OnRise( Keyboard_Left ) )
	{
		return true;
	}

	if( pKeyboard->OnRise( Keyboard::EB_Left ) )
	{
		return true;
	}

	if( pController->OnRise( XInputController::EB_Left ) ||
		pController->OnRise( XInputController::EB_LeftThumbLeft ) ||
		pController->OnRise( XInputController::EB_RightThumbLeft ) )
	{
		return true;
	}

	return false;
}

bool UIInputMapEld::OnRight()
{
	Keyboard* const pKeyboard = m_Framework->GetKeyboard();
	XInputController* const pController = m_Framework->GetController();
	InputSystem* const pInputSystem = m_Framework->GetInputSystem();

	STATIC_HASHED_STRING( Right );
	const uint Keyboard_Right = pInputSystem->GetBoundKeyboardSignal( sRight );

	if( Keyboard_Right > Keyboard::EB_None && pKeyboard->OnRise( Keyboard_Right ) )
	{
		return true;
	}

	if( pKeyboard->OnRise( Keyboard::EB_Right ) )
	{
		return true;
	}

	if( pController->OnRise( XInputController::EB_Right ) ||
		pController->OnRise( XInputController::EB_LeftThumbRight ) ||
		pController->OnRise( XInputController::EB_RightThumbRight ) )
	{
		return true;
	}

	return false;
}

/*virtual*/ bool UIInputMapEld::OnPrevPage()
{
	Keyboard* const			pKeyboard		= m_Framework->GetKeyboard();
	XInputController* const	pController		= m_Framework->GetController();
	InputSystem* const		pInputSystem	= m_Framework->GetInputSystem();

	STATIC_HASHED_STRING( LeanLeft );
	const uint Keyboard_LeanLeft = pInputSystem->GetBoundKeyboardSignal( sLeanLeft );
	if( Keyboard_LeanLeft != Keyboard::EB_None && pKeyboard->OnRise( Keyboard_LeanLeft ) )
	{
		return true;
	}

	const uint Controller_LeanLeft = pInputSystem->GetBoundControllerSignal( sLeanLeft );
	if( Controller_LeanLeft > XInputController::EB_None && pController->OnRise( Controller_LeanLeft ) )
	{
		return true;
	}

	return false;
}

/*virtual*/ bool UIInputMapEld::OnNextPage()
{
	Keyboard* const			pKeyboard		= m_Framework->GetKeyboard();
	XInputController* const	pController		= m_Framework->GetController();
	InputSystem* const		pInputSystem	= m_Framework->GetInputSystem();

	STATIC_HASHED_STRING( LeanRight );
	const uint Keyboard_LeanRight = pInputSystem->GetBoundKeyboardSignal( sLeanRight );
	if( Keyboard_LeanRight != Keyboard::EB_None && pKeyboard->OnRise( Keyboard_LeanRight ) )
	{
		return true;
	}

	const uint Controller_LeanRight = pInputSystem->GetBoundControllerSignal( sLeanRight );
	if( Controller_LeanRight > XInputController::EB_None && pController->OnRise( Controller_LeanRight ) )
	{
		return true;
	}

	return false;
}

bool UIInputMapEld::OnAccept()
{
	Keyboard* const pKeyboard = m_Framework->GetKeyboard();
	XInputController* const pController = m_Framework->GetController();

	if( pKeyboard->OnRise( Keyboard::EB_Enter ) &&
		!( pKeyboard->IsHigh( Keyboard::EB_LeftAlt ) ||
		   pKeyboard->IsHigh( Keyboard::EB_RightAlt ) ) )
	{
		return true;
	}

	if( pController->OnRise( XInputController::EB_A ) || pController->OnRise( XInputController::EB_Start ) )
	{
		return true;
	}

	return false;
}

bool UIInputMapEld::OnCancel()
{
	Keyboard* const pKeyboard = m_Framework->GetKeyboard();
	XInputController* const pController = m_Framework->GetController();
	InputSystem* const pInputSystem = m_Framework->GetInputSystem();

	STATIC_HASHED_STRING( Frob );
	const uint Keyboard_Frob = pInputSystem->GetBoundKeyboardSignal( sFrob );
	if( Keyboard_Frob > Keyboard::EB_None && pKeyboard->OnRise( Keyboard_Frob ) )
	{
		return true;
	}

	if( pKeyboard->OnRise( Keyboard::EB_Escape ) )
	{
		return true;
	}

	const uint Controller_Frob = pInputSystem->GetBoundControllerSignal( sFrob );
	if( Controller_Frob > XInputController::EB_None && pController->OnRise( Controller_Frob ) )
	{
		return true;
	}

	if( pController->OnRise( XInputController::EB_B ) )
	{
		return true;
	}

	return false;
}
