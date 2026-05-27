//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================


#include "cbase.h"

#ifdef CLIENT_DLL

#include "achievementmgr.h"
#include "baseachievement.h"
#include "tf_hud_statpanel.h"
#include "c_tf_team.h"
#include "c_tf_player.h"
#include "c_tf_playerresource.h"
#include "tf_gamerules.h"
#include "econ_wearable.h"
#include "achievements_tf.h"

// NVNT include for tf2 damage
#include "haptics/haptic_utils.h"

//-----------------------------------------------------------------------------
// Halloween Achievements
//-----------------------------------------------------------------------------

class CAchievementTFHalloweenCollectPumpkins : public CBaseTFAchievementSimple
{
	void Init() 
	{
		SetFlags( ACH_SAVE_GLOBAL );
		SetGoal( 20 );
		SetStoreProgressInSteam( true );
	}

	virtual void ListenForEvents()
	{
		ListenForGameEvent( "halloween_pumpkin_grab" );
	}

	void FireGameEvent_Internal( IGameEvent *event )
	{
		if ( !TFGameRules()->IsHolidayActive( kHoliday_Halloween ) )
			return;

		if ( Q_strcmp( event->GetName(), "halloween_pumpkin_grab" ) == 0 )
		{
			int iPlayer = engine->GetPlayerForUserID( event->GetInt( "userid" ) );
			CBaseEntity *pPlayer = UTIL_PlayerByIndex( iPlayer );

			if ( pPlayer && pPlayer == C_TFPlayer::GetLocalTFPlayer() )
			{
				IncrementCount();
			}
		}
	}
};
DECLARE_ACHIEVEMENT( CAchievementTFHalloweenCollectPumpkins, ACHIEVEMENT_TF_HALLOWEEN_COLLECT_PUMPKINS, "TF_HALLOWEEN_COLLECT_PUMPKINS", 5 );

//----------------------------------------------------------------------------------------------------------------
class CAchievementTFHalloweenDominateForHat : public CBaseTFAchievementSimple
{
	void Init()
	{
		SetFlags( ACH_LISTEN_PLAYER_KILL_ENEMY_EVENTS | ACH_SAVE_GLOBAL );
		SetGoal( 1 );
	}

	virtual void Event_EntityKilled( CBaseEntity *pVictim, CBaseEntity *pAttacker, CBaseEntity *pInflictor, IGameEvent *event ) 
	{
		CTFPlayer *pTFVictim = ToTFPlayer( pVictim );
		bool bDomination = event->GetInt( "death_flags" ) & TF_DEATH_DOMINATION;

		if ( pTFVictim && pAttacker == C_TFPlayer::GetLocalTFPlayer() && bDomination == true )
		{
			// Are they wearing the HAT?
			for ( int i=0; i<pTFVictim->GetNumWearables(); ++i )
			{
				C_EconWearable *pWearable = pTFVictim->GetWearable( i );
				if ( pWearable && pWearable->GetAttributeContainer() )
				{
					CEconItemView *pItem = pWearable->GetAttributeContainer()->GetItem();
					if ( pItem && pItem->IsValid() )
					{
						if ( ( pItem->GetItemDefIndex() == 116 ) ||	// Ghastly Gibus
							 ( pItem->GetItemDefIndex() == 279 ) || 	// Ghastly Gibus 2010 
							 ( pItem->GetItemDefIndex() == 584 ) ||	// Ghastly Gibus 2011
							 ( pItem->GetItemDefIndex() == 940 ) )		// Ghostly Gibus
						{
							IncrementCount();
						}
					}
				}
			}
		}
	}
};
DECLARE_ACHIEVEMENT( CAchievementTFHalloweenDominateForHat, ACHIEVEMENT_TF_HALLOWEEN_DOMINATE_FOR_HAT, "TF_HALLOWEEN_DOMINATE_FOR_HAT", 5 );

//----------------------------------------------------------------------------------------------------------------
class CAchievementTFHalloweenKillScaredPlayer : public CBaseTFAchievementSimple
{
	void Init()
	{
		SetFlags( ACH_LISTEN_PLAYER_KILL_ENEMY_EVENTS | ACH_SAVE_GLOBAL );
		SetGoal( 1 );
	}

