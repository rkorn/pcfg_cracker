#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <deque>
#include <list>
#include "./hash_src/hashlibpp.h"

using namespace std;

typedef struct dataStruct {
	string password;
	int count;
	bool found;
	list<int> llist;
} dataType;
typedef struct passwordwtag {
	string password;
	int tagnumber;
};
int numberoffiles=1;
ofstream resultsFile[20];
ofstream resultsFile1;
ofstream resultsFile2;
ofstream resultsFile3;
int totalCracked[20];
void help();
int binarySearchPassword(deque<dataType>* hashList, string key);
void 	print_result ( int filen, unsigned long long numGuesses, string guess);
int cmp(struct passwordwtag p1, struct passwordwtag p2);
double calculateEntropy(string input);
bool containsDigits(string guess);
int evaluateRules(deque<dataStruct> *findQueue);

int main(int argc, char *argv[]) {
	
	hashwrapper *hashWrapper;// = new sha1wrapper();
	/*try
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
	*/

	ifstream passwordFile[20];
		
	ofstream notFoundFile;
	
    list<passwordwtag> slist1;
		
	deque<dataStruct> findQueue1;
	deque<dataStruct> findQueue2;
	
	bool isMinSize=false;
	bool digitsRequired=false;
	int minSize=0;
		
	bool findEntropy=false; //if it should print out the entropy to the found/notfound files
	bool limitGuesses=false; //if there are a limited number of guesses it should make
	bool checkRule=false; //used to debug rules to get them in the proper cracking order
	bool md5Flag = false; //perform an md5 hash on the strings being input from the Password Generator
	bool sha1Flag = false; //perform a sha1 hash 

	unsigned long long maxGuesses=0;  //The maximum number of guesses to make;
	
	unsigned long long numGuesses=0;

	for (int i=0; i<numberoffiles; i++)
		totalCracked[i]=0;
	
	//--Process the command line---------//
	
	if (argc<2) {
		cout << "\nYou need to specify an input file\n";
		help();
		return 0;
	}
	if (strcmp(argv[1], "-c")==0){
		sscanf(argv[2], "%d", &numberoffiles);
	}
	else numberoffiles=1;
	//cout << numberoffiles<<endl;
	for (int i=3; i<argc; i++) {
		//   if (i==argc-1) {  //find the name of the passwordfile to check against
		//   passwordFile.open(argv[i]);
		//}
		if (strcmp(argv[i],"-e")==0) { //print entropy data
			findEntropy=true;
		}
		else if (strcmp(argv[i],"-f")==0){
		
			for (int j=0; j< numberoffiles; j++)
			{
				i++;
				passwordFile[j].open(argv[i]);
			}
		}
		else if (strcmp(argv[i],"-l")==0) { //limit the number of guesses
			limitGuesses=true;
			i++;
			if ((i==argc)||(!isdigit(argv[i][0]))) { //--Yah, I'm lazy here as someone could input 1a
				cout << "\nNo max value specified\n";
				help();
				return 0;
			}
			else {
				sscanf (argv[i], "%llu", &maxGuesses);
				//maxGuesses= atoi(argv[i]);
			}
		}
		else if (strcmp(argv[i],"-m")==0) { //impose a minimum size for each password guess
			isMinSize=true;
			i++;
			if ((i==argc)||(!isdigit(argv[i][0]))) {
				cout << "\nNo min length specified\n";
				help();
				return 0;
			}
			else {
				minSize = atoi(argv[i]);
			}
		}
		else if (strcmp(argv[i],"-digits")==0) { //require digits in each password guess
			digitsRequired=true;
		}
		else if (strcmp(argv[i],"-checkRule")==0) { //debug the rules
			checkRule=true;
		}
		else if (strcmp(argv[i],"-md5") == 0) {
			if (sha1Flag) {
				sha1Flag = false;
				cout << "\nWarning: Two hashing algorithms specified, using md5\n";
			}
			md5Flag = true;
		}
		else if (strcmp(argv[i],"-sha1") == 0) {
			if(md5Flag) {
				md5Flag = false;
				cout << "\nWarning: Two hashing algorithms specified, using sha1\n";
			}
			sha1Flag = true;
		}		
		else {
			cout << "\nInvalid option\n";
			//help();
			return 0;
		}
	}

	//---Now Process the input file-------------------------//
	for (int i=0; i< numberoffiles; i++){
	if (!passwordFile[i].is_open()) {
		cout << "\nCould not open the " << i+1 << "th password file to check against\n";
		help();
		return 0;
	}
	}
	string filename;
	char numb[5];
	for (int i=1; i <= numberoffiles; i++)
	{
		//sprintf(filename,"result%d.txt", i); 
	//	itoa(i, numb, 10);
		filename="result";
		sprintf(numb, "%d", i);
		filename += numb;
		filename += ".txt";
		
	  resultsFile[i].open(filename.c_str());
	}

	notFoundFile.open("notfoundfile.txt");

	string inputLine;
	
	size_t carrigeReturnStripper;
	size_t marker;
	int filenumber;
    
	for (int i=0; i<numberoffiles; i++){
		getline(passwordFile[i],inputLine);
		int j =0;
		while (!passwordFile[i].eof()) {
			carrigeReturnStripper=inputLine.find('\r');  //get rid of any carrige returns in the input file
			if (carrigeReturnStripper!=string::npos) {
			      inputLine.resize(inputLine.size()-1);
			}
			if (inputLine.size()>=1) {
				passwordwtag temppass;
			//	sscanf(inputLine, "%s", &temppass.password);
				temppass.password=inputLine;
				temppass.tagnumber=i+1;
	//			cout << temppass.password << "\t" << temppass.tagnumber << endl;
				slist1.push_back(temppass);
			}
			getline(passwordFile[i],inputLine);
			string eight = "8";
			if (inputLine == "8")
		       cout << "\njth iteration : " << eight.size() << endl;
	        j++;
		}
		if (inputLine.find('\r')!=string::npos) {
			passwordwtag temppass;
			temppass.password=inputLine;
			temppass.tagnumber=i+1;
	//		cout << temppass.password << "\t" << temppass.tagnumber << endl;
	        cout << "\nhmmm" << endl;
			slist1.push_back(temppass);
				
		}
		cout << "Size of slist: " << slist1.size() << endl;
		slist1.sort(cmp);
	}
//	list<passwordwtag>::iterator it;
//		for (it=slist1.begin();it!=slist1.end();it++) {
//		cout << (*it).password << "\t"<< (*it).tagnumber << endl;
//		}
//		cout << "----------\n";

	// Assign hashWrapper to a hash function or delete
	
	if (md5Flag)
		hashWrapper = new md5wrapper();
	else if (sha1Flag)
		hashWrapper = new sha1wrapper();
	else { //non selected, free up memory
		//delete hashWrapper;
		hashWrapper = NULL;
	}
	//-----------now set up the datastructure for quick searches-----------------//
	list<passwordwtag>::iterator listIt;
	list<int> resultfile;
//	list<int>::iterator listIt2;
	string prevPass="";
	int curNumber=0;
	dataStruct inputValue;
//	listIt2=slist2.begin();
    ofstream compareDebug;
    compareDebug.open("compareDebug.txt");
	for (listIt=slist1.begin();listIt!=slist1.end();++listIt) {
		if (prevPass.compare((*listIt).password)==0) {

			resultfile.push_back((*listIt).tagnumber);	
			cout.clear();
			cout.flush();
			cout << "\nNon-unique string: " << prevPass << endl;

		}
		else {
			if (curNumber != 0) 
			{
				list<int>::iterator lit;
				for (lit=resultfile.begin(); lit!=resultfile.end(); ++lit){
		//			cout << "we got something: "<< (*lit) <<endl;
					inputValue.llist.push_back((*lit));
				}				
				findQueue1.push_back(inputValue);
				inputValue.llist.clear();
				resultfile.clear();
				curNumber=0;				
			}
			inputValue.password=(*listIt).password;
			//(*listIt).password;
//			cout << inputValue.password << "\t" << curNumber <<endl;
			inputValue.found=false;
			curNumber++;
//			if (curNumber ==1) 
//			cout << "password: " << (*listIt).password <<endl;
//			cout << "tag :" << (*listIt).tagnumber << endl;
			resultfile.push_back((*listIt).tagnumber);
			prevPass=(*listIt).password;
			compareDebug << listIt->password << "\n";
		}
	}
	if (curNumber != 0) 
	{
		list<int>::iterator lit;
		for (lit=resultfile.begin(); lit!=resultfile.end(); ++lit){
			inputValue.llist.push_back((*lit));
		}				
		findQueue1.push_back(inputValue);
		resultfile.clear();
		curNumber=0;				
	}
	if (findQueue1.empty()) {
		cout << "No passwords to parse in q 1\n";
		help();
		return 0;
	}
	
	
	cout << "Size of unique hashes: "<<findQueue1.size() <<endl ;
	//---------now process the input from the password cracker-------------------//
	 if (checkRule) {
	 evaluateRules(&findQueue1);
	}
	printf("0\t0\n");
	cout << "Max number of guesses: " << maxGuesses <<"\n";
	string guess;
	cin >> guess;
	int pos;
	while (!cin.fail()) {
		if (((!isMinSize)||(guess.size()>=minSize))&&((!digitsRequired)||(containsDigits(guess)))) {
			numGuesses++;
			if (numGuesses%1000000==0)
				cout << numGuesses  << endl;
			if ((limitGuesses)&&(maxGuesses<numGuesses)) { //reached the max number of guesses
				cout << "Reached the max number of guesses\n";
				cout << "numGuesses:" << '\t' << numGuesses << endl;
				break;
			}
			if (md5Flag || sha1Flag) {
				try {
					guess = hashWrapper -> getHashFromString(guess);
				}
				catch(hlException &e) {
					cerr << "Hashing error" << endl;
					exit(EXIT_FAILURE);
				}
			}
			pos=binarySearchPassword(&findQueue1,guess);
			if ((pos!=-1)&&(!findQueue1[pos].found)) {  //if the guess cracked a password
				findQueue1[pos].found=true;
				list<int>:: iterator tempit;
				for (tempit=findQueue1[pos].llist.begin(); tempit!=findQueue1[pos].llist.end(); ++tempit){
					print_result ( (*tempit), numGuesses, guess);
				}
				findQueue1.erase(findQueue1.begin()+pos);
			}
		}
		cin >> guess;
	}
	  for (int i=0;i!=findQueue1.size();i++) {
//	 for (int j=0;j<findQueue[i].count;j++) {
//	 if (!findEntropy) {
	 notFoundFile << findQueue1[i].password << endl; //prints a plaintext list of the not-cracked passwords
//	 }_re
//	 else {
//	 notFoundFile << calculateEntropy(findQueue[i].password) << endl; 
//	 }
	 }
//	 }

	
	
	return 0;
}

