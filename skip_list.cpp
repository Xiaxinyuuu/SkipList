//
// Created by xiaxinyu03 on 2024/7/28.
//

#include "skip_list.h"

SkipList::SkipList() {
    // 初始化虚拟头节点和虚拟尾节点
    dum_head_node = new Node("dumHeadNode", "-1", INT_MIN, 1);
}

SkipList::~SkipList() {
    std::vector<Node*> nodes;
    while (dum_head_node != nullptr) {
        nodes.push_back(dum_head_node);
        dum_head_node = dum_head_node->level[0];
    }
    for (Node *node : nodes) {
        delete node;
    }
}

Node *SkipList::find_by_key(const std::string &key) {
    Node* cur = dum_head_node;
    std::shared_lock<std::shared_mutex> lock(mutex);  // 使用shared_lock来允许并发读
    int level_num = dum_head_node->level.size() - 1;
    while (level_num >= 0) {
        if (cur->level[level_num] != nullptr && compare_node(key, cur->level[level_num]) == 0) { 
            // 找到节点直接返回
            return cur->level[level_num];
        } else if (cur->level[level_num] == nullptr || compare_node(key, cur->level[level_num]) == -1) {
            // 向下走
            level_num --;
        } else {
            // 向右走
            cur = cur->level[level_num];
        }
    }
    return nullptr;
}

std::vector<Node*> SkipList::find_levels_prev_by_key(const std::string &key) {
    Node* cur = dum_head_node;
    int level_num = dum_head_node->level.size() - 1;
    std::vector<Node*> levels_prev(level_num + 1, dum_head_node);
    while (level_num >= 0) {
        if (cur->level[level_num] == nullptr || compare_node(key, cur->level[level_num]) <= 0) {
            // 记录当前层前一个节点
            levels_prev[level_num] = cur;
            // 向下走
            level_num --;
        } else {
            // 向右走
            cur = cur->level[level_num];
        }
    }
    return levels_prev;
}


void SkipList::insert(const std::string &key, const std::string &val) {
    std::unique_lock<std::shared_mutex> lock(mutex);  // 使用unique_lock来独占访问
    std::vector<Node *> levels_prev = find_levels_prev_by_key(key);
    int level_num = calculate_index_level_num() + 1;
    if (level_num > MAX_LEVEL_NUM) {
        level_num = MAX_LEVEL_NUM;
    }
    if (level_num > dum_head_node->level.size()) {
        dum_head_node->level.resize(level_num, nullptr);
        levels_prev.resize(level_num, dum_head_node);
    }
    
    Node *node = new Node(key, val, calculate_score(key), level_num);
    for (int i = 0; i < level_num; i ++) {
        node->level[i] = levels_prev[i]->level[i];
        levels_prev[i]->level[i] = node;
    }
}



void SkipList::update_by_key(const std::string &key, const std::string &val) {
    std::shared_lock<std::shared_mutex> lock_read(mutex);  // 保留读锁以查找节点
    Node *node = find_by_key(key);
    if (node != nullptr) {
        lock_read.unlock();  // 解锁读锁
        std::unique_lock<std::shared_mutex> lock_write(mutex);  // 获取写锁以更新节点
        node->val = val;
        lock_write.unlock();  // 释放写锁
    }
}

void SkipList::delete_by_key(const std::string &key) {
    std::unique_lock<std::shared_mutex> lock(mutex);  // 使用unique_lock来独占访问
    std::vector<Node *> levels_prev = find_levels_prev_by_key(key);
    if (levels_prev[0] == nullptr || compare_node(key, levels_prev[0]->level[0]) != 0) {
        return;
    } else {
        Node *del = levels_prev[0]->level[0];
        for (int i = 0; i < del->level.size(); i ++) {
            levels_prev[i]->level[i] = del->level[i];
        }
        delete del;
    }
    
    // 降低层数，节省内存开销
    int i = dum_head_node->level.size() - 1;
    while (i >= 0 && dum_head_node->level[i] == nullptr) {
        i --;
    }
    dum_head_node->level.resize(i + 1);
}


int SkipList::rand2() {
    static std::mt19937 gen(static_cast<unsigned int>(std::time(0))); // 使用 Mersenne Twister 作为随机数生成器
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(gen);
}

int SkipList::calculate_index_level_num() {
    int level = 1;
    for ( ; level <= MAX_LEVEL_NUM; level ++) {
        if (rand2()) {
            break;
        }
    }
    return level;    
}

int64_t SkipList::calculate_score(const std::string &key) {
    const int64_t BASE = 256;
    int64_t score = 0;

    for (char c : key) {
        int64_t temp_score = score * BASE + static_cast<unsigned char>(c);
        if (temp_score < score) {
            return LLONG_MAX;
        }
        score = temp_score;
    }
    return score;
}


bool SkipList::dump(const std::string &filename) {

    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    // 遍历 SkipList，写入每个节点的数据
    Node* cur = dum_head_node->level[0];
    if (cur == nullptr) {
        return false;
    }
    for ( ; cur != nullptr; cur = cur->level[0]) {
        ofs << cur->key << "\t" << cur->val << "\n";
    }
    ofs.close();
    return true;
}

bool SkipList::load(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        return false;
    }
    // 读取每个节点的数据并插入到SkipList中
    std::string key, value;
    while (ifs >> key >> value) {
        insert(key, value);
    }
    ifs.close();
    return true;
}

int SkipList::compare_node(const std::string &key, Node *node) {
    int score = calculate_score(key);
    if (score > node->score) {
        return 1;
    } else if (score < node->score) {
        return -1;
    } else {
        if (key > node->key) {
            return 1;
        } else if (key < node->key) {
            return -1;
        } else {
            return 0;
        }
    }
}
