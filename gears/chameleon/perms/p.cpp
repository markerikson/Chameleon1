/*******************\
*Permissions Manager*
\*******************/

#include "p.h"
#include<fstream>
#include<string>
#include<iostream>

using namespace std;

const string PERM_FILE = "perm.dat";
const int NOISE = 42;

Permission::Permission()
{
	long fuzz;

	//load the perm level
	ifstream in;
	in.open(PERM_FILE.c_str());

	in>>fuzz>>fuzz>>permCode>>fuzz;
	in.close();

	//decode the permCode (oooh secure)
	permCode = (permCode / fuzz) + fuzz;
	fuzz = permCode;

	cout<<"--PERMISSIONS::CONSTRUCTOR fuzz = "<<fuzz<<" |permCode = "<<permCode<<endl;

	//load permission code into bitset Array
	//after reading codumentation the following code is unneccesary
	//load up the status "array" using this method:
	/*
		2|134 = 0
		 | 67 = 1
		 | 33 = 1
		 | 16 = 0
		 |  8 = 0
		 |  4 = 0
		 |  2 = 0
		    1
			
		10000110 = 134
	*//*
	for(int i = 0; i < NUM_MODULES; i++)
	{
		if(permCode == 1)
		{
			status.set(i);
			i = NUM_MODULES;
		}
		else if(permCode % 2 == 1)
		{
			status.set(i);
		}
		else
		{
			status.reset(i);
		}

		permCode = long(permCode / 2);
	}
	*/
	//this code accomplishes the same thing:
	bitset<NUM_MODULES> temp(permCode);
	status = temp;

	cout<<"--PERMISSIONS::CONSTRUCTOR temp = "<<temp<<endl;


	permCode = status.to_ulong();

	if(fuzz != permCode)
	{
		cout<<"ERROR: Perm code did not de-code into Bitset"<<endl
			<<"Bitset: "<<permCode<<endl
			<<"Code:   "<<fuzz<<endl;
		exit(1);
	}

	//everything loaded correctly!
}

Permission::~Permission()
{
	ofstream fout;
	fout.open(PERM_FILE.c_str());

	//encode the permCode
	permCode = (permCode - NOISE) * NOISE;

	fout<<NOISE + NOISE<<endl
		<<NOISE / (NOISE - (NOISE / 2))<<endl
		<<permCode<<endl
		<<NOISE<<endl;

	fout.close();

	cout<<"--PERMISSIONS::DESTRUCTOR permCode = "<<permCode<<endl;

	status.reset();
	permCode = 0;
}


bool Permission::isEnabled(int id)
{
	return(status.test(id));
}

void Permission::enable(int id)
{
	status.set(id);
	permCode = status.to_ulong();
}

void Permission::disable(int id)
{
	status.reset(id);
	permCode = status.to_ulong();
}

bool Permission::setGlobal(long code)
{
	long int fuzz = code;

	//after reading documentation, the following code is unneccesary
	//load up the status "array" using this method:
	/*
		2|134 = 0
		 | 67 = 1
		 | 33 = 1
		 | 16 = 0
		 |  8 = 0
		 |  4 = 0
		 |  2 = 0
		    1
			
		10000110 = 134
	*//*
	for(int i = 0; i < NUM_MODULES; i++)
	{
		if(code == 1)
		{
			status.set(i);
			i = NUM_MODULES;
		}
		else if(code % 2 == 1)
		{
			status.set(i);
		}
		else
		{
			status.reset(i);
		}

		code = long(code / 2);
	}
	*/

	//this code does the same thing:
	bitset<NUM_MODULES> temp(code);
	status = temp;

	code = status.to_ulong();

	if(fuzz != code)
		return(false);

	permCode = code;
	return(true);
}

long Permission::getGlobal(int mode = 0)
{
	if(mode == 0)
		return(status.to_ulong());

	return(permCode);
}