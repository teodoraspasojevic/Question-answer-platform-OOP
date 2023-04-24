#pragma once

#include <exception>
#include <iostream>
#include <list>
#include <queue>
#include <stack>
#include <string>
#include <vector>
using namespace std;

class QuestionNotInPlatform : public exception {
public:
	QuestionNotInPlatform() : exception("Error. Pitanje se ne nalazi u platformi.") {}
};

class AnswerNotInPlatform : public exception {
public:
	AnswerNotInPlatform() : exception("Error. Odgovor se ne nalazi u platformi.") {}
};

class QuestionStars : public exception {
public:
	QuestionStars() : exception("Error. Pitanje se ne moze ocenjivati.") {}
};

class AnswerOverload : public exception {
public:
	AnswerOverload() : exception("Error. Dostignut je maksimalan broj komentara.") {}
};

class TreeNode {
public:
	TreeNode(const string& content) : content_(content), current_child_index_(0), father_(nullptr), visited_(false) {}

	friend bool operator==(const TreeNode& node1, const TreeNode& node2) {
		return node1.content_ == node2.content_;
	}
	friend bool operator!=(const TreeNode& node1, const TreeNode& node2) {
		return node1.content_ != node2.content_;
	}

	struct {
		bool operator()(TreeNode* a, TreeNode* b) const { return a->getStars() > b->getStars(); }
	} custom;

	struct {
		bool operator()(int a, int b) const { return a > b; }
	} custom1;

	TreeNode* getFather() const {
		return this->father_;
	}
	void setFather(TreeNode* father) {
		this->father_ = father;
	}
	string getContent() const {
		return this->content_;
	}
	int getNumberOfAnswers() const {
		return this->children_.size();
	}
	TreeNode* getIthChild(int i) const {
		if (this->children_.size() > 0) return children_[i];
		else nullptr;
	}
	vector<TreeNode*> getChildren() const {
		return this->children_;
	}
	vector<int> getStarsVector() const {
		return this->stars_;
	}
	virtual int getStars() const = 0;

	void incrementeIthChildStar(int i) {
		this->stars_[i]++;
	}

	// pomocne metode za obilazak stabla
	void visite() { this->visited_ = true; }
	void unvisite() { this->visited_ = false; }
	bool isVisited() { return this->visited_; }

	void addAnswer(TreeNode* node) {
		if (this->children_.size() == 10) {
			delete node;
			throw new AnswerOverload();
		}
		else {
			children_.push_back(node);
			node->setFather(this);
			stars_.push_back(0);
		}
	}

	void removeIthChild(int i) {
		vector<TreeNode*>::const_iterator it1 = this->children_.begin();
		vector<int>::const_iterator it2 = this->stars_.begin();
		for (int j = 0; j < i; j++) {
			it1++;
			it2++;
		}
		this->children_.erase(it1);
		this->stars_.erase(it2);
	}

	void sortChildren() {
		sort(this->children_.begin(), this->children_.end(), custom);
		sort(this->stars_.begin(), this->stars_.end(), custom1);
	}

	virtual void incrementeStars() = 0;
	virtual void print() const = 0;

protected:
	TreeNode* father_;
	vector<TreeNode*> children_;
	string content_;
	int current_child_index_;
	vector<int> stars_;
	bool visited_;
};

class Question : public TreeNode {
public:
	Question(const string& content) : TreeNode(content) {};

	int getStars() const override { throw new QuestionStars(); } 

	void incrementeStars() override { throw new QuestionStars(); }

	void print() const override { cout << this->content_ << endl; }
};

class Answer : public TreeNode {
public:
	Answer(const string& content) : TreeNode(content) {
		this->star_ = 0;
	}

	int getStars() const override {
		return this->star_;
	}
	void incrementeStars() override {
		this->star_++;
	}
	void print() const override {
		cout << this->content_ << "(" << this->getFather()->getContent() << "), Ocena: " << this->star_ << endl;
	}

private:
	int star_;
};

class Tree {
public:
	Tree() : root_(nullptr) {}
	~Tree() {
		this->deletePart(this->root_);
	}

