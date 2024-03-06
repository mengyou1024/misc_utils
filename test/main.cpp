#include <iostream>
#include <Yo/File>
#include <Yo/Types>
#include <memory>
#include <utility>
#include <vector>

using namespace std;
using Yo::File::MakeFunctional;
int main() {
    std::string _fileName = R"(C:\Users\Administrator\Desktop\UniOffline\components\UnionUniversalApparatusLib\test\testRead)";
    auto fileName = Yo::Types::WStringFromString(_fileName);
    std::vector<uint8_t> data;
    data.resize(10);
    Yo::File::ReadFile(fileName, data);
    uint64_t aaa = 0;
    Yo::File::ReadFile(fileName, aaa);
    uint8_t a,b,c,d,e,f,g,h,i,j,k;
    Yo::File::ReadFile(fileName, a,b,c,d,e,f,g,h,i,j,k);
    uint8_t vv;
    Yo::File::ReadFile(fileName, Yo::File::SkipSize(10), vv);
    std::vector<uint8_t> vvv;
    auto ptr = vvv.data();
    uint8_t vvv_size = 0;
    Yo::File::ReadFile(fileName, Yo::File::SkipSize(1), vvv_size, MakeFunctional<std::vector<uint8_t>&>([&vvv, &vvv_size]() -> std::vector<uint8_t>& {
        vvv.resize(vvv_size);
        return vvv;
    }));

    Yo::File::ReadFile(fileName, MakeFunctional([&vvv](size_t file_size) -> auto& {
        vvv.resize(file_size);
        return vvv;
    }));
    return 0;
}
