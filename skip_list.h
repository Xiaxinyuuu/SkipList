//
// Created by xiaxinyu03 on 2024/7/28.
//

#ifndef CPP_GOOD_CODER_SKIP_LIST_H
#define CPP_GOOD_CODER_SKIP_LIST_H

#include <string>
#include <utility>
#include <vector>
#include <random>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <shared_mutex>
#include "math.h"

// 跳表中的索引层级最大为多少
const static int MAX_LEVEL_NUM = 10;

// 跳表中节点结构体定义
struct Node {
    std::string key; 
    std::string val;
    int score; // 根据key值计算出的得分，用于加速查找性能
    // level[i]表示当前节点在第i层索引的下一个节点
    std::vector<Node *> level;
    Node(std::string _key, const std::string &_val, unsigned int score, unsigned int level_num) : 
    key(std::move(_key)), val(_val), score(score), level(level_num, nullptr) {}
};

class SkipList {
public:
    SkipList();
    ~SkipList();
    // 通过键值查询节点
    Node* find_by_key(const std::string &key);
    // 通过键值找到要插入节点的每一层对应的前一个节点
    std::vector<Node*> find_levels_prev_by_key(const std::string &key);
    // 通过键值插入节点
    void insert(const std::string &key, const std::string &val);
    // 通过键值更新节点
    void update_by_key(const std::string &key, const std::string &val);
    // 通过键值删除节点
    void delete_by_key(const std::string &key);
    // 一个随机函数，生成0或1的概率均为50%
    int rand2();
    // 计算新节点索引层级，层级为1的概率是50%，层级为2的概是25%，以此类推...
    int calculate_index_level_num();
    // 根据键值计算得分
    int64_t calculate_score(const std::string &key);
    // 序列化SkipList到指定文件中
    bool dump(const std::string &filename);
    // 从指定文件中加载SkipList到内存
    bool load(const std::string &filename);
    // 比较两个节点大小
    int compare_node(const std::string &key, Node *node2);
private:
    // 虚拟头节点
    Node* dum_head_node;
    // 读写锁，支持多个线程并发读取数据，写操作会阻塞读操作
    std::shared_mutex mutex;
};
#endif //CPP_GOOD_CODER_SKIP_LIST_H