void help() {
	cout << "CheckPass - Tool used to check how a password cracker would do, without having to hash the passwords\n";
	cout << "Written by Matt Weir - Florida State University- later changed by Shiva houshmand to work for multiple files with related tags\n";
	cout << "The related results will be stored in files result1.txt, ... \n";
	cout << "Contact Info: weir@cs.fsu.edu\n";
	cout << "\n";
	cout << "Usage Info\n";
	cout << "----------------------------------------------------------------\n";
	cout << "checkPass <password file>\n";
	cout << "Options:\n";
	cout << "-c\t\t<number> number of input files\n";
	cout << "-f\t\tThe file names example: -f file1 file2 file3\n";
	cout << "-e\t\tPrint entropy data instead of plain guesses to found/notfound files\n";
	cout << "-l <max num of guesses>\tLimit the number of guesses made\n";
	cout << "example:\n";
	cout << "./john -incremental=alpha -stdout | ./checkPass mypasswordfile.txt\n";
	cout << "-m <Min Len of a Password Guess>\tWhen I'm too lazy to modify my rules to take into account a password creation policy\n"; 
	cout << "-md5\t\tHashes password guesses using the md5 algorithm. \n";
	cout << "-sha1\t\tHashes password guesses using the sha1 algorithm. \n";
	cout << "-digits\t\tPassword guesses must contain digits to be counted. Once again, implimenting it here since I'm lazy\n";
	cout << "-checkRule\t\tCheck how many passwords are cracked per rule. Rules are seperated by a 'THISISABENCHMARK!!'.\n";
	cout << "\n\n";
}

