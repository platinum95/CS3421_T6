#pragma once
/*
* Custom implementation of a linked list
* Made because std::list didn't fit the needs
*/

//Templated class, can use any data type
template<typename T>
class PetesList{
public:
	struct PetesNode {
		T Data;
		PetesNode *Next, *Prev;
	};

	PetesList() {
		Front = Back = nullptr;	//Initialise the list
	}
	

	const PetesNode *GetFront() const {
		return Front;
	}
	const PetesNode *GetBack() const {
		return Back;
	}

	//Add data to the front. 
	//Doesn't need to be very fast as it is only run in the construction of the cache
	void push_front(T _Data){
		auto newNode = new PetesNode({ _Data, nullptr, nullptr });

		//Check if the list is empty
		if (Front == nullptr) {
			newNode->Next = newNode->Prev = nullptr;
			Front = Back = newNode;
			return;
		}
		else {
			newNode->Next = Front;
			Front->Prev = newNode;
			newNode->Prev = nullptr;
			Front = newNode;
		}
	}

	//Move element from the back to the front.
	//This is called during a cache miss
	//	when the LRU directory is replaced.
	//Should therefore be quite snappy
	void back_to_front() {
		//Return if the list is empty/size 1
		if (Back == Front)
			return;

		auto temp = Back;
		Back = Back->Prev;
		Back->Next = nullptr;

		Front->Prev = temp;
		temp->Next = Front;
		temp->Prev = nullptr;
		Front = temp;
	}

	//Move arbitrary element to the front of the list.
	//Occurs during a cache hit, when a given
	//	directory is the most recently used.
	//Must therefore also be quite snappy.
	void move_to_front(PetesNode *node) {
		//If empty/size 1 list, or node is already the front
		if (Back == Front || node == Front)
			return;

		//Hook up the preceeding node to the succeeding node
		node->Prev->Next = node->Next;
		if (node != Back) {
			//If we're not at the back of the list,
			// hook back the succeeding node to the
			// preceeding node
			node->Next->Prev = node->Prev;
		}
		else {
			//If we are at the back of the list,
			// update the Back pointer
			Back = node->Prev;
			Back->Next = nullptr;
		}

		//Insert the node at the front of the list
		Front->Prev = node;
		node->Next = Front;
		node->Prev = nullptr;
		Front = node;
	}

	//Not private for efficiency purposes
	PetesNode *Front, *Back;
};

