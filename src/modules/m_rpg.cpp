#include "inspircd.h"

/* $ModDesc: Provides NPC, NPCA, AMBIANCE, NARRATOR, and NARRATORA commands for use by Game Masters doing pen & paper RPGs via IRC */
/* $ModAuthor: Naram Qashat (CyberBotX) */
/* $ModAuthorMail: cyberbotx@cyberbotx.com */

/** Base class for /NPC and /NPCA
*/
class NPCx
{
	std::string text;

public:
	/* Removes any ! characters from a given nick */
	std::string strip_npc_nick(const std::string &nick)
	{
		std::string newnick = "";
		unsigned len = nick.size();
		for (unsigned x = 0; x < len; ++x)
		{
			char c = nick[x];
			if (c != '!')
				newnick += c;
		}
		return newnick;
	}

	CmdResult Handle(const std::vector<std::string> &parameters, User *user, bool action)
	{
		Channel *c = ServerInstance->FindChan(parameters[0]);
		if (c)
		{
			if (!c->HasUser(user))
			{
				user->WriteNumeric(ERR_NOTONCHANNEL, "%s %s :You are not on that channel!", user->nick.c_str(), parameters[0].c_str());
				return CMD_FAILURE;
			}
			if (c->GetPrefixValue(user) < OP_VALUE)
			{
				user->WriteNumeric(ERR_CHANOPRIVSNEEDED, "%s %s :You're not a channel operator", user->nick.c_str(), parameters[0].c_str());
				return CMD_FAILURE;
			}
		}
		else
		{
			user->WriteNumeric(ERR_NOSUCHCHANNEL, "%s %s :No such channel", user->nick.c_str(), parameters[0].c_str());
			return CMD_FAILURE;
		}

		/* Source is in the form of: *[nick]*!npc@[server-name] */
		std::string npc_source = "*" + this->strip_npc_nick(parameters[1]) + "*!npc@" + ServerInstance->Config->ServerName;

		c->WriteChannelWithServ(npc_source, "PRIVMSG %s :%s%s%s", c->name.c_str(), action ? "\1ACTION " : "", this->text.c_str(), action ? "\1" : "");

		/* we want this routed out! */
		return CMD_SUCCESS;
	}

	void SetText(const std::string &newText)
	{
		this->text = newText;
	}
};

/** Handle /NPC
*/
class CommandNPC : public Command, public NPCx
{
public:
	CommandNPC(Module *parent) : Command(parent, "NPC", 3, 3), NPCx()
	{
		this->syntax = "<channel> <npc-name> <npc-text>";
	}

	CmdResult Handle(const std::vector<std::string> &parameters, User *user)
	{
		return NPCx::Handle(parameters, user, false);
	}
};

/** Handle /NPCA
*/
class CommandNPCA : public Command, public NPCx
{
public:
	CommandNPCA(Module *parent) : Command(parent, "NPCA", 3, 3), NPCx()
	{
		this->syntax = "<channel> <npc-name> <npc-text>";
	}

	CmdResult Handle(const std::vector<std::string> &parameters, User *user)
	{
		return NPCx::Handle(parameters, user, true);
	}
};

/** Handle /AMBIANCE
*/
class CommandAmbiance : public Command
{
	std::string text;

public:
	CommandAmbiance(Module *parent) : Command(parent, "AMBIANCE", 2, 2)
	{
		this->syntax = "<channel> <text>";
	}

	CmdResult Handle(const std::vector<std::string> &parameters, User *user)
	{
		Channel *c = ServerInstance->FindChan(parameters[0]);
		if (c)
		{
			if (!c->HasUser(user))
			{
				user->WriteNumeric(ERR_NOTONCHANNEL, "%s %s :You are not on that channel!", user->nick.c_str(), parameters[0].c_str());
				return CMD_FAILURE;
			}
			if (c->GetPrefixValue(user) < OP_VALUE)
			{
				user->WriteNumeric(ERR_CHANOPRIVSNEEDED, "%s %s :You're not a channel operator", user->nick.c_str(), parameters[0].c_str());
				return CMD_FAILURE;
			}
		}
		else
		{
			user->WriteNumeric(ERR_NOSUCHCHANNEL, "%s %s :No such channel", user->nick.c_str(), parameters[0].c_str());
			return CMD_FAILURE;
		}

		/* Source is in the form of: >Ambiance<!npc@[server-name] */
		std::string amb_source = ">Ambiance<!npc@" + ServerInstance->Config->ServerName;

		c->WriteChannelWithServ(amb_source, "PRIVMSG %s :%s", c->name.c_str(), this->text.c_str());

		/* we want this routed out! */
		return CMD_SUCCESS;
	}