void 	print_result ( int filen, unsigned long long numGuesses, string guess)
{
	totalCracked[filen]+= 1;
	resultsFile[filen] << guess << '\n';
	resultsFile[filen] << numGuesses << '\t' << totalCracked[filen] << '\n';
		
}
int cmp (struct passwordwtag p1, struct passwordwtag p2)
{
	if (p1.password.compare(p2.password) <=0)
		return true; 
	else if (p1.password.compare(p2.password)>0)
		return false;
//	return p1.password.compare(p2.password);
//	strcmp(p1->password, p2->password);
}

int binarySearchPassword(deque<dataType>* hashList, string key) {
	int left;
	int right;
	int midpt;
	int size;
	int compareResult;
	
	size = (int)(*hashList).size();
	left = 0;
	right = size - 1;
	
	while (left <= right) {
		midpt = (int) ((left + right) / 2);
		compareResult=key.compare((*hashList)[midpt].password);
		if (compareResult== 0) {
			return midpt;
		}
		else if (compareResult > 0) {
			left = midpt + 1;
		}
		else {
			right = midpt - 1;
		}
	}
	return -1;
}

double calculateEntropy(string input) {
	double entropy = 0;
	int curSize = input.size();
	if (curSize==0) {
		return entropy;
	}
	//take care of the first char
	curSize--;
	entropy = 4;  //4 bits of entropy for the first char
	if (curSize<=8) {
		entropy = entropy + (curSize*2); //2 bits of entropy for the next 8 characters
	}
	else {
		curSize=curSize-8;
		entropy = entropy + 16;  //2 bits of entropy for each of the next 8 characters
		if (curSize <= 12) {
			entropy = entropy + (curSize*1.5); //1.5 bits of entropy for characters 9-20
		}
		else {
			curSize=curSize-12;
			entropy = entropy + (12*1.5);  //1.5 bits of entropy for characters 9-20;
			entropy = entropy + (curSize);  //1 bit of entorpy for each character more than 20
		}
	}
	//--now figure out if it has an uppercase character + non-alpha character
	bool hasNonAlpha = false;
	bool hasUpper = false;
	bool hasLower = false;
	for (int i=0;i<input.size();i++) {
		if (isalpha(input[i])) {
			if (islower(input[i])) {
				hasLower=true;
			}
			else {
				hasUpper=true;
			}
		}
		else {
			hasNonAlpha = true;
		}
	}
	if (hasNonAlpha&&hasUpper&&hasLower) {
		entropy = entropy + 6; //6 entropy for having upper/lower+nonAlpha
	}
	return entropy;
}

