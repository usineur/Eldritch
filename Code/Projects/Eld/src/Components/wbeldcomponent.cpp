#include "core.h"
#include "wbeldcomponent.h"
#include "eldframework.h"
#include "wbworld.h"

WBEldComponent::WBEldComponent()
{
}

WBEldComponent::~WBEldComponent()
{
}

EldFramework* WBEldComponent::GetFramework() const
{
	return EldFramework::GetInstance();
}

EldGame* WBEldComponent::GetGame() const
{
	return EldFramework::GetInstance()->GetGame();
}

EldWorld* WBEldComponent::GetWorld() const
{
	return EldFramework::GetInstance()->GetWorld();
}