	void addQuestion(const string& content) {
		TreeNode* node = new Question(content);
		if (!root_) {
			this->root_ = node;
		}
	}
	void addAnswerToQuestion(const string& content) { 
		this->root_->addAnswer(new Answer(content));
	}
	TreeNode* findAnswer(const string& content) {
		//levelorder

		TreeNode* curr = root_;
		queue<TreeNode*> unvisited;
		unvisited.push(curr);
		while (!unvisited.empty()) {
			curr = unvisited.front();
			unvisited.pop();
			if (curr->getContent() == content) return curr;
			for (int i = 0; i < curr->getNumberOfAnswers(); i++) {
				if (curr->getIthChild(i)) unvisited.push(curr->getIthChild(i));
			}
		}
		return nullptr;
	}

	string getQuestion() const { return this->root_->getContent(); }

	int getMaxStar() {
		//levelorder

		int max = -1;
		TreeNode* curr = root_;
		queue<TreeNode*> unvisited;
		unvisited.push(curr);
		while (!unvisited.empty()) {
			curr = unvisited.front();
			unvisited.pop();
			if (curr->getContent()!=this->root_->getContent())
				if (curr->getStars() > max) 
					max = curr->getStars();
			for (int i = 0; i < curr->getNumberOfAnswers(); i++) {
				if (curr->getIthChild(i)) unvisited.push(curr->getIthChild(i));
			}
		}
		return max;
	}

	void printTree() const {
		//levelorder

		TreeNode* curr = root_;
		queue<TreeNode*> unvisited;
		unvisited.push(curr);
		while (!unvisited.empty()) {
			curr = unvisited.front();
			unvisited.pop();
			curr->print();
			for (int i = 0; i < curr->getNumberOfAnswers(); i++) {
				if (curr->getIthChild(i)) unvisited.push(curr->getIthChild(i));
			}
		}
	}

	void sortByStars() {
		//levelorder

		TreeNode* curr = root_;
		queue<TreeNode*> unvisited;
		unvisited.push(curr);
		while (!unvisited.empty()) {
			curr = unvisited.front();
			unvisited.pop();
			if (curr->getChildren().size() > 1) curr->sortChildren();
			for (int i = 0; i < curr->getNumberOfAnswers(); i++) {
				if (curr->getIthChild(i)) unvisited.push(curr->getIthChild(i));
			}
		}
	}

	void deletePart(TreeNode* node) {
		//levelorder
		TreeNode* father = node->getFather();
		vector<TreeNode*> children;
		int i = 0;
		if (father) {
			children = father->getChildren();
			for (; father->getIthChild(i)->getContent() != node->getContent(); i++);
			father->removeIthChild(i);
		}

		TreeNode* curr = node;
		queue<TreeNode*> unvisited;
		unvisited.push(curr);
		while (!unvisited.empty()) {
			curr = unvisited.front();
			unvisited.pop();
			TreeNode* curr2 =  curr;
			for (int i = 0; i < curr2->getNumberOfAnswers(); i++) {
				if (curr2->getIthChild(i)) unvisited.push(curr2->getIthChild(i));
			}
			delete curr;
		}

	}

	/*
	void preorder(TreeNode* node) {
		stack<TreeNode*> unvisited;
		unvisited.push(node);
		while (!unvisited.empty()) {
			TreeNode* curr = unvisited.top();
			unvisited.pop();
			while (curr) {
				//Fja
				if (curr->getNumberOfAnswers() > 1) {
					for (int i = curr->getNumberOfAnswers() - 1; i >= 0; i--) unvisited.push(curr->getIthChild(i));
				}
				curr = curr->getIthChild(0);
			}
		}
	}
	*/

private:
	TreeNode* root_;

};

class QuestionPlatform {
public:
	QuestionPlatform() = default;
	~QuestionPlatform() {
		int i = 0;
		while (i < this->questions_.size()) {
			Tree* curr = this->questions_[i];
			if (curr) delete curr;
			i++;
		}
	}

	int getNumberOfQuestions() const { return this->questions_.size(); }

	void addQuestion(const string& content) {
		Tree* tree = new Tree();
		tree->addQuestion(content);
		this->questions_.push_back(tree);
	}

