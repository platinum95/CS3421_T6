#pragma once
template<typename T>
class PetesList{
private:
	
public:
	struct PetesNode {
		T Data;
		PetesNode *Next, *Prev;
	};

	PetesList() {
		Front = Back = nullptr;
	}

	void push_front(T _Data){
		auto newNode = new PetesNode;
		newNode->Data = _Data;
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

	inline void back_to_front() {
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

	inline void move_to_front(PetesNode *node) {
		if (Back == Front || node == Front)
			return;

		node->Prev->Next = node->Next;
		
		if (node != Back) {
			node->Next->Prev = node->Prev;
		}
		else {
			Back = node->Prev;
			Back->Next = nullptr;
		}
		Front->Prev = node;

		node->Next = Front;
		node->Prev = nullptr;
		Front = node;
	}

	PetesNode *Front, *Back;

	
};

