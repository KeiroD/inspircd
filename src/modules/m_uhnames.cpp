/*       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  InspIRCd: (C) 2002-2008 InspIRCd Development Team
 * See: http://www.inspircd.org/wiki/index.php/Credits
 *
 * This program is free but copyrighted software; see
 *            the file COPYING for details.
 *
 * ---------------------------------------------------
 */

#include "inspircd.h"
#include "m_cap.h"

/* $ModDesc: Provides the UHNAMES facility. */

class ModuleUHNames : public Module
{
	CUList nl;
 public:
	
	ModuleUHNames(InspIRCd* Me)
		: Module(Me)
	{
		Implementation eventlist[] = { I_OnEvent, I_OnSyncUserMetaData, I_OnPreCommand, I_OnNamesListItem, I_On005Numeric };
		ServerInstance->Modules->Attach(eventlist, this, 4);
	}

	virtual ~ModuleUHNames()
	{
	}

	void OnSyncUserMetaData(User* user, Module* proto,void* opaque, const std::string &extname, bool displayable)
	{
		if ((displayable) && (extname == "UHNAMES"))
			proto->ProtoSendMetaData(opaque, TYPE_USER, user, extname, "Enabled");
	}

	virtual Version GetVersion()
	{
		return Version(1,2,0,1,VF_VENDOR,API_VERSION);
	}

	virtual void On005Numeric(std::string &output)
	{
		output.append(" UHNAMES");
	}

	virtual int OnPreCommand(const std::string &command, const char* const* parameters, int pcnt, User *user, bool validated, const std::string &original_line)
	{
		irc::string c = command.c_str();
		/* We don't actually create a proper command handler class for PROTOCTL,
		 * because other modules might want to have PROTOCTL hooks too.
		 * Therefore, we just hook its as an unvalidated command therefore we
		 * can capture it even if it doesnt exist! :-)
		 */
		if (c == "PROTOCTL")
		{
			if ((pcnt) && (!strcasecmp(parameters[0],"UHNAMES")))
			{
				user->Extend("UHNAMES");
				return 1;
			}
		}
		return 0;
	}

	virtual void OnNamesListItem(User* issuer, User* user, Channel* channel, std::string &prefixes, std::string &nick)
	{
		if (!issuer->GetExt("UHNAMES"))
			return;

		if (nick.empty())
			return;

		nick = user->GetFullHost();
	}

	virtual void OnEvent(Event* ev)
	{
		GenericCapHandler(ev, "UHNAMES", "userhost-in-names");
	}
};

MODULE_INIT(ModuleUHNames)
