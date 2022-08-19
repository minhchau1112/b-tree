// B Tree
#include<iostream>
#define M 5
#define MIN (M-1)/2
using namespace std;

struct Node {
	int count; // số lượng khóa trên mỗi nút
	int value[M]; // mảng các giá trị của các khóa trên nút
	Node* child[M+1]; // địa chỉ các con của nút
};

struct BTree {
	Node* root;
};

struct QNode {
	int data;
	QNode* next;
};

struct Queue {
	QNode* front;
	QNode* rear;
};

void Init(Queue& q);
QNode* CreateNewNode(int value);
void Enqueue(Queue& q, int value);
void Dequeue(Queue& q);
void Enqueue_Node(Queue& q, Node* p);
void Initialize(BTree& b);
int findNode(Node* cur, int value, int& pos);
Node* search(Node* cur, int value);
void BFT(Node* cur);
void DFT(Node* cur);
Node* findPos(Node* cur, Node* parent, Node*& grandfather, int value, int& pos, bool& flag);
void QuickSort(int* a, int l, int r);
void Allocated(Node*& p);
Node* Create(int value);
void split(Node*& cur, Node*& parent, Node*& root, int& value);
void InsertNode(Node*& root, int value);
bool isLeaf(Node* p);
void clear(Node*& del, int pos);
bool isExistNode(Node* p, int value);
int findSibling(Node* del, Node* parent, int value, Node*& siblingLeft, Node*& siblingRight);
void borrowSibling(Node* del, Node* parent, Node*& siblingLeft, Node*& siblingRight, int pos, int& id);
void merge(Node*& root, Node*& del, Node*& parent, Node*& siblingLeft, Node*& siblingRight, int& pos, int id);
void DeleteNode(Node*& root, int value);

void Init(Queue& q) {
	q.front = NULL;
	q.rear = NULL;
}

QNode* CreateNewNode(int value) {
	QNode* p = new QNode;
	if (p == NULL)return NULL;
	p->data = value;
	p->next = NULL;
	return p;
}

void Enqueue(Queue& q, int value) {
	QNode* temp = CreateNewNode(value);
	if (q.rear == NULL) {
		q.rear = temp;
		q.front = temp;
		return;
	}
	q.rear->next = temp;
	q.rear = temp;
}

void Dequeue(Queue& q) {
	if (q.front == NULL) return;
	QNode* temp = q.front;
	q.front = q.front->next;
	if (q.front == NULL) {
		q.rear = NULL;
	}
	delete temp;

}

void Enqueue_Node(Queue& q, Node* p) {
	if (p != NULL) {
		for (int i = 0; i < p->count; i++) {
			Enqueue(q, p->value[i]);
		}
	}
}

void Initialize(BTree& b) {
	b.root = NULL;
}

int findNode(Node* cur, int value, int& pos) {
	if(cur == NULL)return 0;
	if (value < cur->value[0]) { // Nếu giá trị cần tìm nhỏ hơn khóa đầu tiên thì gán pos = 0 rồi return 0
		pos = 0;
		return 0;
	} // Nếu lớn hơn khóa đầu tiên thì
	else {
		pos = cur->count - 1; // gán pos = vị trí của khóa cuối cùng
		while (value < cur->value[pos] && pos>0)pos--; // tìm vị trí nhỏ nhất mà tại đó giá trị cần tìm nhỏ hơn khóa bên phải
		pos++; // vị trí của nút con thích hợp để chèn node vào nếu cần
		// nếu giá trị cần tìm = với giá trị ở vị trí nhỏ nhất mà tại đó nhỏ hơn khóa bên phải thì return 1, khác thì return 0
		if (value == cur->value[pos - 1])return 1;
		else return 0;
	}
}

Node* search(Node* cur, int value) {
	int pos;
	if (cur == NULL) return NULL;
	/* nếu tìm thấy giá trị đấy trong cây thì trả về địa chỉ của nút đó, còn vẫn chưa tìm thấy thì tiếp
	tục tìm ở nút con thích hợp.
	*/
	if (findNode(cur, value, pos))return cur;
	else return search(cur->child[pos], value);
}