bool containsDigits(string guess) {
	for (int i=0;i<guess.size();i++) {
		if (isdigit(guess[i])) {
			return true;
		}
	}
	return false;
}

int evaluateRules(deque<dataStruct> *findQueue) {
	string guess;
	long long numGuesses=0;
	long long numGuessesPerRule=0;
	int curRule=1;
	long long numCrackedPerRule=0;
	bool ruleDone=false;
	double crackedPerGuess=0.0;
	
	cin >> guess;
	while (!cin.fail()) {
		numGuesses++;
		if (guess.compare("THISISABENCHMARK!!")==0) { //done with the current rule
			if (!ruleDone) {
				crackedPerGuess=numCrackedPerRule/(numGuesses*1.0);
				cout << "Rule:\t" << curRule << "\t" << crackedPerGuess << "\t" << numGuessesPerRule << "\t" << numCrackedPerRule << endl;
				curRule++;
				numGuessesPerRule=0;
				numCrackedPerRule=0;
				ruleDone=true;
				//        for (int i=0; i< (*findQueue).size();i++) { // clean it up for the next rule, aka any cracked passwords are no longer cracked
				//          (*findQueue)[i].found=false;
				//        }
			}
		}
		else {
			ruleDone=false;
			numGuessesPerRule++;
			int pos=binarySearchPassword(findQueue,guess);
			if ((pos!=-1)&&(!(*findQueue)[pos].found)) {  //if the guess cracked a password
				(*findQueue)[pos].found=true;
				numCrackedPerRule=numCrackedPerRule+(*findQueue)[pos].count; 
			}
		}
		cin >> guess;
	}
	return 0;
}