	void addAnswer(const string& content1, const string& content2) {
		vector<Tree*>::const_iterator i;
		Tree* tree = findQuestion(content1, i);
		if (tree) tree->addAnswerToQuestion(content2);
		else {
			TreeNode* node = this->findAnswer(content1);
			if (node) node->addAnswer(new Answer(content2));
		}
	}

	Tree* findQuestion(const string& content, vector<Tree*>::const_iterator& i) const {
		i = questions_.begin();
		while (i < this->questions_.end()) {
			Tree* node = *i;
			if (node->getQuestion() == content) return node;
			i++;
		}
		return nullptr;
	}

	Tree* findQuestionWithMaxStars() {
		int max_val = this->questions_[0]->getMaxStar();
		int max_index = 0;
		if (this->getNumberOfQuestions() == 1) return this->questions_[0];
		for (int i = 1; i < this->getNumberOfQuestions(); i++) {
			if (this->questions_[i]->getMaxStar() > max_val) {
				max_val = this->questions_[i]->getMaxStar();
				max_index = i;
			}
		}
		return this->questions_[max_index];
	}

	TreeNode* findAnswer(const string& content) {
		int i = 0;
		while (i < questions_.size()) {
			TreeNode* node = this->questions_[i]->findAnswer(content);
			if (node) return node;
			i++;
		}
		return nullptr;
	}

	TreeNode* findAnswerByQuestion(Tree* question, const string& content) {
		int i = 0;
		for (; this->questions_[i]->getQuestion() != question->getQuestion(); i++);
		return this->questions_[i]->findAnswer(content);
	}

	void incrementeStars(const string& content) {
		TreeNode* answer = this->findAnswer(content);
		if (!answer) throw new AnswerNotInPlatform();
		answer->incrementeStars();
		int i = 0;
		TreeNode* father = answer->getFather();
		vector<TreeNode*> children = father->getChildren();
		for (; children[i]->getContent() != content; i++);
		answer->getFather()->incrementeIthChildStar(i);
	}

	void sortByStars() {
		for (int i = 0; i < this->getNumberOfQuestions(); i++) this->questions_[i]->sortByStars();
	}

	void printPlatform() const {
		int i = 0;
		cout << "Pitanja:" << endl;
		while (i < this->questions_.size()) {
			cout << this->questions_[i]->getQuestion() << endl;
			i++;
		}
	}

	void removeElem(vector<Tree*>::const_iterator position) {
		this->questions_.erase(position);
	}

private:
	vector<Tree*> questions_;

};

