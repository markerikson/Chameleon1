/*******************\
*Permissions Manager*
\*******************/
/*
  Revisions:

  Notes:
*/

#ifndef PERM___H
#define PERM___H

#include<bitset>

using namespace std;

const int NUM_MODULES = 15;
//give each module a number


enum Permissions
{
	PERM_SYNTAXHIGHLIGHT = 0,
	PERM_AUTOCOMPLETE,
	PERM_AUTOINDENT,
	PERM_DEBUG,
};



class Permission
{
	public:
		Permission();
		~Permission();
		bool isEnabled(int);	//module-specific permissions request
		void enable(int);		//module-specific enable
		void disable(int);		//module-specific disable
		bool setGlobal(long);	//set everything
		long getGlobal(int);	//status of everything

	private:
		bitset <NUM_MODULES> status;//array for holding module status
		long permCode;				//int for holding permissions code #
};

#endif