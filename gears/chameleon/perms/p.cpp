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
Permission::Permission(wxString loadAuthCode, wxString loadPermCode)
{
	long tPCode, tACode;
	
	loadPermCode.ToLong(&tPCode);
	loadAuthCode.ToLong(&tACode);
	
	permNames = new wxArrayString();

	for(int i = 0; i < PERM_LAST; i++)
	{
		permNames->Add(GlobalPermStrings[i]);
	}

	//load permission code into bitset Array

	bitset<NUM_MODULES> tempP(tPCode);
	bitset<NUM_MODULES> tempA(tACode);
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
bool Permission::setGlobal(wxString newAuthCode)
{
	//the permcode comes like this:
	//                                       XX XX NN...NN
	//we're interested in these chars/nums:  |   | |    ||
	//                                       c1 c2 1    23
	char charOne, charTwo;
	long int specialOne = 0,
		specialTwo = 0,
		specialThree = 0;
	int sLength = int(newAuthCode.Length());
	int charOnePos = 0,
		charTwoPos = 0;
	long int fuzz = 0,
		attemptedAuthCode = 0;
	wxString temp;

	newAuthCode.Upper();

	charOne = newAuthCode.GetChar(0);
	charTwo = newAuthCode.GetChar(3);

	temp = newAuthCode.GetChar(4);
	temp.ToLong(&specialOne);

	temp = newAuthCode.GetChar(sLength - 2);
	temp.ToLong(&specialTwo);

	temp = newAuthCode.GetChar(sLength - 1);
	temp.ToLong(&specialThree);

	//begin test for authentic authorization code
	//charTwo is the product of [specialOne + specialThree] in relation to
	//our alphabet, with G = 0.  eg: [s1 + s2] = 6.  Since G=0, then
	//charTwo should equal M.
	//charOne is the product of [offset(charTwo) + specialTwo)].  In our
	//example, if specialTwo = 4 then charOne = Q

	//get position of first character (charOne)
	while(AuthCodeLookupTable[charOnePos] != charOne)
	{charOnePos++;}

	charTwoPos = charOnePos - specialTwo; //charOne = [charTwo + specialTwo)]
	if(charTwoPos < 0)
	{charTwoPos += 26;}

	//first pass
	if(AuthCodeLookupTable[charTwoPos] == charTwo)
	{
		//second pass; [specialOne + specialThree] must equal [charTwoPos]
		if((specialOne + specialThree) == charTwoPos)
		{
			//get the new authcode; it's valid.
			newAuthCode = newAuthCode.Mid(4); //get rid of the first 4 chars

			newAuthCode.ToLong(&fuzz);	//extract the authorization code
			attemptedAuthCode = fuzz;

			//load up the authorization array with the new permissions
			bitset<NUM_MODULES> temp(attemptedAuthCode);
			auth = temp;
			return(true);
		}
	}

	//someone tried to hack our code.  naughty naughty
	return(false);
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