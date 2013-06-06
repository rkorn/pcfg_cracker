#include <iostream>
#include "hashlibpp.h"

using namespace std;

int main()
{
	hashwrapper *myWrapper = new sha1wrapper();
	try
	{
		cout << myWrapper -> getHashFromString("Hello World") << endl;
	}
	catch(hlException &e)
	{
		cerr << "Err" << endl;
	}

	cout << "this is running" << endl;
	delete myWrapper;
	myWrapper = NULL;
	
	return 0;
}