int main() {

	bool flag = false;
	QuestionPlatform* platform = nullptr;

	while (1) {

		cout << endl;
		cout << "Meni:" << endl;
		cout << "1. Kreiraj sistem pitanja." << endl;
		cout << "2. Postavi pitanje." << endl;
		cout << "3. Odgovari na pitanje." << endl;
		cout << "4. Pronadji pitanje." << endl;
		cout << "5. Pronadji odgovor." << endl;
		cout << "6. Inkrementiraj ocenu." << endl;
		cout << "7. Uredi pitanja po njihovoj oceni." << endl;
		cout << "8. Ispisi pitanja." << endl;
		cout << "9. Ispisi pitanje, sa njegovim odgovorima." << endl;
		cout << "10. Obrisi odgovor." << endl;
		cout << "11. Obrisi pitanje." << endl;
		cout << "0. Prekini sa radom programa." << endl;
		cout << endl;

		cout << "Izaberite opciju: ";
		int option;
		cin >> option;

		if (!flag && option != 1) {
			cout << "Morate prvo inicijalizovati platformu sa pitanjima. Molimo Vas, izaberite opciju 1." << endl;
			continue;
		}

		try {

			switch (option) {

			case 0: {
				delete platform;
				exit(0);
				break;
			}
			case 1: {
				flag = true;
				platform = new QuestionPlatform();
				break;
			}
			case 2: {
				cout << "Postavite pitanje: ";
				string txt;
				getline(cin, txt);
				string content;
				getline(cin, content);
				cout << endl;
				platform->addQuestion(content);
				break;
			}
			case 3: {
				cout << "Da li zelite da odgovorite na pitanje (1) ili drugi komentar (2)?" << endl;
				cout << "Opcija: ";
				int option2;
				cin >> option2;
				if (option2 != 1 && option2 != 2) cout << "Error. Uneli ste pogresnu opciju" << endl << endl;
				else {
					cout << "Napisite na sta zelite da odgovorite: ";
					string txt;
					getline(cin, txt);
					string content;
					getline(cin, content);
					cout << endl;
					if (option2 == 1) {
						vector<Tree*>::const_iterator i;
						Tree* question = platform->findQuestion(content, i);
						if (!question) throw new QuestionNotInPlatform();
						cout << "Napisite Vas odgovor: ";
						string content1;
						getline(cin, content1);
						cout << endl;
						question->addAnswerToQuestion(content1);
					}
					else {
						TreeNode* answer = platform->findAnswer(content);
						if (!answer) throw new AnswerNotInPlatform();
						cout << "Napisite Vas komentar: ";
						string content1;
						getline(cin, content1);
						cout << endl;
						answer->addAnswer(new Answer(content1));
					}
				}
				break;
			}
			case 4: {
				if (platform->getNumberOfQuestions() == 0) cout << "Error. Niste postavili nijedno pitanje na platformi." << endl << endl;
				else {
					Tree* question = platform->findQuestionWithMaxStars();
					cout << "Pitanje sa najvisom ocenom na nekom od njegovih komentara: " << question->getQuestion() << endl << endl;
				}
				break;
			}
			case 5: {
				cout << "Unesite tekst pitanja ciji biste odgovor da nadjemo: ";
				string txt;
				getline(cin, txt);
				string content;
				getline(cin, content);
				cout << endl;
				vector<Tree*>::const_iterator i;
				Tree* question = platform->findQuestion(content, i);
				if (!question) throw new QuestionNotInPlatform();
				cout << "Unesite tekst odgovora koji biste da pronadjemo: ";
				//string txt1;
				//getline(cin, txt1);
				string content1;
				cin >> content1;
				TreeNode* answer = platform->findAnswerByQuestion(question, content1);
				if (!answer) throw new AnswerNotInPlatform();
				else cout << "Pretraga uspesna." << endl << endl;
				break;
			}	
			case 6: {
				cout << "Unesite tekst odgovora ciju biste ocenu da povecate: ";
				string txt;
				getline(cin, txt);
				string content;
				getline(cin, content);
				cout << endl;
				platform->incrementeStars(content);
				break;
			}
			case 7:
				platform->sortByStars();
				break;
			case 8: {
				platform->printPlatform();
				break;
			}
			case 9: {
				cout << "Unesite tekst pitanja koje biste da se odstampa: ";
				string txt;
				getline(cin, txt);
				string content;
				getline(cin, content);
				cout << endl;
				vector<Tree*>::const_iterator i;
				Tree* question = platform->findQuestion(content, i);
				if (!question) throw new QuestionNotInPlatform();
				question->printTree();
				break;
			}
			case 10: {
				cout << "Unesite pitanje ciji odgovor biste da obrisete: ";
				string txt;
				getline(cin, txt);
				string content;
				getline(cin, content);
				cout << endl;
				vector<Tree*>::const_iterator i;
				Tree* question = platform->findQuestion(content, i);
				if (!question) throw new QuestionNotInPlatform();
				cout << "Unesite odgovor koji biste da se obrise: ";
				string content1;
				getline(cin, content1);
				cout << endl;
				TreeNode* answer = platform->findAnswer(content1);
				if (!answer) throw new AnswerNotInPlatform();
				question->deletePart(answer);
				break;
			}
			case 11: {
				cout << "Unesite pitanje koje biste da se obrise: ";
				string txt;
				getline(cin, txt);
				string content;
				getline(cin, content);
				cout << endl;
				vector<Tree*>::const_iterator i;
				Tree* question = platform->findQuestion(content, i);
				if (!question) throw new QuestionNotInPlatform();
				delete question;
				platform->removeElem(i);
				break;
			}
			default: {
				cout << "Trazena opcija se ne nalazi u meniju. Molimo Vas, pokusajte ponovo." << endl << endl;
				break;
			}
			}

		}
		catch (exception* e) {
			cout << endl << e->what() << endl << endl;
			delete e;
			continue;
		}
	}
}
	
	
