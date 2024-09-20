//
// Created by xiaxinyu03 on 2024/7/28.
//

#include "gtest/gtest.h"
#include "skip_list.h"
#include <iostream>

class SkipListTest : public ::testing::Test {
protected:
    void SetUp() override {
        skipList = new SkipList();
    }

    void TearDown() override {
        delete skipList;
    }

    SkipList* skipList;
};

TEST_F(SkipListTest, InsertAndFindByKey) {
    skipList->insert("key1", "value1");
    Node* node = skipList->find_by_key("key1");
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->key, "key1");
    EXPECT_EQ(node->val, "value1");

    Node* node2 = skipList->find_by_key("key2");
    EXPECT_EQ(node2, nullptr);
}

TEST_F(SkipListTest, UpdateByKey) {
    skipList->insert("key1", "value1");
    skipList->update_by_key("key1", "value2");
    Node* node = skipList->find_by_key("key1");
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->val, "value2");

}

TEST_F(SkipListTest, DeleteByKey) {
    skipList->insert("key3", "value3");
    skipList->delete_by_key("key3");
    Node* node = skipList->find_by_key("key3");
    EXPECT_EQ(node, nullptr);
}

TEST_F(SkipListTest, DumpAndLoad) {
    skipList->insert("key1", "value1");
    skipList->insert("key2", "value2");
    ASSERT_TRUE(skipList->dump("test_dump.txt"));
    SkipList* newSkipList = new SkipList();
    ASSERT_TRUE(newSkipList->load("test_dump.txt"));
    Node* node1 = newSkipList->find_by_key("key1");
    ASSERT_NE(node1, nullptr);
    EXPECT_EQ(node1->val, "value1");
    Node* node2 = newSkipList->find_by_key("key2");
    ASSERT_NE(node2, nullptr);
    EXPECT_EQ(node2->val, "value2");

    delete newSkipList;
    std::remove("test_dump.txt");
}

TEST_F(SkipListTest, CalculateScore) {
    int64_t score1 = skipList->calculate_score("key1");
    int64_t score2 = skipList->calculate_score("key1");
    int64_t score3 = skipList->calculate_score("key2");
    EXPECT_EQ(score1, score2);
    EXPECT_GT(score3, score1);
}

TEST_F(SkipListTest, Rand2) {
    int result = skipList->rand2();
    EXPECT_TRUE(result == 0 || result == 1);
}

TEST_F(SkipListTest, CalculateIndexLevelNum) {
    int level = skipList->calculate_index_level_num();
    EXPECT_GE(level, 1);
}

TEST_F(SkipListTest, FindLevelsPrevByKey) {
    skipList->insert("key1", "value1");
    std::vector<Node*> prev_nodes = skipList->find_levels_prev_by_key("key1");
    EXPECT_EQ(prev_nodes[0]->key, "dumHeadNode");

    std::vector<Node*> non_existent_prev_nodes = skipList->find_levels_prev_by_key("key2");
    EXPECT_EQ(non_existent_prev_nodes[0]->key, "key1");
}

TEST_F(SkipListTest, CompareNode) {
    skipList->insert("key1", "value1");
    Node* node = skipList->find_by_key("key1");

    int result = skipList->compare_node("key1", node);
    EXPECT_EQ(result, 0);

    int result_greater = skipList->compare_node("key2", node);
    EXPECT_EQ(result_greater, 1);

    int result_less = skipList->compare_node("key0", node);
    EXPECT_EQ(result_less, -1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}