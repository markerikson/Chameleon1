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

//includes
#include "p.h"

//global variables
using namespace std;



//@@@@@@@@@
//@ BEGIN @
//@@@@@@@@@

//Constructor
//Input: Zip right now.
//Output: Nothing really; just initializes the authorization and permission
//        codes and arrays.
Permission::Permission(long int loadAuthCode, long int loadPermCode)
{
	
	permNames = new wxArrayString();

	for(int i = 0; i < PERM_LAST; i++)
	{
		permNames->Add(GlobalPermStrings[i]);
	}



	//load permission code into bitset Array

	bitset<NUM_MODULES> tempP(loadPermCode);
	bitset<NUM_MODULES> tempA(loadAuthCode);
	status = tempP;
	auth = tempA;

	permCode = status.to_ulong();
	authCode = auth.to_ulong();
}

//Destructor
//Kills everything.
Permission::~Permission()
{
	status.reset();
	auth.reset();
	permCode = 0;
	authCode = 0;

	delete permNames;
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
	status.set(id);
	permCode = status.to_ulong();
}

//disable
//input: a valid module ID
//output: nothing.  [status] and [permCode] are updated
void Permission::disable(int id)
{
	status.reset(id);
	permCode = status.to_ulong();
}

//setGlobal
//input: a valid authorization code
//output: TRUE if the code parsed correctly, FALSE otherwise.  [auth] and
//        [authCode] are updated.
bool Permission::setGlobal(long newAuthCode)
{
	long int fuzz = newAuthCode;

	//load up the authorization array with the new permissions
	bitset<NUM_MODULES> temp(newAuthCode);
	auth = temp;

	//test for successful pass
	newAuthCode = auth.to_ulong();

	if(newAuthCode != fuzz)
		return(false);

	authCode = newAuthCode;
	return(true);
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
	
	if((int)permNames->Count() > permEnum)
	{
		return permNames->Item(permEnum);
	}
	return wxEmptyString;
}
//@@@@@@@
//@ END @
//@@@@@@@