// Duyệt theo chiều rộng
void BFT(Node* cur) {
	Queue q;
	Init(q);
	Node* t = cur;
	Enqueue_Node(q, t); // Lần luợt thêm các khóa của nút gốc vào queue;
	// Lặp lại vòng lặp khi t khác NULL.
	while (t) {
		int i = 0;
		while (i < t->count) {
			cout << q.front->data << " "; // In ra giá trị đầu trong queue
			Enqueue_Node(q, t->child[i]); // Lần lượt thêm các khóa của nút con thứ i của t vào queue 
			i++;
			Dequeue(q); // Lấy giá trị đầu trong queue ra
		}
		Enqueue_Node(q, t->child[i]); // Thêm khóa của nút con cuối cùng của t vào queue
		// Nếu vẫn còn giá trị trong queue thì tìm kiếm nút chứa giá trị đấy trong cây, nếu đã hết giá trị thì kết thúc.
		if (q.front != NULL) {
			t = search(cur, q.front->data);
		}
		else return;
	}
}

// Duyệt theo chiều sâu
void DFT(Node* cur) {
	if (cur == NULL)return;
	// Lần lượt in ra các khóa trong nút
	for (int i = 0; i < cur->count; i++) {
		cout << cur->value[i] << " ";
	}
	for (int j = 0; j <= cur->count; j++) {
		DFT(cur->child[j]); // Lần lượt thăm các nút con.
	}
}

Node* findPos(Node* cur, Node* parent,Node*& grandfather, int value, int& pos, bool& flag) {
	// Nếu đã tìm thấy vị trí thích hợp thì return parent
	if (cur == NULL) return parent;
	// Nếu giá trị cần thêm đã tồn tại thì gán Flag = true (đánh dấu đã có trong cây)
	if (findNode(cur, value, pos)) {
		flag = true;
		grandfather = parent;
		return cur;
	}
	else {
		// Nếu giá trị cần thêm chưa có trong cây thì gán grandfather = parent, parent = cur, và tiếp tục tìm kiếm ở nút con thích hợp
		grandfather = parent;
		parent = cur;
		findPos(cur->child[pos], parent,grandfather, value, pos, flag);
	}
}

void QuickSort(int* a, int l, int r) {
	int pivot = a[(l + r) / 2];
	int i = l, j = r;
	while (i <= j) {
		while (a[i] < pivot)i++;
		while (a[j] > pivot)j--;
		if (i <= j) {
			swap(a[i], a[j]);
			i++; j--;
		}
	}
	if (j > l)QuickSort(a, l, j);
	if (i < r)QuickSort(a, i, r);
}

// Cấp phát vùng nhớ cho các nút con
void Allocated(Node*& p) {
	*p->child = NULL;
	for (int i = 0; i <= M; i++) {
		p->child[i] = NULL;
	}
}

// Khởi tạo nút mới.
Node* Create(int value) {
	Node* p = new Node;
	p->value[0] = value;
	p->count = 1;
	Allocated(p);
	return p;
}

void split(Node*& cur, Node*& parent, Node*& root, int &value) {
	int pos = (cur->count - 1) / 2; // lấy vị trí ở giữa nút cur
	value = cur->value[pos]; // lấy giá trị chính giữa đấy
	Node* branch_new = new Node; // tạo nhánh mới
	for (int i = 0; i < pos; i++) { 
		branch_new->value[i] = cur->value[i + pos +1]; // nhánh mới chứa các khóa bên phải pos của nút cur.
	}
	for (int j = cur->count; j > pos; j--) {
		cur->value[j - 1] = cur->value[j]; // chỉ giữ lại các khóa bên trái pos của nút cur
	}
	branch_new->count = pos; // cập nhật số lượng khóa của nhánh mới = pos
	Allocated(branch_new); // cấp phát bộ nhớ cho các con của nhánh mới
	if (parent == NULL) { // nếu nút parent NULL thì 
		Node* tmp = root; 
		Node* root_new = Create(value); // khởi tạo nút gốc mới chứa 1 khóa value
		for (int a = 0; a < 3; a++) { 
			branch_new->child[a] = cur->child[a + 1 + pos]; // nhánh mới chứa các nút con là các nút con nằm bên phải pos của nút cur
		}
		for (int b = cur->count; b > pos; b--) {
			cur->child[b] = NULL; // gán các nút con bên phải pos của nút cur = NULL (chỉ giữ các nút con bên trái pos)
		}
		root_new->child[0] = cur; // nút gốc mới có con trái là nút cur
		root_new->child[1] = branch_new; // nút gốc mới có con phải là nút nhánh mới
		root = root_new; // cập nhật lại nút gốc
	}
	else { // nếu nút parent khác NULL thì
		parent->value[parent->count] = value; // thêm khóa value vào nút parent
		parent->count++; // cập nhật lại số lượng khóa của nút parent
		QuickSort(parent->value, 0, parent->count - 1); // sắp xếp lại nút parent
		int k;
		if (findNode(parent, value, k) == 0)return; // lấy k là vị trí của value trong nút parent
		for (int z = parent->count; z > k; z--) { 
			parent->child[z] = parent->child[z-1]; // dời các nút con bên phải k sang nút con kế bên; 
		}
		parent->child[k] = branch_new; // thêm nút con mới thứ k là nhánh mới.
	}
	cur->count = pos; // cập nhật lại số lượng khóa của nút cur
}