	void SetText(const std::string &newText)
	{
		this->text = newText;
	}
};

/** Base class for /NARRATOR and /NARRATORA
*/
class Narratorx
{
	std::string text;

public:
	CmdResult Handle(const std::vector<std::string> &parameters, User *user, bool action)
	{
		Channel *c = ServerInstance->FindChan(parameters[0]);
		if (c)
		{
			if (!c->HasUser(user))
			{
				user->WriteNumeric(ERR_NOTONCHANNEL, "%s %s :You are not on that channel!", user->nick.c_str(), parameters[0].c_str());
				return CMD_FAILURE;
			}
			if (c->GetPrefixValue(user) < OP_VALUE)
			{
				user->WriteNumeric(ERR_CHANOPRIVSNEEDED, "%s %s :You're not a channel operator", user->nick.c_str(), parameters[0].c_str());
				return CMD_FAILURE;
			}
		}
		else
		{
			user->WriteNumeric(ERR_NOSUCHCHANNEL, "%s %s :No such channel", user->nick.c_str(), parameters[0].c_str());
			return CMD_FAILURE;
		}

		/* Source is in the form of: -Narrator-!npc@[server-name] */
		std::string narr_source = std::string("-Narrator-!npc@") + ServerInstance->Config->ServerName;

		c->WriteChannelWithServ(narr_source, "PRIVMSG %s :%s%s%s", c->name.c_str(), action ? "\1ACTION " : "", this->text.c_str(), action ? "\1" : "");

		/* we want this routed out! */
		return CMD_SUCCESS;
	}

	void SetText(const std::string &newText)
	{
		this->text = newText;
	}
};

/** Handle /NARRATOR
*/
class CommandNarrator : public Command, public Narratorx
{
public:
	CommandNarrator(Module *parent) : Command(parent, "NARRATOR", 2, 2), Narratorx()
	{
		this->syntax = "<channel> <text>";
	}

	CmdResult Handle(const std::vector<std::string> &parameters, User *user)
	{
		return Narratorx::Handle(parameters, user, false);
	}
};

/** Handle /NARRATORA
*/
class CommandNarratorA : public Command, public Narratorx
{
public:
	CommandNarratorA(Module *parent) : Command(parent, "NARRATORA", 2, 2), Narratorx()
	{
		this->syntax = "<channel> <text>";
	}

	CmdResult Handle(const std::vector<std::string> &parameters, User *user)
	{
		return Narratorx::Handle(parameters, user, true);
	}
};

class ModuleRPGCommands : public Module
{
	CommandNPC npc;
	CommandNPCA npca;
	CommandAmbiance ambiance;
	CommandNarrator narrator;
	CommandNarratorA narratora;

public:
	ModuleRPGCommands() : npc(this), npca(this), ambiance(this), narrator(this), narratora(this)
	{
	}

	void init()
	{
		ServiceProvider *services[] = { &this->npc, &this->npca, &this->ambiance, &this->narrator, &this->narratora };
		ServerInstance->Modules->AddServices(services, sizeof(services) / sizeof(services[0]));

		ServerInstance->Modules->Attach(I_OnPreCommand, this);
	}

	Version GetVersion()
	{
		return Version("Provides NPC, NPCA, AMBIANCE, NARRATOR, and NARRATORA commands for use by Game Masters doing pen & paper RPGs via IRC");
	}

	ModResult OnPreCommand(std::string &command, std::vector<std::string> &parameters, LocalUser *user, bool validated, const std::string &original_line)
	{
		irc::spacesepstream sep(original_line);
		std::string text;
		sep.GetToken(text);
		sep.GetToken(text);
		if (command == "NPC" || command == "NPCA")
		{
			sep.GetToken(text);
			text = sep.GetRemaining();
			if (command == "NPC")
				this->npc.SetText(text);
			else
				this->npca.SetText(text);
		}
		else if (command == "AMBIANCE" || command == "NARRATOR" || command == "NARRATORA")
		{
			text = sep.GetRemaining();
			if (command == "AMBIANCE")
				this->ambiance.SetText(text);
			else if (command == "NARRATOR")
				this->narrator.SetText(text);
			else
				this->narratora.SetText(text);
		}

		return MOD_RES_PASSTHRU;
	}
};

MODULE_INIT(ModuleRPGCommands)