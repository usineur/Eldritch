#ifndef WBPEELDGETPLAYTIME_H
#define WBPEELDGETPLAYTIME_H

#include "wbpe.h"

class WBPEEldGetPlayTime : public WBPE
{
public:
	WBPEEldGetPlayTime();
	virtual ~WBPEEldGetPlayTime();

	DEFINE_WBPE_FACTORY( EldGetPlayTime );

	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual void	Evaluate( const WBParamEvaluator::SPEContext& Context, WBParamEvaluator::SEvaluatedParam& EvaluatedParam ) const;

private:
	bool	m_Minutes;
	bool	m_Seconds;
};

#endif // WBPEELDGETPLAYTIME_H