void InsertNode(Node*& root, int value) {
	int pos;
	bool flag = false; // flag kiểm soát xem giá trị cần thêm đã có trong cây chưa
	Node* parent = NULL; // nút cha của nút chứa khóa mới
	Node* newNode = findPos(root, NULL, parent, value, pos, flag); //tìm địa chỉ của nút thích hợp để chèn khóa value vào
	// Nếu giá trị cần thêm đã có trong cây thì return, còn chưa thì tiếp tục
	if (flag) return;
	// Nếu cây rỗng thì khởi tạo nút gốc
	if (root == NULL) {
		root = Create(value);
		return;
	}
	newNode->value[newNode->count] = value; // chèn khóa mới vào 
	newNode->count++; // cập nhật lại số lượng khóa của nút chứa khóa mới
	QuickSort(newNode->value, 0, newNode->count - 1); // sắp xếp lại nút chứa khóa mới
	if (newNode->count > M-1) { // nếu số lượng khóa của nút chứa khóa mới nhiều hơn lượng khóa tối đa
		bool flag2 = false; // đánh dấu xem thỏa mãn điều kiện không lớn hơn lượng khóa tối đa không
		while (flag2 == false) {
			Node* p = parent;
			Node* n = newNode;
			split(n, p,root, value); // tách nút làm đôi, đẩy giá trị chính giữa nên nút cha.
			if (p == NULL) return;
			if (p->count < M) flag2 = true; // nếu nút cha có số lượng khóa nhỏ hơn lượng khóa tối đa thì ngưng lặp
			parent = NULL;
			newNode = findPos(root, NULL, parent, value, pos, flag);
		}
	}
}

// kiểm tra xem nút đó có phải nút lá không
bool isLeaf(Node* p) {
	if (p == NULL)return false;
	for (int i = 0; i <= p->count; i++) {
		if (p->child[i] != NULL)return false;
	}
	return true;
}

// Hàm xóa một khóa trong nút
void clear(Node*& del, int pos) {
	// dồn các giá trị đằng sau pos lên.
	for (int i = pos + 1; i <= del->count; i++) {
		del->value[i - 1] = del->value[i];
	}
	// nếu nút chỉ có đúng một khóa đấy thì gán số lượng khóa = 0, và cho địa chỉ của nút đấy = NULL
	if (del->count == 1) {
		del->count = 0;
		del = NULL;
	} // nếu nút có nhiều hơn 1 khóa thì giảm số lượng khóa đi một đơn vị
	else if (del->count > 1) {
		del->count--;
	}
}

// kiểm tra xem một giá trị có tồn tại trong nút không
bool isExistNode(Node* p,int value) {
	if (p == NULL) return false;
	for (int i = 0; i < p->count; i++) {
		if (p->value[i] == value) return true;
	}
	return false;
}

int findSibling(Node* del, Node* parent, int value, Node*& siblingLeft, Node*& siblingRight) {
	int id = 0;
	// tìm kiếm vị trí của nút con của parent chứa value 
	while (id <= parent->count) {
		if (isExistNode(parent->child[id], value)) break;
		id++;
	}
	// Nếu nút con đấy không nằm ngoài cùng bên trái thì :
	if (id > 0) {
		siblingLeft = parent->child[id - 1]; // có anh em trái
		if (id < parent->count) { // nếu nút con đấy không nằm ngoài cùng bên phải thì có anh em phải
			siblingRight = parent->child[id + 1]; 
		}
	} 
	else { // Nếu nút con đấy nằm ngoài cùng bên trái thì nó chỉ có anh em bên phải là :
		siblingRight = parent->child[id + 1];
	}
	return id;
}

