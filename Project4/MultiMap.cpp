#include <string>
#include "MultiMap.h"
using namespace std;

MultiMap::MultiMap()
{
	m_root = nullptr;
};

void MultiMap::insert(std::string key, unsigned int value)
{
	//4 cases for insertion: no current value, less than current value, greater than curernt value, equal to current value
	//case one: no current value
	if (m_root == nullptr)
	{
		BSTnode* temp = new BSTnode(key);
		valNode* curVal = new valNode;
		//add value to linked list
		curVal->value = value;
		curVal->next = temp->m_head;
		curVal->prev = temp->m_head;
		temp->m_head = curVal;
		m_root = temp;
		
		return;
	}

	BSTnode *cur = m_root;
	for (;;)
	{
		//next case, add lesser values to left
		if (key < cur->key)
		{
			if (cur->pLeft != nullptr) { cur = cur->pLeft;}
			else
			{
				BSTnode* temp = new BSTnode(key);
				valNode* curVal = new valNode;

				temp->parent = cur;
				curVal->value = value;
				curVal->next = temp->m_head;
				curVal->prev = nullptr;

				temp->m_head = curVal;

				cur->pLeft = temp;

				return;
			}
		}
		//next case, add greater values to right
		else if (key > cur->key)
		{
			if (cur->pRight != nullptr) { cur = cur->pRight;}
			else
			{
				BSTnode* temp = new BSTnode(key);
				valNode* curVal = new valNode;

				temp->parent = cur;
				curVal->value = value;
				curVal->next = temp->m_head;
				curVal->prev = nullptr;

				temp->m_head = curVal;

				cur->pRight = temp;

				return;
			}
		}
		//worst case, add equal values
		else if (key == cur->key)
		{
			valNode* curVal = new valNode;
			//for this case, we must not add the key, only add value to the linked list
			curVal->value = value;
			curVal->next = cur->m_head;
			curVal->prev = nullptr;
			cur->m_head->prev = curVal;			
			cur->m_head = curVal;
			return;
		}
	}
}

void MultiMap::clear()
{
	//frees memory and allows tree to be reused
	FreeTree(m_root);
	m_root = nullptr;	
}

MultiMap::~MultiMap()
{ 
	FreeTree(m_root);
}

bool MultiMap::Iterator::next()
{
	//must currently have a valid tree
	if (!valid()){return false;}
	//if the linked list has values i can move towards
	if (cur->m_head != nullptr)
	{
		if (cur->m_head->next != nullptr)
		{			
			cur->m_head = cur->m_head->next;
			return true;
		}
	}
	//else the next value would be to the right
	if (cur->pRight != nullptr)
	{   
		cur = cur->pRight; 
		//if the right child has left children, these will be next
		while (cur->pLeft != nullptr)
		{
			cur = cur->pLeft;
		}
		return true;
	}
	//else the next is an ancester
	string checkKey = cur->key;
	bool flag = false;
	BSTnode* temp = cur;
	while(cur != m_root && !flag)
	{
		cur = cur->parent; 
		//make sure parent is the right one
		if (cur->key > checkKey){flag = true;}
	}
	// i have gotten to the very end of the tree and must stabilize tree
	if (!flag) { m_flag = false; cur = temp; astabilize();}
	
	return true;
}

bool MultiMap::Iterator::prev()
{
	//literally symmetric to next
	if (!valid()){return false;}
	//had to implement doubly linked list so that i could move backwards here
	if (cur->m_head != nullptr)
	{
		if (cur->m_head->prev != nullptr)
		{			
			cur->m_head = cur->m_head->prev;
			return true;
		}
	}
	//prev would be before i.e. left child
	if (cur->pLeft != nullptr)
	{   
		cur = cur->pLeft;
		//go to the end of the linked list
		goThru(cur);
		while (cur->pRight != nullptr)
		{
			cur = cur->pRight;
			goThru(cur);
		}
		return true;
	}
	//prev must be an ancester
	string checkKey = cur->key;
	bool flag = false;
	BSTnode* temp = cur;
	while(cur != m_root && !flag)
	{
		cur = cur->parent;
		goThru(cur);
		if (cur->key < checkKey){flag = true;}
	}
	if (!flag) { m_flag = false; cur = temp; astabilize();}

	return true;
}

MultiMap::Iterator MultiMap::findEqual(std::string key) const
{
	//uses helper function and returns iterator to key node
	BSTnode *cur;
	if (!(Search(key,m_root,cur)))
	{
		Iterator it(nullptr,m_root);
		return it;
	}
	Iterator it(cur,m_root);
	return it;
}

MultiMap::Iterator MultiMap::findEqualOrSuccessor(std::string key) const
{
	//checks if key is in node, if not it finds closest value to key and uses next to find sucessor
	BSTnode *cur;
	MultiMap::Iterator temp = findEqual(key);
	if (temp.valid()) { return temp; } 
	if (!(bigSearch(key,m_root,cur)))
	{
		Iterator it(nullptr,m_root);
		return it;
	}
	stabilize();
	Iterator it(cur,m_root);
	while (key > it.getKey() && it.valid())
	{
		it.next();
	}
	return it;
}

MultiMap::Iterator MultiMap::findEqualOrPredecessor(std::string key) const
{
	//checks if key in tree, if not finds closest value and uses prev to find predecessor
	BSTnode *cur;
	MultiMap::Iterator temp = findEqual(key);
	if (temp.valid()) { return temp; } 

	if (!(bigSearch(key,m_root,cur)))
	{
		Iterator it(nullptr,m_root);
		return it;
	}
	stabilize();
	Iterator it(cur,m_root);
	while (key < it.getKey() && it.valid())
	{
		it.prev();
	}
	
	return it;
}

unsigned int MultiMap::Iterator::getValue() const
{
	if (cur->m_head != nullptr) {return cur->m_head->value;}
	return -1; 
}
