#include <catch2/catch_test_macros.hpp>

#include <fstream>
#include <istream>
#include <ostream>

// #include <deluxe_lib/sample_library.hpp>

#include "deluxe_lib/math/vectorization/DataStruct.hpp"
#include "deluxe_lib/math/vectorization/DataVector.hpp"

#include "deluxe_lib/configs/settings/Settings.hpp"

struct test : public DataStruct<test>
{
    DataVector<int, 0> x;
    DataVector<int, 1> y;
    DataVector<char, 2> c;
};


TEST_CASE("DataVector", "[vectorization]")
{
    test one = test();
    one.x = 0;
    one.y = 1;
    one.c = 'a';

    REQUIRE(one.x.getRefs().size() == 1);

    test two = test();
    two.x = 1;
    two.y = 2;
    two.c = 'b';

    REQUIRE(one.y.getRefs().size() == 2);

    test three = test();
    three.x = 3;
    three.y = 4;
    three.c = 'c';

    REQUIRE(one.c.getRefs().size() == 3);
}

#include "deluxe_lib/serialization/Serializer.hpp"

struct test2
{
    test2(int a, int b) : a(a), b(b) {}
    test2() {}
    int a = 0;
    bool b = false;
};

TEST_CASE("Codec", "[serialization]")
{
    Codec<test2> RECORD_CODEC = Serializer::createCodec<test2>(
        [=](Serializer::Instance& instance)
        {
            instance.group(
                Codec<test2>::INT().fieldOf("a").forGetter(&test2::a),
                Codec<test2>::BOOL().fieldOf("b").forGetter(&test2::b)
            ).apply(+[](int a, bool b) { return std::make_unique<test2>(a, b); });
        }
    );
    test2 obj(2, true);
    test2 obj1;

    std::ofstream file("test2.txt");
    RECORD_CODEC.serialize(obj, file);

    std::ifstream file2("test2.txt");
    RECORD_CODEC.deserialize(obj1, file2);
}