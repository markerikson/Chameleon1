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

//includes
#include "p.h"
#include<fstream>
#include<string>
#include<iostream>

//global variables
using namespace std;

const string PERM_FILE = "perm.dat";
const int NOISE = 42;

//@@@@@@@@@
//@ BEGIN @
//@@@@@@@@@

//Constructor
//Input: Zip right now.
//Output: Nothing really; just initializes the authorization and permission
//        codes and arrays.
Permission::Permission()
{
	int fuzz, topaz;

	//load the perm level
	ifstream in;
	in.open(PERM_FILE.c_str());

	if(!in)
	{
		io = false;
		permCode = -1764;
		authCode = -729;
	}
	else
	{
		io = true;
		in>>fuzz>>fuzz>>permCode>>fuzz;
		in>>topaz>>authCode>>topaz>>topaz;
	}
	
	in.close();

	//decode the codes (oooh secure)
	permCode = (permCode / fuzz) + fuzz;
	fuzz = permCode;

	authCode = (authCode + (topaz * topaz)) / topaz;
	topaz = authCode;

	//DEBUG CODE//
	//cout<<"--PERMISSIONS::CONSTRUCTOR fuzz = "<<fuzz<<" |permCode = "<<permCode<<endl;

	//load permission code into bitset Array

	bitset<NUM_MODULES> tempP(permCode);
	bitset<NUM_MODULES> tempA(authCode);
	status = tempP;
	auth = tempA;

	//DEBUG CODE//
	//cout<<"--PERMISSIONS::CONSTRUCTOR temp = "<<temp<<endl;

	permCode = status.to_ulong();
	authCode = auth.to_ulong();

	if(permCode != fuzz)
	{
		/* Old Error Handling
		cout<<"ERROR: Perm code did not de-code into Bitset"<<endl
			<<"Bitset: "<<permCode<<endl
			<<"Code:   "<<fuzz<<endl;
		exit(1);
		*/
		//error has occured decoding the permissions code.Reset to null state
		status.reset();
	}

	if(authCode != topaz)
	{
		auth.reset();
	}

	//everything loaded correctly!
}

//Destructor
//Kills everything.  Also handles saving of the permissions and authorization
//codes
Permission::~Permission()
{
	ofstream fout;
	fout.open(PERM_FILE.c_str());

	//unable to generate output file.
	if(!fout)
	{
		io = false;
	}
	else
	{
		//output file generated.
		io = true;	//*pointless really*//

		//encode the codes
		permCode = (permCode - NOISE) * NOISE;
		authCode = (authCode * (NOISE - 15)) - ((NOISE - 15) * (NOISE - 15));

		fout<<NOISE + NOISE<<endl
			<<(NOISE / (NOISE - (NOISE / 2)))<<endl
			<<permCode<<endl
			<<NOISE<<endl
			<<(NOISE * NOISE)<<endl
			<<authCode<<endl
			<<(NOISE * (NOISE - 15))<<endl
			<<(NOISE - 15)<<endl;
	}

	fout.close();

	//DEBUG CODE//
	//cout<<"--PERMISSIONS::DESTRUCTOR permCode = "<<permCode<<endl;

	status.reset();
	auth.reset();
	permCode = 0;
	authCode = 0;
}

//fileSuccess
//Input: nothing
//Output: returns whether the permissions file was successfully parsed.
bool Permission::fileSuccess()
{
	return(io);
}

//isEnabled
//Input: a valid module ID
//Output: a boolean TRUE if the [status] bit is set, FALSE otherwise
bool Permission::isEnabled(int id)
{
	return(status.test(id));
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
bool Permission::setGlobal(long code)
{
	long int fuzz = code;

	//load up the authorization array with the new permissions
	bitset<NUM_MODULES> temp(code);
	auth = temp;

	//test for successful pass
	code = auth.to_ulong();

	if(code != fuzz)
		return(false);

	authCode = code;
	return(true);
}

//getGlobal
//input: (this version) nothing
//output: the current permissions code
long Permission::getGlobal()
{
	return(permCode);
}

//getGlobal 2
//input: a key number, 0 or otherwise
//output: the authorization code, either straight or from the bitset array
long Permission::getGlobal(int mode = 0)
{
	if(mode == 0)
		return(auth.to_ulong());

	return(authCode);
}
//@@@@@@@
//@ END @
//@@@@@@@