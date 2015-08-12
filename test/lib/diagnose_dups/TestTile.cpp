#include "diagnose_dups/Tile.hpp"

#include <gtest/gtest.h>

class TestTile : public ::testing::Test {

};

TEST_F(TestTile, equals) {
    Tile test_tile;
    test_tile.flowcell = "a";
    test_tile.lane = 1;
    test_tile.id = 1101;
    test_tile.subtile_x = 1;
    test_tile.subtile_y = 5;

    Tile test_tile2 = test_tile;

    ASSERT_TRUE(test_tile == test_tile2);

    test_tile2.subtile_y = 6;
    ASSERT_FALSE(test_tile == test_tile2);
}

TEST_F(TestTile, same_tile) {
    Tile test_tile;
    test_tile.flowcell = "a";
    test_tile.lane = 1;
    test_tile.id = 1101;

    Tile test_tile2 = test_tile;
    ASSERT_TRUE(same_tile(test_tile, test_tile2));

    test_tile2.id = 1102;
    ASSERT_FALSE(same_tile(test_tile, test_tile2));
}

TEST_F(TestTile, adjacent_tile) {
    Tile test_tile;
    test_tile.flowcell = "a";
    test_tile.lane = 1;
    test_tile.id = 1202;

    Tile test_tile2 = test_tile;
    ASSERT_TRUE(adjacent_tile(test_tile, test_tile2));

    test_tile2.id = 1303;
    ASSERT_TRUE(adjacent_tile(test_tile, test_tile2));

    test_tile2.id = 1101;
    ASSERT_TRUE(adjacent_tile(test_tile, test_tile2));

    test_tile2.id = 2202;
    ASSERT_FALSE(adjacent_tile(test_tile, test_tile2));

    test_tile2.id = 1204;
    ASSERT_FALSE(adjacent_tile(test_tile, test_tile2));
}
    
TEST_F(TestTile, hash_value) {
    Tile test_tile;
    test_tile.flowcell = "a";
    test_tile.lane = 1;
    test_tile.id = 1101;
    test_tile.subtile_x = 1;
    test_tile.subtile_y = 5;

    Tile test_tile2 = test_tile;

    ASSERT_TRUE(hash_value(test_tile) == hash_value(test_tile2));

    test_tile2.subtile_y = 6;
    ASSERT_FALSE(hash_value(test_tile) == hash_value(test_tile2));
}

TEST_F(TestTile, less) {
    Tile test_tile;
    test_tile.flowcell = "a";
    test_tile.lane = 1;
    test_tile.id = 1101;
    test_tile.subtile_x = 1;
    test_tile.subtile_y = 5;

    Tile test_tile2 = test_tile;

    ASSERT_FALSE(test_tile < test_tile2);

    test_tile2.subtile_y = 6;
    ASSERT_TRUE(test_tile < test_tile2);

}
