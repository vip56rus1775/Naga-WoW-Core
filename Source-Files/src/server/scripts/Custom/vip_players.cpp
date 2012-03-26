#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "MapManager.h"
#include "Chat.h"
#include "Common.h"
#include "DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ArenaTeamMgr.h"
#include "GuildMgr.h"
#include "AuctionHouseMgr.h"
#include "AccountMgr.h"
#include "PlayerDump.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Opcodes.h"
#include "GameObject.h"
#include "Log.h"
#include "Guild.h"
#include "ObjectAccessor.h"
#include "Language.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "Weather.h"
#include "PointMovementGenerator.h"
#include "TargetedMovementGenerator.h"
#include "SkillDiscovery.h"
#include "SkillExtraItems.h"
#include "SystemConfig.h"
#include "Config.h"
#include "Util.h"
#include "ItemEnchantmentMgr.h"
#include "BattlegroundMgr.h"
#include "InstanceSaveMgr.h"
#include "InstanceScript.h"
#include "CreatureEventAIMgr.h"
#include "SpellAuraEffects.h"
#include "DBCEnums.h"
#include "ConditionMgr.h"
#include "DisableMgr.h"
#include "Transport.h"
#include "WeatherMgr.h"
#include "ScriptMgr.h"
#include "CreatureTextMgr.h"
#include "SmartAI.h"
#include "Group.h"
#include "ChannelMgr.h"

class vipcommands : public CommandScript
{
public:
    vipcommands() : CommandScript("vipcommands") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand vipCommandTable[] =

        {
            { "mall",	    SEC_PLAYER,     true, &HandleVipMallCommand,         "", NULL },
            { "chat",	    SEC_PLAYER,     true, &HandleVipChatCommand,         "", NULL },
            { "changerace",    SEC_PLAYER,  false, &HandleChangeRaceCommand,             "", NULL },
	    { "changefaction",	SEC_PLAYER,  false, &HandleChangeFactionCommand,		"", NULL },
	    { "maxskills",	SEC_PLAYER,  false, &HandleMaxSkillsCommand,		"", NULL },
	    { "customize",	SEC_PLAYER,  false, &HandleCustomizeCommand,		"", NULL },
	    { "tele",           SEC_PLAYER,  false, &HandleTeleCommand,		"", NULL },
 
            { NULL,             0,                     false, NULL,                                           "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "vip",	    SEC_PLAYER,   true, NULL,      "",  vipCommandTable},
	       { NULL,             0,                  false, NULL,                               "", NULL }
        };
        return commandTable;
    }


static bool HandleTeleCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* me = handler->GetSession()->GetPlayer();

        // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
        GameTele const* tele = handler->extractGameTeleFromLink((char*)args);

        if (!tele)
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (me->isInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        MapEntry const* map = sMapStore.LookupEntry(tele->mapId);
        if (!map || map->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_CANNOT_TELE_TO_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (me->isInFlight())
        {
            me->GetMotionMaster()->MovementExpired();
            me->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            me->SaveRecallPosition();

        me->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
        return true;
		}

static bool HandlevipCommand(ChatHandler* handler, const char* args)
    {

        Player* me = handler->GetSession()->GetPlayer();

            me->Say("vip command?", LANG_UNIVERSAL);
            return true;
    }

static bool HandleChangeRaceCommand(ChatHandler* handler, const char* args)
    {

        Player* me = handler->GetSession()->GetPlayer();
		me->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
		handler->PSendSysMessage("Relog to change race of your character.");
        return true;
    }

static bool HandleChangeFactionCommand(ChatHandler* handler, const char* args)
    {

        Player* me = handler->GetSession()->GetPlayer();
		me->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
		handler->PSendSysMessage("Relog to change faction of your character.");
        return true;
    }

static bool HandleMaxSkillsCommand(ChatHandler* handler, const char* args)
    {

        Player* me = handler->GetSession()->GetPlayer();
		me->UpdateSkillsForLevel();
		handler->PSendSysMessage("Your weapon skills are now maximized.");
        return true;
    }

static bool HandleCustomizeCommand(ChatHandler* handler, const char* args)
    {

        Player* me = handler->GetSession()->GetPlayer();
		me->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
		handler->PSendSysMessage("Relog to customize your character.");
        return true;
    }

static bool HandleVipMallCommand(ChatHandler* handler, const char* args)
    {

        Player* me = handler->GetSession()->GetPlayer();

        if (me->isInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (me->isInFlight())
        {
            me->GetMotionMaster()->MovementExpired();
            me->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            me->SaveRecallPosition();

		me->TeleportTo(0,	23.8378f,	-1588.85f,	195.419f,	4.54306f); // MapId, X, Y, Z, O
                return true;
    }


static bool HandleVipChatCommand(ChatHandler * handler, const char * args)
	{
		if (!args)
			return false;

		std::string msg = "";
		Player * player = handler->GetSession()->GetPlayer();

		switch(player->GetSession()->GetSecurity())
		{
 			case SEC_Viip: // VIP
				msg += "|cffff0000[V.I.P|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |CFF7BBEF7";
				break;

			case SEC_MODERATOR: // Trial GM
				msg += "|cffff0000[Trial GM|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |CFF7BBEF7";
				break;
				
			case SEC_JuniorGM: // Junior GM
				msg += "|CFF520084[Junior GM|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |CFF7BBEF7";
				break;
				
			case SEC_GAMEMASTER: // GM
				msg += "|cff00ffff[GM|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |CFF7BBEF7";
				break;
				
			case SEC_SeniorGM: // Senior GM
				msg += "|CFF7BBEF7[Senior GM|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |CFF7BBEF7";
				break;
				
			case SEC_ADMINISTRATOR: // Admin
				msg += "|cfffa9900[Admin|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |CFF7BBEF7";
				break;
				
			case SEC_SeniorAdmin: // Senior Admin
				msg += "|CFF18E7BD[Senior Admin|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |CFF7BBEF7";
				break;
								
		    case SEC_Dev: // Dev
				msg += "|CFF18BE00[Dev|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |CFF7BBEF7";
				break;
				
			case SEC_HeadDev: // Head Dev
				msg += "|CFF086142[Head Dev|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |CFF7BBEF7";
				break;
				
		    case SEC_HeadAdmin: // Head Admin
				msg += "|CFF18BE00[Head Admin|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |CFF7BBEF7";
				break;
			
			case SEC_CoOwner: // CO - OWNER
				msg += "|CFF7BBEF7[Co-Owner|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |cffff0000";
				break;
			
			case SEC_Owner: // OWNER
		     	msg += "|CFF7BBEF7[OWNER|r |cff18be00Chat:|r [";
				msg += player->GetName();
				msg += "]: |cffff0000";
				break;
	
			case SEC_CONSOLE: // Root, Console!
				msg += "|CFF18BE00[ROOT|r |cff18be00Chat:|r [";
				msg += player->GetName();
		msg += "]: |cffff0000";
				break;
		}
			
		msg += args;
		sWorld->SendServerMessage(SERVER_MSG_STRING, msg.c_str(), 0);	

		return true;
	}
	
};

void AddSC_vipcommands()
{
    new vipcommands();
}