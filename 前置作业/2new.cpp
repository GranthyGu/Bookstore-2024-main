#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

const int maxN = 200;
struct Node {
    char index[60] = {'0'};
    long value_pos;
    long next_offset;
    long next_offset_of_index;
    void writetoFile(fstream &File) {
        for (int i = 0; i < 60; i++) {
            File.write(reinterpret_cast<char*>(&index[i]), sizeof(char));
        }
        File.write(reinterpret_cast<char*>(&value_pos), sizeof(long));
        File.write(reinterpret_cast<char*>(&next_offset_of_index), sizeof(long));
        File.write(reinterpret_cast<char*>(&next_offset), sizeof(long));
    }
    void readfromFile(fstream &File) {
        for (int i = 0; i < 60; i++) {
            File.read(reinterpret_cast<char*>(&index[i]), sizeof(char));
        }
        File.read(reinterpret_cast<char*>(&value_pos), sizeof(long));
        File.read(reinterpret_cast<char*>(&next_offset_of_index), sizeof(long));
        File.read(reinterpret_cast<char*>(&next_offset), sizeof(long));
    }
};
class List {
private:
    bool empty;
    string fileofindex;
    string fileofvalues;
public:
    List(string str1, string str2) : fileofindex(str1), fileofvalues(str2), empty(true) {}
    vector<Node> findnode(string index) {
        fstream File(fileofindex, ios::in | ios::out | ios::binary);
        if (!File) {
            File.open(fileofindex, ios::out | ios::binary);
        }
        Node tmp;
        vector<Node> tmpp;
        long cur_pos = 0;
        if (empty) {
            return tmpp;
        }
        while (true) {
            File.seekg(cur_pos);
            tmp.readfromFile(File);
            bool flag = true;
            for (int i = 0; i < index.size(); i++) {
                if (tmp.index[i] != index[i]) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                tmpp.push_back(tmp);
                cur_pos = tmp.next_offset_of_index;
                if (tmp.next_offset_of_index == -1) {
                    break;
                }
            } else {
                cur_pos = tmp.next_offset;
                if (tmp.next_offset == -1) {
                    break;
                }
            }
        }
        return tmpp;
    }
    void splitnode(Node node) {
        Node newnode;
        for (int i = 0; i < 60; i++)
        {
            newnode.index[i] = node.index[i];
        }
        newnode.next_offset = -1;
        newnode.next_offset_of_index = node.next_offset_of_index;
        fstream File(fileofindex, ios::in | ios::out | ios::binary);
        File.seekp(0, ios::end);
        long sizeofFile = File.tellp();
        long put_position = sizeofFile - sizeof(long);
        File.seekp(put_position);
        File.write(reinterpret_cast<char*>(&sizeofFile), sizeof(long));
        File.close();
        fstream File_(fileofvalues, ios::in | ios::out | ios::binary);
        File_.seekp(0, ios::end);
        File_.seekg(0, ios::end);
        long sizeofvalue = File_.tellg();
        long sizeofvalue_ = File_.tellp();
        newnode.value_pos = sizeofvalue;
        File_.seekg(node.value_pos + maxN / 2 * sizeof(int));
        vector<int> valuesofindex;
        for (int i = 0; i < maxN / 2; i++) {
            int tmp;
            File_.read(reinterpret_cast<char*>(&tmp), sizeof(int));
            valuesofindex.push_back(tmp);
        }
        File_.seekp(node.value_pos + maxN / 2 * sizeof(int));
        int c = -1;
        for (int i = 0; i < maxN / 2; i++) {
            File_.write(reinterpret_cast<char*>(&c), sizeof(int));
        }
        File_.seekp(sizeofvalue_);
        for (int i = 0; i < maxN / 2; i++) {
            File_.write(reinterpret_cast<char*>(&valuesofindex[i]), sizeof(int));
        }
        for (int i = 0; i < maxN / 2; i++) {
            File_.write(reinterpret_cast<char*>(&c), sizeof(int));
        }
        File_.close();
        fstream File__(fileofindex, ios::in | ios::out | ios::binary);
        File__.seekp(0, ios::end);
        node.next_offset_of_index = File.tellp();
        newnode.writetoFile(File__);
        File__.close();
    }
    void insert(string index, int value) {
        vector<Node> tmp = findnode(index);
        int num = tmp.size();
        if (num == 0) {
            Node node;
            fstream File_(fileofvalues, ios::in | ios::out | ios::binary);
            if (!File_) {
                File_.open(fileofvalues, ios::out | ios::binary);
            }
            File_.seekp(0, ios::end);
            node.value_pos = File_.tellp();
            File_.write(reinterpret_cast<char*>(&value), sizeof(int));
            int c = -1;
            for (int i = 0; i < maxN - 1; i++) {
                File_.write(reinterpret_cast<char*>(&c), sizeof(int));
            }
            File_.close();
            for (int i = 0; i < index.size(); i++)
            {
                node.index[i] = index[i];
            }
            node.next_offset = -1;
            node.next_offset_of_index = -1;
            fstream File(fileofindex, ios::in | ios::out | ios::binary);
            if (!File) {
                File.open(fileofindex, ios::out | ios::binary);
            }
            if (!empty) {
                File.seekp(0, ios::end);
                long sizeofFile = File.tellp();
                long put_position = sizeofFile - sizeof(long);
                File.seekp(put_position);
                File.write(reinterpret_cast<char*>(&sizeofFile), sizeof(long));
                node.writetoFile(File);
                File.close();
            } else {
                node.writetoFile(File);
                File.close();
            }
            empty = false;
            return;
        }
        int i;
        bool flag = false;
        fstream File(fileofvalues, ios::in | ios::out | ios::binary);
        for (i = 0; i < num; i++) {
            File.seekg(tmp[i].value_pos);
            int tmp;
            File.read(reinterpret_cast<char*>(&tmp), sizeof(int));
            if (tmp > value) {
                flag = true;
                break;
            }
        }
        int target = max(0, i - 1); // tmp[target] is exactly where we insert the value
        File.seekg(tmp[target].value_pos);
        vector<int> valueoftarget;
        for (int i = 0; i < maxN; i++) {
            int tmp;
            File.read(reinterpret_cast<char*> (&tmp), sizeof(int));
            if (tmp == -1) {
                break;
            }
            valueoftarget.push_back(tmp);
        }
        if (find(valueoftarget.begin(), valueoftarget.end(), value) != valueoftarget.end()) {
            return;
        }
        valueoftarget.push_back(value);
        sort(valueoftarget.begin(), valueoftarget.end());
        bool flaggg = false;
        if (valueoftarget.size() == maxN) {
            flaggg = true;
        }
        File.seekp(tmp[target].value_pos);
        for (int i = 0; i < valueoftarget.size(); i++) {
            File.write(reinterpret_cast<char*> (&valueoftarget[i]), sizeof(int));
        }
        if (flaggg) {
            splitnode(tmp[target]);
        }
    }
    void remove(string index, int value) {
        vector<Node> tmp = findnode(index);
        int num = tmp.size();
        if (num == 0) {
            return;
        }
        int i;
        bool flag = false;
        fstream File(fileofvalues, ios::in | ios::out | ios::binary);
        for (i = 0; i < num; i++) {
            File.seekg(tmp[i].value_pos);
            int tmp;
            File.read(reinterpret_cast<char*>(&tmp), sizeof(int));
            if (tmp > value) {
                flag = true;
                break;
            }
        }
        int target = max(0, i - 1); // tmp[target] is exactly where we delete the value
        File.seekg(tmp[target].value_pos);
        vector<int> valueoftarget;
        for (int i = 0; i < maxN; i++) {
            int tmp;
            File.read(reinterpret_cast<char*> (&tmp), sizeof(int));
            if (tmp == -1) {
                break;
            }
            valueoftarget.push_back(tmp);
        }
        auto it = find(valueoftarget.begin(), valueoftarget.end(), value);
        if (it == valueoftarget.end()) {
            return;
        }
        valueoftarget.erase(it);
        File.seekp(tmp[target].value_pos);
        for (int i = 0; i < valueoftarget.size(); i++) {
            File.write(reinterpret_cast<char*> (&valueoftarget[i]), sizeof(int));
        }
        int c = -1;
        File.write(reinterpret_cast<char*> (&c), sizeof(int));
        return;
    }
    void Find(string index) {
        vector<Node> tmp = findnode(index);
        if (tmp.empty()) {
            cout << "null" << endl;
            return;
        } else {
            bool flag = false;
            fstream File(fileofvalues, ios::in | ios::out | ios::binary);
            for (int i = 0; i < tmp.size(); i++)
            {
                File.seekg(tmp[i].value_pos);
                int tmpp;
                File.read(reinterpret_cast<char*> (&tmpp), sizeof(int));
                if (tmpp == -1) {
                    continue;
                } else {
                    flag = true;
                    cout << tmpp << ' ';
                    for (int i = 0; i < maxN; i++) {
                        int tmppp;
                        File.read(reinterpret_cast<char*> (&tmppp), sizeof(int));
                        if (tmppp == -1) {
                            break;
                        }
                        cout << tmppp << ' ';
                    }
                }
            }
            if (!flag) {
                cout << "null" << endl;
                return;
            }
            cout << endl;
        }
    }
};

int main() {	
    // freopen("in.txt", "r", stdin);   // 读入in.dat文件作为输入 
	// freopen("out.dat", "w", stdout);  // 将输入写入out.dat文件中 
    fstream tmp;
    tmp.open("fileofindex", ios::in | ios::out | ios::binary);
    tmp.close();
    tmp.open("fileofvalues", ios::in | ios::out | ios::binary);
    tmp.close();
    List bl("fileofindex", "fileofvalues");
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
    return 0;
}