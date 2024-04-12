#include <Yo/File>
#include <Yo/Types>
#include <Yo/Y_Algorithm>
#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <vector>

TEST(File, type_traits) {
    using namespace Yo::File;
    struct A {
        void *data() {
            return nullptr;
        };
        int size() {
            return 0;
        };
    };
    EXPECT_TRUE(is_readable<std::vector<uint8_t>>);
    EXPECT_TRUE(is_readable<std::vector<uint32_t>>);
    EXPECT_TRUE(is_readable<A>);
    EXPECT_FALSE(is_readable<int>);
    std::vector<uint8_t> data;
    EXPECT_TRUE(is_read_func<decltype(MakeFunctional([&]() -> std::vector<uint8_t> & { return data; }))>);
    EXPECT_FALSE(is_read_func<int>);
    EXPECT_EQ(is_readable<std::vector<uint8_t>>, !not_readable<std::vector<uint8_t>>);
    EXPECT_TRUE(is_com_type<int>);
    EXPECT_FALSE(is_com_type<std::vector<uint8_t>>);
}

TEST(File, read_file) {
    using namespace Yo::File;
    auto                 fileName = Yo::Types::WStringFromString("template/testRead");
    std::vector<uint8_t> raw      = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<uint8_t> data;
    data.resize(10);
    Yo::File::ReadFile(fileName, data);
    EXPECT_EQ(data, raw);
    uint8_t aaa = 0;
    Yo::File::ReadFile(fileName, SkipSize(1), aaa);
    EXPECT_EQ(aaa, 1);

    std::vector<uint32_t> data2;
    data2.resize(2);
    std::vector<uint32_t> raw2 = {0x03020100, 0x07060504};
    Yo::File::ReadFile(fileName, data2);
    EXPECT_EQ(data2, raw2);

    std::vector<uint8_t> data3;
    std::vector<uint8_t> raw3 = {4, 5, 6};
    uint8_t              temp;
    Yo::File::ReadFile(fileName, SkipSize(3), temp,
                       MakeFunctional([&]() -> std::vector<uint8_t> & {
                           data3.resize(temp);
                           return data3;
                       }));
    EXPECT_EQ(data3, raw3);
    struct A {
        uint32_t a;
        uint32_t b;
        uint16_t c;
    };
    A stru;
    Yo::File::ReadFile(fileName, MakeStructSub(&(stru.a), &(stru.c), sizeof(uint16_t)));
    EXPECT_EQ(stru.a, 0x03020100);
    EXPECT_EQ(stru.b, 0x07060504);
    EXPECT_EQ(stru.c, 0x0908);
}

TEST(algorithm, lzw) {
    using namespace Yo::File;
    using namespace Yo::algorithm::lzw;
    std::vector<uint8_t> raw;
    raw.resize(10);
    EXPECT_TRUE(ReadFile(L"template/testRead", raw));
    auto compressed = lzw_compress(raw.data(), raw.size());
    EXPECT_TRUE(compressed.has_value());
    auto decompressed = lzw_decompress(compressed->data(), compressed->size());
    EXPECT_TRUE(decompressed.has_value());
    EXPECT_TRUE(std::equal(raw.begin(), raw.end(), decompressed->begin(), decompressed->end()));
}
