#ifndef Database_h
#define Database_h

#include <algorithm>
#include <vector>
#include <set>
#include <string>

class MultiMap;

class Database
{
public:
	enum IndexType { it_none, it_indexed };
	enum OrderingType { ot_ascending, ot_descending };

	struct FieldDescriptor
	{
		std::string name;
		IndexType index;
	};

	struct SearchCriterion
	{
		std::string fieldName;
		std::string minValue;
		std::string maxValue;
	};

	struct SortCriterion
	{
		std::string fieldName;
		OrderingType ordering;
	};

	static const int ERROR_RESULT = -1;

	Database(); //Implemented
	~Database(); //Implemented
	bool specifySchema (const std::vector<FieldDescriptor>& schema); //Implemented
	bool addRow(const std::vector<std::string>& rowOfData); // Implemented
	bool loadFromURL(std::string url);//Implemented
	bool loadFromFile(std::string filename);  //Implemented
	int getNumRows() const; //Implemented
	bool getRow(int rowNum, std::vector<std::string>& row) const; //Implemented
	int search (const std::vector<SearchCriterion>& searchCriteria, const std::vector<SortCriterion>& sortCriteria, std::vector<int>& results);  //Implemented

private:
	Database(const Database& other);
	Database& operator=(const Database& rhs);

	std::vector<std::vector<std::string> > m_rows;
	std::vector<MultiMap*> m_fieldIndex;
	std::vector<FieldDescriptor> m_schema;
	std::vector<SortCriterion> sorting;

	//Helper Functions
	//Checks if field in question is in schema and sets pos to which column it is 
	bool inSchema(std::string a,int &pos) 
	{
		bool flag = false;

		for (unsigned int i = 0; i < m_schema.size() ; i++)
		{
			if (a == m_schema[i].name) { flag = true; pos = i; }
		}
		return flag;
	}
	//Intersector finds set intersection of a and b and puts it in a
	void intersector(std::set<int> &a,std::set<int> &b)
	{
		std::set<int>::iterator it1 = a.begin();
		std::set<int>::iterator it2 = b.begin();
		while( (it1!=a.end()) && (it2!=b.end()))
		{
			if (*it1<*it2)
			{
				a.erase(it1++);
			}
			else if (*it1>*it2)
			{
				++it2;
			}
			else
			{
				++it1;
				++it2;
			}
		}
		a.erase(it1,a.end());
	}
	//Determines which row of the database ROW is 
	int whichRow(std::vector<std::string>& row)
	{
		for (unsigned int j = 0 ; j < m_rows.size(); j++)
		{
			std::vector<std::string> curRow = m_rows[j];
			if (curRow == row) {return j;}
		}
		return 0;
	}
	//madHatter is a SUPERIOR version of the stl::sort 
	void madHatter(std::vector<std::vector<std::string> > &a,int colN, bool ascending)
	{
		int colNum = colN;
		quickSort(a,0,a.size()-1,colNum); //By default, returns entries in ascending order. 
		if (ascending == false)
		{
			std::reverse(a.begin(), a.end());
		}
	}
	//quickSort which sorts an entire database
	void quickSort(std::vector<std::vector<std::string>> &a, int First, int Last, int col)
	{
		if (Last-First < 1) {return;}
		int pivotIndex;

		pivotIndex = partition(a, First,Last,col);
		quickSort(a,First,pivotIndex-1,col);
		quickSort(a, pivotIndex+1,Last,col);
	}
	//called by quicksort to aid in sorting procedure 
	int partition(std::vector<std::vector<std::string>> &a, int low, int high, int col)
	{
		int pi = low;
		std::vector<std::string> pivot = a[low];
		do
		{
			while(low<=high && a[low][col]<=pivot[col])
				low++;
			while (a[high][col]>pivot[col])
				high--;
			if (low<high)
				swap(a[low],a[high]);
		}
		while(low<high);
		swap(a[pi],a[high]);
		pi = high;
		return (pi);
	}
};

#endif // Database_h

