/*******************\
*Permissions Manager*
\*******************/
//Source code for the permissions manager header file, created for the
//Chameleon Flex IDE.
//Author: Ben Carhart
//Date started: 11/22/03
//Date officially completed: --/--/--
//Description: manages the handling of permissions
//Major revisions:
//  ~BSC--xx/xx/xx--Changed the loading of the permCode by using a temp
//                  bitset variable
//  ~BSC--xx/xx/xx--Altered the error handling of the program to be
//                  a little more graceful.
//  ~BSC--11/29/03--Added the authorization bitset variable & implementation
//  ~BSC--02/09/04--Removed all file access.  User must pass in the # on
//                  initialization.  Also # encryption removed.
//  ~BSC--02/26/04--Changed authorization code to include check-sum for some
//                  security against kids just randomly changing authorization
//                  numbers and getting interesting authorized results.  i
//                  also pulled out the little extra code-checking down in
//                  "setGlobal()".  Why?  It wasn't working right.
//  ~BSC--03/20/04--adjusted authorization so that if "adv. comp." is enabled,
//					so is "compilation".  I basically put a small "if()...then"
//                  statement everywhere the [permCode] is somehow modified.  I
//                  also set it so that if normal compilation is disabled*, ADV
//                  compilation is disabled.  I didn't touch authorizations.
//                  *note- this disabling only occurs of "disable(id)" is called.
//  ~BSC--03/22/04--Adjused for Mark: a blank constructor

//includes
#include "p.h"
#include "../common/Crc16.h"
#include "../common/debug.h"
//global variables
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

wxString GlobalPermStrings[] = {"Syntax highlighting", 
								"Auto-indentation",
								"Debugging",
								"Terminal",
								"Local mode",
								"Projects",
								"Compilation",
								"Advanced compiler output",
								"Test permission"};

//@@@@@@@@@
//@ BEGIN @
//@@@@@@@@@

//Constructor(s)

//blank one: blank slate init.
Permission::Permission()
{
	//this is a valid "everything unauthorized" code
	setGlobalAuthorized("10A80000000");
	setGlobalEnabled("0");
}

Permission::Permission(wxString loadAuthCode, wxString loadPermCode)
{
	long tPCode, tACode;
	
	loadPermCode.ToLong(&tPCode);
	loadAuthCode.ToLong(&tACode);
	
	//permNames = new wxArrayString();

	for(int i = 0; i < PERM_LAST; i++)
	{
		permNames.Add(GlobalPermStrings[i]);
	}

	//load permission code into bitset Array

	bitset<NUM_MODULES> tempP(tPCode);
	bitset<NUM_MODULES> tempA(tACode);
	status = tempP;
	auth = tempA;

	permCode = status.to_ulong();
	authCode = auth.to_ulong();

	if(isEnabled(PERM_ADVANCEDCOMPILE))
	{
		enable(PERM_COMPILE);
	}
}

//Destructor
//Kills everything.
Permission::~Permission()
{
	status.reset();
	auth.reset();
	permCode = 0;
	authCode = 0;

	//delete permNames;
}

//isEnabled
//Input: a valid module ID
//Output: a boolean TRUE if the [status] bit is set, FALSE otherwise
bool Permission::isEnabled(int id)
{
	return(status.test(id) && auth.test(id));
}

//isAuthorized
//Input: a valid module ID
//Output: a boolean TRUE if the [auth] bit is set, FALSE otherwise
bool Permission::isAuthorized(int id)
{
	return(auth.test(id));
}

//enable
//Input: a valid module ID
//Output: nothing.  [status] and [permCode] are updated.
void Permission::enable(int id)
{
	if(isAuthorized(id))
	{
		status.set(id);
		permCode = status.to_ulong();

		if(isEnabled(PERM_ADVANCEDCOMPILE) && !(isEnabled(PERM_COMPILE)))
		{
			enable(PERM_COMPILE);
		}
	}

}

//disable
//input: a valid module ID
//output: nothing.  [status] and [permCode] are updated
void Permission::disable(int id)
{
	status.reset(id);
	permCode = status.to_ulong();

	if(id == PERM_COMPILE)
	{
		status.reset(PERM_ADVANCEDCOMPILE);
		permCode = status.to_ulong();
	}
}

//setGlobal
//input: a valid authorization code, defined as a string containing an integer
//		 with the CRC16 code for that integer's characters prepended to it
//output: TRUE if the code parsed correctly, FALSE otherwise.  [auth] and
//        [authCode] are updated.
bool Permission::setGlobalAuthorized(wxString newAuthCode)
{
	wxString crcPrefix;
	wxString crcGenerated;
	wxString numberString;

	if(newAuthCode.Len() < 4)
	{
		return false;
	}

	newAuthCode.MakeUpper();
	crcPrefix = newAuthCode.Left(4);

	numberString = newAuthCode.Mid(4);
	//wxLogDebug("newAuthCode: %s", newAuthCode);
	//wxLogDebug("numberString: %s", numberString);
	//wxLogDebug("crcPrefix: %s", crcPrefix);

	ClsCrc16 crc;
	crc.CrcInitialize();
	crc.CrcAdd((unsigned char *)numberString.GetData(), numberString.Len());
	
	crcGenerated.Printf("%X", crc.CrcGet());
	//wxLogDebug("crcGenerated: %s", crcGenerated);

	bool validAuthCode = (crcGenerated == crcPrefix);

	if(validAuthCode)
	{
		numberString.ToLong(&authCode);
		auth = bitset<NUM_MODULES>(authCode);
		savedAuthCode = newAuthCode;
	}

	return validAuthCode;
}

void Permission::setGlobalEnabled(wxString newEnableCode)
{
	newEnableCode.ToLong(&permCode);
	status = bitset<NUM_MODULES>(permCode);

	if(isEnabled(PERM_ADVANCEDCOMPILE))
	{
		enable(PERM_COMPILE);
	}

	savedPermCode << getGlobalEnabled();
}

//getGlobal
//input: nothing
//output: current permissions code stored; this is what is visible
long Permission::getGlobalEnabled()
{
	return(permCode);
}

//getGlobal 2
//input: nothing
//output: the authorization code.  This is what can be turned on/off.
long Permission::getGlobalAuthorized()
{
	return(authCode);
}


wxString Permission::getPermName(int permEnum)
{
	
	if((int)permNames.Count() > permEnum)
	{
		return permNames.Item(permEnum);
	}
	return wxEmptyString;
}
//@@@@@@@
//@ END @
//@@@@@@@