// hàm mượn khóa của anh em nếu anh em dư khóa
void borrowSibling(Node* del, Node* parent,Node*& siblingLeft,Node*& siblingRight, int pos,int &id) {
	// nếu anh em trái có tồn tại và dư khóa thì mượn :
	if (siblingLeft != NULL && siblingLeft->count > MIN) {
		del->value[pos] = parent->value[id-1]; // đẩy khóa thích hợp của nút cha xuống 
		QuickSort(del->value, 0, del->count-1); // sắp xếp lại nút 
		parent->value[id-1] = siblingLeft->value[siblingLeft->count - 1];  // thế chỗ khóa cha bằng khóa anh em trái ngoài cùng bên phải
		clear(siblingLeft, siblingLeft->count - 1); // xóa khóa anh em trái ngoài cùng bên phải đấy
	} // nếu anh em phải có tồn tại và dư khóa thì mượn : 
	else if (siblingRight != NULL && siblingRight->count > MIN) {
		del->value[pos] = parent->value[id]; // đẩy khóa thích hợp của nút cha xuống
		QuickSort(del->value, 0, del->count-1); // sắp xếp lại nút
		parent->value[id] = siblingRight->value[0]; // thế chỗ khóa cha bằng khóa anh em phải ngoài cùng bên trái 
		clear(siblingRight,0); // xóa khóa anh em phải ngoài cùng bên trái đó
	}
}

// hàm hợp nhất lại với anh em nếu anh em không dư khóa
void merge(Node*& root, Node*& del, Node*& parent, Node*& siblingLeft, Node*& siblingRight, int &pos,int id) {
	clear(del, pos); // xóa khóa đấy
	if (siblingLeft != NULL) { // nếu có anh em trái thì : 
		siblingLeft->value[siblingLeft->count] = parent->value[id - 1]; // đẩy khóa cha xuống nút anh em trái
		siblingLeft->count++; // cập nhật lại số lượng khóa của anh em trái
		// hợp nhất lại với anh em bằng cách dồn hết qua bên phải của anh em trái
		for (int i = 0; i < del->count; i++) {
			siblingLeft->value[siblingLeft->count] = del->value[i];
			siblingLeft->count++;
		}
		// đẩy các nút con từ thứ id của parent qua bên trái một nút 
		for (int j = id; j < parent->count; j++) {
			parent->child[j] = parent->child[j + 1];
		}
		// cho nút con ngoài cùng bên phải của parent bằng NULL
		parent->child[parent->count] = NULL;
		delete del; 
		pos = id - 1; //lưu lại vị trí của khóa cha
	}
	else { // nếu có anh em phải thì :
		del->value[del->count] = parent->value[id]; // đẩy khóa cha xuống nút del
		del->count++; // cập nhật lại số lượng khóa của nút del
		int z = del->count; // giữ số lượng khóa của nút del trước khi hợp nhất với anh em
		// hợp nhất với anh em phải bằng cách dồn hết khóa của anh em phải vào 
		for (int i = 0; i < siblingRight->count; i++) {
			del->value[del->count] = siblingRight->value[i];
			del->count++;
		}
		// nếu nút cha chính là nút gốc thì :
		if (parent == root) {
			// dồn các nút con của anh em phải qua
			for (int k = 0; k <= siblingRight->count; k++) {
				del->child[z + k] = siblingRight->child[k];
			}
			parent = NULL;
			Node* tmp = root;
			root = del; // cập nhật lại nút gốc chính là nút del sau khi hợp nhất
			delete tmp; // xóa nút gốc cũ
		} // nếu nút cha không phải nút gốc thì :
		else {
			// đẩy các nút con từ thứ id+1 của parent qua bên trái một nút 
			for (int j = id + 1; j < parent->count; j++) {
				parent->child[j] = parent->child[j + 1];
			}
			parent->child[parent->count] = NULL;
			delete siblingRight; // xóa nút anh em phải
			pos = id; // lưu lại vị trí của khóa cha
		}
	}
}


