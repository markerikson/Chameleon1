/*******************\
*Permissions Manager*
\*******************/
//Header file for the permissions manager for Chameleon Flex IDE
//Author: Ben Carhart
//Date started: 11/22/03
//Date officially completed: --/--/--
//Description: code prototyping for the permissions class
//Revisions:

#ifndef PERM___H
#define PERM___H

//includes
#include<bitset>
#include <wx/wx.h>
#include "../common/datastructures.h"

//globals
using namespace std;

const int NUM_MODULES = 15;
//give each module a number


//class declaration
class Permission
{
	public:
		Permission(long int, long int);
		~Permission();
		bool isEnabled(int);		//module-specific permissions request
		bool isAuthorized(int);		//module-specific authorization request
		void enable(int);			//module-specific enable
		void disable(int);			//module-specific disable
		bool setGlobal(long);		//set everything
		long getGlobalEnabled();	//what should be visible
		long getGlobalAuthorized();	//what's allowed to be turned on/off

		wxString getPermName(int permEnum);  // returns the string the matches the enumeration

	private:
		bitset <NUM_MODULES> status;//array for holding module status
		bitset <NUM_MODULES> auth;	//array for holding what's allowed
		long permCode;				//int for holding permissions code #
		long authCode;				//int for holding authorized code #
		wxArrayString* permNames;    //matches the enumerated perms with a name 
};

#endif