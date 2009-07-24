/* OperServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandOSOLine : public Command
{
 public:
	CommandOSOLine() : Command("OLINE", 2, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *flag = params[1].c_str();
		User *u2 = NULL;

		/* Only allow this if SuperAdmin is enabled */
		if (!u->isSuperAdmin)
		{
			notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ONLY);
			return MOD_CONT;
		}

		/* let's check whether the user is online */
		if (!(u2 = finduser(nick)))
			notice_lang(s_OperServ, u, NICK_X_NOT_IN_USE, nick);
		else if (u2 && flag[0] == '+')
		{
			ircdproto->SendSVSO(s_OperServ, nick, flag);
			ircdproto->SendMode(findbot(s_OperServ), nick, "+o");
			common_svsmode(u2, "+o", NULL);
			notice_lang(s_OperServ, u2, OPER_OLINE_IRCOP);
			notice_lang(s_OperServ, u, OPER_OLINE_SUCCESS, flag, nick);
			ircdproto->SendGlobops(s_OperServ, "\2%s\2 used OLINE for %s", u->nick, nick);
		}
		else if (u2 && flag[0] == '-')
		{
			ircdproto->SendSVSO(s_OperServ, nick, flag);
			notice_lang(s_OperServ, u, OPER_OLINE_SUCCESS, flag, nick);
			ircdproto->SendGlobops(s_OperServ, "\2%s\2 used OLINE for %s", u->nick, nick);
		}
		else
			this->OnSyntaxError(u);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_OLINE);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "OLINE", OPER_OLINE_SYNTAX);
	}
};

class OSOLine : public Module
{
 public:
	OSOLine(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSOLine(), MOD_UNIQUE);

		if (!ircd->omode)
			throw ModuleException("Your IRCd does not support OMODE.");
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_OLINE);
	}
};

MODULE_INIT("os_oline", OSOLine)
