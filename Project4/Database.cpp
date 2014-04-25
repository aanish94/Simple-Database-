#include "Database.h"
#include <iostream>
#include "MultiMap.h"
#include <algorithm>
#include <set>
#include <string>
#include <vector>
#include "http.h"
#include "Tokenizer.h"
#include <fstream>
using namespace std;

Database::Database()
{
	//do nothing
}

Database::~Database()
{
	//Clears all allocated vectors
	m_schema.clear();
	m_rows.clear();
	m_fieldIndex.clear();
	sorting.clear();
}

bool Database::specifySchema (const std::vector<FieldDescriptor>& schema)
{
	//reset database
	m_schema.clear();
	m_rows.clear();
	m_fieldIndex.clear();
	//install new schema
	m_schema = schema;

	bool flag = false; 
	for (unsigned int i = 0; i < m_schema.size() ; i++)
	{
		if (m_schema[i].index == Database::it_indexed)
		{
			flag = true;
			m_fieldIndex.push_back(new MultiMap());
		}
		else{m_fieldIndex.push_back(nullptr);}
	}
	//returns false if there is no indexed column
	return flag;
}

bool Database::addRow(const std::vector<std::string>& rowOfData)
{
	//cannot add a row if there is no schema OR if the row to be added doesn't match the schema
	if (m_schema.empty()){return false;}
	if (m_schema.size() != rowOfData.size()){return false;}

	m_rows.push_back(rowOfData);

	int cur = m_rows.size() - 1;

	for (unsigned int j =0 ; j< m_rows[cur].size() ;j++)
	{
		if (m_schema[j].index == Database::it_indexed)
		{
			m_fieldIndex[j]->insert(m_rows[cur][j],cur);
		}
	}
	return true;
}

int Database::getNumRows() const
{
	//simply returns size of m_rows
	return m_rows.size() - 1;
}

bool Database::getRow(int rowNum, std::vector<std::string>& row) const
{
	//if valid index,
	if (rowNum < 0 || rowNum > getNumRows()) { return false; }
	//clear whatever was in row 
	row.clear();
	//get the rowNum th row
	row = m_rows[rowNum];

	return true;
}

int Database::search (const std::vector<SearchCriterion>& searchCriteria, const std::vector<SortCriterion>& sortCriteria, std::vector<int>& results)
{
	//empty the results vecotr
	results.clear();
	sorting = sortCriteria;
	std::vector<SearchCriterion> searchCrit = searchCriteria;
	std::vector<int> curResults;
	std::set<int> s1;
	std::set<int> s2;
	//go through each of the search criteria
	for (unsigned int i = 0; i < searchCrit.size(); i++)
	{
		std::set<int> s3;
		string min = searchCrit[i].minValue;
		string max = searchCrit[i].maxValue;
		//finds which colNum to be searching
		int colNum;
		if (!inSchema(searchCrit[i].fieldName,colNum)) { return ERROR_RESULT;}
		if (min=="" && max ==""){return ERROR_RESULT;}
		//cannot leave both min and max empty 
		std::vector<std::string> curRow;

		MultiMap::Iterator aMin = m_fieldIndex[colNum]->findEqualOrSuccessor(min);
		MultiMap::Iterator aMax = m_fieldIndex[colNum]->findEqualOrPredecessor(max);
		//So now i have iterators pointing to min and max values. 
		if (max != "")
		{
			while (aMin.valid() && aMin.getKey() <= aMax.getKey())
			{
				s3.insert(aMin.getValue());
				aMin.next();
			}
		}
		else
		{
			while (aMin.valid())
			{
				s3.insert(aMin.getValue());
				aMin.next();
			}
		}
		//right now, s3 is set filled with all integers corresponding to correct rows
		if (s1.empty())
		{
			set<int>::iterator it = s3.begin();
			while(it!=s3.end())
			{
				s1.insert(*it);
				++it;
			}
		}
		else
		{
			//finds the intersection of s1 and s3 and puts it in s1
			intersector(s1,s3);
		}
	}
	set<int>::iterator it1 = s1.begin();
	//fill up results
	while(it1!=s1.end())
	{
		results.push_back(*it1);
		++it1;
	}
	
	//at this point both results and s1 contain all the to be returned queries
	//now we have to sort them
	
	vector<vector<string> > corRows;
	for (unsigned int m = 0; m < results.size(); m++)
	{
		vector<string> rowData;
		if (getRow(results[m],rowData))
		{
			corRows.push_back(rowData);
		}
	} 
	bool sorted = false;
	//so now i have a vector of all the correct rows. 
	//go through each one of the sortcriteria
	for (unsigned int y = 0 ; y < sortCriteria.size() ; y++)
	{
		int curCol;
		if (!inSchema(sortCriteria[y].fieldName,curCol)) { return Database::ERROR_RESULT;}
		
		bool ascending = false;
		if (sortCriteria[y].ordering == Database::ot_ascending) { ascending = true;}
		
		//stl sort is useless, i'm writing my own using quicksort
		if (sorted == false) // If more than one sort criteria are provided, my algorithm will sort the entire vector each time. Therefore, this if statement ensures sorting only occurs once.
			madHatter(corRows,curCol,ascending);
		sorted = true; // remove this line to sort more than once. note: it will override any previous sorting. 
		
	}
	
	//at this point curRows is correctly sorted and contains all of the rows i need. now i need a way to get the integer row values
	results.clear();
	//re-fill results with all the row numbers correctly ordered
	for (unsigned int n = 0 ; n < corRows.size() ; n++)
	{
		results.push_back(whichRow(corRows[n]));
	}
	return results.size();
}