void DeleteNode(Node*& root,int value) {
	int pos;
	Node* parent = NULL; // nút cha của nút chứa khóa cần xóa
	bool flag = false;
	Node* del = findPos(root, NULL, parent, value, pos, flag); // tìm nút chứa khóa cần xóa
	if (del == NULL) return;
	pos--; // pos là vị trí của khóa trong nút đó
	bool stop = false; 
	while (stop == false) {
		// nếu nút chứa khóa cần xóa là nút lá thì :
		if (isLeaf(del)) {
			while (stop == false) {
				// nếu sau khi xóa khóa kia đi mà số lượng khóa trong node bị thiếu thì :
				if (del->count - 1 < MIN) {
					// Nút anh em trái và anh em phải
					Node* siblingLeft = NULL, * siblingRight = NULL;
					// id là vị trí của nút con chứa khóa cần xóa của parent
					int id = findSibling(del, parent, value, siblingLeft, siblingRight);
					int c1 = (siblingLeft) ? siblingLeft->count : 0; // c1 là số lượng khóa của anh em trái
					int c2 = (siblingRight) ? siblingRight->count : 0; // c2 là số lượng khóa của anh em phải
					// Nếu anh em của nó dư khóa thì mượn rồi dừng.
					if (c1 > MIN|| c2 > MIN) {
						borrowSibling(del, parent, siblingLeft, siblingRight, pos, id);
						stop = true;
					} // Nếu anh em của nó không dư khóa thì :
					else {
						// hợp nhất với anh em
						merge(root, del, parent, siblingLeft, siblingRight, pos, id);
						// nếu nút cha = NULL thì kết thúc
						if (parent == NULL)return;
						// xóa khóa cha
						value = parent->value[pos];
						parent = NULL;
						del = findPos(root, root, parent, value, pos, flag);
						pos--;
					}
				}
				else { // Nếu sau khi xóa khóa đấy mà vẫn không bị thiếu khóa thì 
					// xóa khóa đấy trong nút rồi dừng
					clear(del, pos);
					stop = true;
				}
			}
		}
		else {
			Node* child_left = del->child[pos]; // địa chỉ nút con trái của khóa cần xóa
			Node* child_right = del->child[pos + 1]; // điạ chỉ nút con phải của khóa cần xóa
			// Nếu nút con phải khác NULL thì 
			if (child_left != NULL) {
				Node* tmp = child_left->child[child_left->count];
				while (tmp != NULL) { //Tìm nút con phải nhất 
					child_left = tmp;
					tmp = tmp->child[tmp->count];
				}
				// thế giá trị cần xóa = giá trị lớn nhất của nút con phải nhất
				del->value[pos] = child_left->value[child_left->count - 1];
				// sau đấy xóa khóa lớn nhất của nút con phải nhất.
				value = child_left->value[child_left->count - 1];
				parent = del;
				del = findPos(child_left, del, parent, value, pos, flag);
				pos--;
			}
			else { // Nếu nút con phải NULL, con trái khác NULL thì
				Node* tmp2 = child_right->child[0];
				while (tmp2 != NULL) { // tìm nút con trái nhất
					child_right = tmp2;
					tmp2 = tmp2->child[0];
				}
				// thế giá trị cần xóa = giá trị nhỏ nhất của nút con trái nhất
				del->value[pos] = child_right->value[0];
				// sau đấy xóa khóa nhỏ nhất của nút con trái nhất.
				value = child_right->value[0];
				parent = del;
				del = findPos(child_right, del, parent, value, pos, flag);
				pos--;
			}
		}
	}
}

int main() {
	BTree b;
	Initialize(b);
	InsertNode(b.root, 3);
	InsertNode(b.root, 7);
	InsertNode(b.root, 9);
	InsertNode(b.root, 23);
	InsertNode(b.root, 45);
	InsertNode(b.root, 1);
	InsertNode(b.root, 5);
	InsertNode(b.root, 14);
	InsertNode(b.root, 25);
	InsertNode(b.root, 24);
	InsertNode(b.root, 13);
	InsertNode(b.root, 11);
	InsertNode(b.root, 8);
	InsertNode(b.root, 19);
	InsertNode(b.root, 4);
	InsertNode(b.root, 31);
	InsertNode(b.root, 35);
	InsertNode(b.root, 56);
	InsertNode(b.root, 2);
	InsertNode(b.root, 6);
	InsertNode(b.root, 12);
	InsertNode(b.root, 28);
	cout << "DFT : ";
	DFT(b.root);
	cout << endl << "BFT : ";
	BFT(b.root);
	return 0;
}