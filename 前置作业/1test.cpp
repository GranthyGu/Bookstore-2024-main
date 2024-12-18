#include "1.hpp"
#include <iostream>
#include <cstring>

// 定义自定义数据类型
struct Data {
    int id;
    char name[20];

    Data() : id(0) { 
        std::memset(name, 0, sizeof(name)); 
    }

    Data(int id, const char* name) : id(id) {
        std::strncpy(this->name, name, sizeof(this->name) - 1);
        this->name[sizeof(this->name) - 1] = '\0';
    }
};

int main() {
    try {
        // 初始化 MemoryRiver
        MemoryRiver<Data> river("data.bin");
        river.initialise();

        // 写入数据
        Data d1(1, "Alice");
        Data d2(2, "Bob");
        Data d3(3, "Charlie");

        int index1 = river.write(d1);
        int index2 = river.write(d2);
        int index3 = river.write(d3);

        std::cout << "Data written to file:\n";
        std::cout << "Index1: " << index1 << ", Index2: " << index2 << ", Index3: " << index3 << "\n\n";

        // 读取数据
        Data temp;
        river.read(temp, index1);
        std::cout << "Read Data from Index1: ID = " << temp.id << ", Name = " << temp.name << "\n";

        river.read(temp, index2);
        std::cout << "Read Data from Index2: ID = " << temp.id << ", Name = " << temp.name << "\n";

        // 更新数据
        Data updatedData(2, "Bob Updated");
        river.update(updatedData, index2);
        river.read(temp, index2);
        std::cout << "\nUpdated Data at Index2: ID = " << temp.id << ", Name = " << temp.name << "\n";

        // 删除数据
        river.Delete(index3);
        river.read(temp, index3);  // 注意：删除后的数据将为空对象
        std::cout << "\nData at Index3 after Delete: ID = " << temp.id << ", Name = " << temp.name << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}