bool Database::loadFromURL(std::string url) 
{
	string s = url;

	string delimiters = "\n";
	string delim2 = ",";
	string delim3 = "*";

	string page; 
	if (!HTTP().get(s,page)){return false;}
	
	vector<Database::FieldDescriptor> schema;
	Tokenizer t (page, delimiters);

	string first;
	t.getNextToken(first);

	Tokenizer t1(first,delim2);
	//Gets the very first line of text and uses it to specify schema
	while(t1.getNextToken(first))
	{
		Database::FieldDescriptor* a = new Database::FieldDescriptor;
		int size = first.size();
		if (first.substr(size-1,size-2) == "*")
		{
			//asterik indicates the field is indexed
			a->name = first.substr(0,size-1);
			a->index = Database::it_indexed;
		}
		else
		{
			a->name = first;
			a->index = Database::it_none;
		}
		schema.push_back(*a);
	}
	specifySchema(schema);

	string cur;
	//now go thru all other rows and use addRow method to create database
	while (t.getNextToken(cur))
	{
		Tokenizer t2 (cur,delim2);
		string nCur;
		vector<string> row;

		while (t2.getNextToken(nCur))
		{
			row.push_back(nCur);
		}
		addRow(row);
	}
	return true;
}

bool Database::loadFromFile(std::string filename)
{
	//analogous to loadFromURL
	ifstream infile(filename);
	if (!infile){cerr << "Error. Cannot Open file."<<endl;return false;}

	string page;
	vector<Database::FieldDescriptor> schema;

	string delim2 = ",";
	string delim3 = "*";
	//get first line of file and use it to specify schema
	if (getline(infile,page))
	{
		Tokenizer t1(page,delim2);
		while(t1.getNextToken(page))
		{
			Database::FieldDescriptor* a = new Database::FieldDescriptor;
			int size = page.size();
			if (page.substr(size-1,size-2) == "*")
			{
				//if asterik, field should be indexed 
				a->name = page.substr(0,size-1);
				a->index = Database::it_indexed;
			}
			else
			{
				a->name = page;
				a->index = Database::it_none;
			}
			schema.push_back(*a);
		}
		specifySchema(schema);
	}
	//go thru and use addRow method to create database 
	while (getline(infile,page))
	{
		string nCur;
		Tokenizer t2(page,delim2);
		vector<string> row;
		while (t2.getNextToken(nCur))
		{
			row.push_back(nCur);
		}
		addRow(row);
	}
	return true;
}