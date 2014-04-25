#ifndef MultiMap_h
#define MultiMap_h

class MultiMap
{
private:
	//This is a linked list I used to hold the values of duplicate key's
	struct valNode
	{
		unsigned int value;
		valNode *next;
		valNode *prev;
	};
	//This is the BST It contains afore-mentioned linked list 
	struct BSTnode
	{
		BSTnode(const std::string &aKey)
		{
			key = aKey;
			//value = aVal;
			pLeft = pRight = nullptr;
			m_head = nullptr;
			parent = nullptr;
		}

		std::string key;

		unsigned int value;
		valNode* m_head;

		BSTnode* parent;
		BSTnode *pLeft;
		BSTnode *pRight;
	};	

public:
	//Iterator for the MultiMap
	class Iterator
	{
	public:
		Iterator()
		{
			cur = nullptr;
			m_flag = true;
		}
		Iterator(BSTnode* temp,BSTnode* root) 
		{
			cur = temp;
			m_root = root;
			m_flag = true;	
		}; 
		bool valid() const
		{
			if (m_flag==false){return false;}
			if (cur != nullptr){return true;}
			return false;
		}
		std::string getKey() const {return cur->key;} 
		unsigned int getValue() const; 
		bool next();
		bool prev();
	private:
		BSTnode *cur;
		BSTnode* m_root;
		bool m_flag;
		//Iterates through the linked list of values, used in FindEqualorPred...
		void goThru(BSTnode* temp)
		{
			while (temp->m_head->next!=nullptr)
			{
				temp->m_head = temp->m_head->next;
			}
		}
		//This resests the head pointers of every linked list of every node
		void astabilize() const
		{
			aInOrder(m_root);
		}
		//Uses in-order traversal to reinitilze linked lists
		void aInOrder(BSTnode* cur) const
		{
			if (cur == nullptr){return;}
			aInOrder(cur->pLeft);
			if (cur->m_head != nullptr)
			{
				if (cur->m_head->next == nullptr)
				{
					while (cur->m_head->prev != nullptr)
					{
						cur->m_head = cur->m_head->prev;
					}
				}
			}
			else
			{
				while (cur->m_head->prev != nullptr)
				{
					cur->m_head = cur->m_head->prev;
				}
			}
			aInOrder(cur->pRight);
		}
	};

	MultiMap(); 
	~MultiMap();
	void clear();
	
	void insert(std::string key, unsigned int value);

	Iterator findEqual(std::string key) const;
	Iterator findEqualOrSuccessor(std::string key) const; 
	Iterator findEqualOrPredecessor(std::string key) const;

private:
	//To prevent MultiMaps from being copied or assigned, declear these members private and don't implement
	MultiMap(const MultiMap &other);
	MultiMap& operator=(const MultiMap& rhs);
	
	//Data MEMBERS
	BSTnode *m_root;

	//Helper Function to Clear Tree
	void FreeTree(BSTnode *cur)
	{
		if (cur == nullptr) return;
		FreeTree(cur->pLeft);
		FreeTree(cur->pRight);
		valNode* temp = cur->m_head;
		while (temp!= nullptr)
		{
			valNode* next;
			next = temp->next;
			delete temp;
			temp = next;
		}
		delete cur;
	}

	//Helper Function to search BST
	bool Search(std::string key, BSTnode *ptr, BSTnode* &c) const
	{
		if (ptr==nullptr) {return false;}
		else if (key == ptr->key) 
		{
			c = ptr;
			return true;
		}
		else if (key < ptr->key)
		{
			return Search(key,ptr->pLeft,c);
		}
		else
		{
			return Search(key,ptr->pRight,c);
		}
		return false;
	}
	//Helper function to search BST when key may not be in the BST
	bool bigSearch(std::string key, BSTnode *ptr, BSTnode* &c) const
	{
		if (ptr==nullptr) {return true;}
		c = ptr;
		if (key < ptr->key)
		{
			return bigSearch(key,ptr->pLeft,c);
		}
		else if (key > ptr->key)
		{
			return bigSearch(key,ptr->pRight,c);
		}
		return false;
	}
	//resets head pointers for every linked list
	void stabilize() const
	{
		InOrder(m_root);
	}
	void InOrder(BSTnode* cur) const
	{
		if (cur == nullptr){return;}
		InOrder(cur->pLeft);
		if (cur->m_head != nullptr)
		{
			if (cur->m_head->next == nullptr)
			{
				while (cur->m_head->prev != nullptr)
				{
					cur->m_head = cur->m_head->prev;
				}
			}
		}
		else
		{
			while (cur->m_head->prev != nullptr)
			{
				cur->m_head = cur->m_head->prev;
			}
		}
		InOrder(cur->pRight);
	}
};

#endif // MultiMap_h