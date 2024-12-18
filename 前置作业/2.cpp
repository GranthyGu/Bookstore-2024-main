#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

struct Node {
    string index;
    vector<int> values;
    Node* next;
    Node(string str) : index(str), next(nullptr) {}
};

class BlockLinked {
private:
    const int maximal = 1000;
    const int minimal = 500;
    Node* head;

    Node* findnode(string index) {
        Node* cur = head;
        while (cur) {
            if (cur->index == index) {
                return cur;
            }
            cur = cur->next;
        }
        return nullptr;
    }

    void splitnode(Node* node) {
        int mid = node->values.size() / 2;
        vector<int> tmp_(node->values.begin() + mid, node->values.end());
        node->values.resize(mid);
        Node* newnode = new Node(node->index);
        newnode->values = tmp_;
        newnode->next = node->next;
        node->next = newnode;
    }
public:
    void saveToFile() {
        ofstream outFile("data.dat", ios::binary | ios::trunc);
        Node* cur = head;
        while (cur) {
            int len = cur->index.size();
            outFile.write(reinterpret_cast<char*>(&len), sizeof(int));
            outFile.write(cur->index.c_str(), len);
            int valueCount = cur->values.size();
            outFile.write(reinterpret_cast<char*>(&valueCount), sizeof(int));
            outFile.write(reinterpret_cast<char*>(cur->values.data()), sizeof(int) * valueCount);
            cur = cur->next;
        }
        outFile.close();
    }

    void loadFromFile() {
        ifstream inFile("data.dat", ios::binary);
        if (!inFile) return;
        while (inFile) {
            int len;
            inFile.read(reinterpret_cast<char*>(&len), sizeof(int));
            if (inFile.eof()) break;
            char* buffer = new char[len + 1];
            inFile.read(buffer, len);
            buffer[len] = '\0';
            string index(buffer);
            delete[] buffer;
            int valueCount;
            inFile.read(reinterpret_cast<char*>(&valueCount), sizeof(int));
            vector<int> values(valueCount);
            inFile.read(reinterpret_cast<char*>(values.data()), sizeof(int) * valueCount);
            Node* node = new Node(index);
            node->values = values;
            node->next = head;
            head = node;
        }
        inFile.close();
    }

    BlockLinked() : head(nullptr) {
        loadFromFile();
    }

    ~BlockLinked() {
        Node* cur = head;
        while (cur) {
            Node* tmp = cur->next;
            delete cur;
            cur = tmp;
        }
    }

    void insert(string index, int value) {
        Node* node = findnode(index);
        if (!node) {
            Node* newNode = new Node(index);
            newNode->values.push_back(value);
            newNode->next = head;
            head = newNode;
            return;
        } else {
            while (node->next && !node->values.empty() && node->values.back() < value && node->next->index == index) {
                node = node->next;
            }
            if (binary_search(node->values.begin(), node->values.end(), value)) {
                return;
            }
            node->values.push_back(value);
            sort(node->values.begin(), node->values.end());
            if (node->values.size() >= maximal) {
                splitnode(node);
            }
        }
    }

    void remove(string index, int value) {
    Node* node = findnode(index);
    if (!node) {
        return;
    }
    while (node && node->index == index) {
        auto it = find(node->values.begin(), node->values.end(), value);
        if (it != node->values.end()) {
            node->values.erase(it);
        }
        if (node->values.empty()) {
            if (node == head) {
                head = node->next;
            } else {
                Node* tmp = head;
                while (tmp->next != node) {
                    tmp = tmp->next;
                }
                tmp->next = node->next;
            }
            Node* tmp = node;
            node = node->next;
            delete tmp;
        } else {
            node = node->next;
        }
    }
}

    void Find(string index) {
        Node* node = findnode(index);
        if (!node) {
            cout << "null" << endl;
        } else {
            bool hasValue = false;
            while (node && node->index == index) {
                if (!node->values.empty()) {
                    hasValue = true;
                    for (auto i : node->values) {
                        cout << i << ' ';
                    }
                }
                node = node->next;
            }
            if (!hasValue) {
                cout << "null";
            }
            cout << endl;
        }
    }
};

int main() {
    BlockLinked bl;
    int n;
    cin >> n;
    string operation;
    string index;
    int value;
    for (int i = 0; i < n; i++) {
        cin >> operation;
        if (operation == "insert") {
            cin >> index;
            cin >> value;
            bl.insert(index, value);
        }
        if (operation == "delete") {
            cin >> index;
            cin >> value;
            bl.remove(index, value);
        }
        if (operation == "find") {
            cin >> index;
            bl.Find(index);
        }
    }
    bl.saveToFile();
    return 0;
}