#include <catch2/catch_test_macros.hpp>


// #include <deluxe_lib/sample_library.hpp>

#include "deluxe_lib/math/vectorization/DataStruct.hpp"
#include "deluxe_lib/math/vectorization/DataVector.hpp"

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