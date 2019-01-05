#include <iostream>
#include <fstream>
#include "picosha2.h"

using namespace std;

bool checkBlockChain(string); //argument is a file name, checks if the current block chain is valid
bool isValid(string); //checks if the first character in the previos block is 0

string getPreviousBlockHash(string); //argument is a block, gets the previous block from the string
string getMerkleRoot(string); //argument is a block, gets the merkleroot from the string
string getNouce(string); //arguement is a block, gets the nouce from the string
string getCurrentBlock(string); //arguement is a file name, gets the most recent block from the block chain
string genMerkleRoot(string); //arguement is a file name, generates the merkleroot from the file
string genNouce(string, string);

string hexToString(string);
int hexCharToInt(char);


int main(int argc, char* argv[])
{
	if(argc == 3)
	{
		if(checkBlockChain(string(argv[1])))
		{
			string block = getCurrentBlock(string(argv[1]));
			string newPrevBlock = picosha2::hash256_hex_string(hexToString(getPreviousBlockHash(block) + getMerkleRoot(block) + getNouce(block)));
			string newMerkleRoot = genMerkleRoot(string(argv[2]));
			if(isValid(newPrevBlock))
			{
				string newNouce = genNouce(newPrevBlock,newMerkleRoot);
				cout << newPrevBlock << " " << newMerkleRoot << " " << newNouce << endl;
			}
			else
			{
				cout << newMerkleRoot << endl;
			}
		}
		else
		{
			string newMerkleRoot = genMerkleRoot(string(argv[2]));
			cout << newMerkleRoot << endl;
		}
	}
	else
	{
		cout << "Error: Command [blockchain] [transactions]" << endl;
	}
}

//check if the current block chain is valid
bool checkBlockChain(string s)
{
	ifstream fin;
	string block;
	fin.open(s);
	if(fin.fail())
	{
		fin.close();
		return false;
	}
	string prevBlock = "0";
	string hash = "0";
	while(getline(fin,block))
	{
		block.erase(remove(block.begin(), block.end(), '\r'), block.end());
		if(prevBlock.compare("0") != 0)
			hash = picosha2::hash256_hex_string(hexToString(getPreviousBlockHash(block) + getMerkleRoot(block) + getNouce(block)));
		if(prevBlock.compare(hash) != 0 || !isValid(prevBlock) || block.length() != 138)
		{
			fin.close();
			return false;
		}
		prevBlock = getPreviousBlockHash(block);
	}
	fin.close();
	return true;
}

//Mine for the Nouce after the new Previous block and Merkle root are found.
string genNouce(string p, string m)
{
	string nouce = "0000000";
	char hexNum = '1';
	string block = picosha2::hash256_hex_string(hexToString(p + m + (nouce + hexNum)));
	while(!isValid(block))
	{
		if(hexNum == 57)
			hexNum = 96;
		hexNum = hexNum+1;
		block = picosha2::hash256_hex_string(hexToString(p + m + (nouce + hexNum)));
	}
	return (nouce + hexNum);
}

//Generate the merkle root from the transactions file
string genMerkleRoot(string s)
{
	ifstream fin;
	vector<string> tree1,tree2;
	string t1,t2;
	fin.open(s);
	if(fin.fail())
	{
		fin.close();
		return "";
	}
	while(getline(fin,t1))
	{
		t1.erase(remove(t1.begin(), t1.end(), '\r'), t1.end());
		getline(fin,t2);
		t2.erase(remove(t2.begin(), t2.end(), '\r'), t2.end());
		t1 = picosha2::hash256_hex_string(t1);
		t2 = picosha2::hash256_hex_string(t2);
		tree1.push_back(t1);
		tree1.push_back(t2);
	}
	int treeSize = tree1.size();
	while(treeSize != 1)
	{
		for(int i = 0; i<treeSize; i=i+2)
		{
			tree2.push_back(picosha2::hash256_hex_string(tree1.at(i) + tree1.at(i+1)));
		}
		treeSize = tree2.size();
		tree1 = tree2;
		tree2.clear();
	}
	fin.close();
	return tree1.at(0);
}

//Gets the most recent block from the block chain
string getCurrentBlock(string s)
{
	ifstream fin;
	string block;
	fin.open(s);
	if(fin.fail())
	{
		fin.close();
		return "";
	}
	getline(fin,block);
	block.erase(remove(block.begin(), block.end(), '\r'), block.end());
	fin.close();
	return block;
}

//Gets the previous block from a block
string getPreviousBlockHash(string block)
{
	return block.substr(0,64);
}

//Gets the merkle root from a block
string getMerkleRoot(string block)
{
	return block.substr(65,64);
}

//Gets the Nouce from a block
string getNouce(string block)
{
	return block.substr(130,8);
}

//If the block starts with 0, it is valid
bool isValid(string block)
{
	if(block.at(0) == '0')
		return true;
	return false;
}

int hexCharToInt(char a)
{
    if(a>='0' && a<='9')
        return(a-48);
    else if(a>='A' && a<='Z')
        return(a-55);
    else
        return(a-87);
}

string hexToString(string str)
{
    stringstream HexString;
    for(int i=0;i<str.length();i++)
	{
        char a = str.at(i++);
        char b = str.at(i);
        int x = hexCharToInt(a);
        int y = hexCharToInt(b);
        HexString << (char)((16*x)+y);
    }
    return HexString.str();
}