	virtual void Event_EntityKilled( CBaseEntity *pVictim, CBaseEntity *pAttacker, CBaseEntity *pInflictor, IGameEvent *event ) 
	{
		CTFPlayer *pTFVictim = ToTFPlayer( pVictim );
		if ( !pTFVictim )
			return;

		CTFPlayer *pLocalPlayer = ToTFPlayer( C_TFPlayer::GetLocalPlayer() );
		if ( !pLocalPlayer )
			return;

		if ( !TFGameRules()->IsHolidayActive( kHoliday_Halloween ) )
			return;

		if ( pVictim == pLocalPlayer )
			return;

		int iStunFlags = event->GetInt( "stun_flags" );
		bool bStunByTrigger = iStunFlags & TF_STUN_BY_TRIGGER;
		if ( bStunByTrigger )
		{
			IncrementCount();
		}
	}
};
DECLARE_ACHIEVEMENT( CAchievementTFHalloweenKillScaredPlayer, ACHIEVEMENT_TF_HALLOWEEN_KILL_SCARED_PLAYER, "TF_HALLOWEEN_KILL_SCARED_PLAYER", 1 );

//----------------------------------------------------------------------------------------------------------------
class CAchievementTFHalloweenPumpkinKill : public CBaseTFAchievementSimple
{
	void Init()
	{
		SetFlags( ACH_LISTEN_PLAYER_KILL_ENEMY_EVENTS | ACH_SAVE_GLOBAL );
		SetGoal( 5 );
		SetStoreProgressInSteam( true );
	}

	virtual void Event_EntityKilled( CBaseEntity *pVictim, CBaseEntity *pAttacker, CBaseEntity *pInflictor, IGameEvent *event ) 
	{
		CTFPlayer *pTFVictim = ToTFPlayer( pVictim );
		if ( !pTFVictim )
			return;

		CTFPlayer *pLocalPlayer = ToTFPlayer( C_TFPlayer::GetLocalPlayer() );
		if ( !pLocalPlayer )
			return;

		if ( pLocalPlayer != pAttacker )
			return;

		if ( !TFGameRules()->IsHolidayActive( kHoliday_Halloween ) )
			return;

		if ( pVictim == pLocalPlayer )
			return;

		int customdmg = event->GetInt( "customkill" );
		if ( customdmg == TF_DMG_CUSTOM_PUMPKIN_BOMB )
		{
			IncrementCount();
		}
	}
};
DECLARE_ACHIEVEMENT( CAchievementTFHalloweenPumpkinKill, ACHIEVEMENT_TF_HALLOWEEN_PUMPKIN_KILL, "TF_HALLOWEEN_PUMPKIN_KILL", 5 );

//----------------------------------------------------------------------------------------------------------------
class CAchievementTFHalloweenDisguisedSpyKill : public CBaseTFAchievementSimple
{
	void Init()
	{
		SetFlags( ACH_LISTEN_PLAYER_KILL_ENEMY_EVENTS | ACH_SAVE_GLOBAL );
		SetGoal( 1 );
	}

	virtual void Event_EntityKilled( CBaseEntity *pVictim, CBaseEntity *pAttacker, CBaseEntity *pInflictor, IGameEvent *event ) 
	{
		CTFPlayer *pTFVictim = ToTFPlayer( pVictim );
		if ( !pTFVictim )
			return;

		CTFPlayer *pTFAttacker = ToTFPlayer( pAttacker );
		if ( !pTFAttacker )
			return;

		CTFPlayer *pLocalPlayer = ToTFPlayer( C_TFPlayer::GetLocalPlayer() );
		if ( !pLocalPlayer )
			return;

		if ( !TFGameRules()->IsHolidayActive( kHoliday_Halloween ) )
			return;

		if ( pVictim == pLocalPlayer )
			return;

		if ( pTFVictim->m_Shared.GetDisguiseClass() == pTFAttacker->GetPlayerClass()->GetClassIndex() )
		{
			IncrementCount();
		}
	}
};
DECLARE_ACHIEVEMENT( CAchievementTFHalloweenDisguisedSpyKill, ACHIEVEMENT_TF_HALLOWEEN_DISGUISED_SPY_KILL, "TF_HALLOWEEN_DISGUISED_SPY_KILL", 5 );

// End of Halloween achievements

#endif // CLIENT_DLL
