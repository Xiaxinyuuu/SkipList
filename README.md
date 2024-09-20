# 一、项目简介
SkipList的 C ++ 实现。
主要功能
* 支持基本的增删改查操作
* 支持数据的dump和load功能
* 支持多线程并发读取操作
* 支持服务快速重启
# 二、详细设计
## 3.1跳表类设计实现
### 如何使用基本数据结构表示跳表
![](https://rte.weiyun.baidu.com/wiki/attach/image/api/imageDownloadAddress?attachId=4f431f993f654d0184ecf92aee421ff7&docGuid=4heTieHNzcfZXx&sign=eyJhbGciOiJkaXIiLCJlbmMiOiJBMjU2R0NNIiwiYXBwSWQiOjEsInVpZCI6IkE0WnJNYWFaYWQiLCJkb2NJZCI6IjRoZVRpZUhOemNmWlh4In0..lJny6gymJm6DDDKD.xaz1DxR0aPHe-usSX5Eza8d81hoW2BNioBy8rEAWglGmASSqmHSuVvViOcZhVcNDk_TIrA_d-HKsMO99yDuNk-JK0e_B1oOlj2voUzw8oZZxrN3f8eE0S8aVVnGbqQ5BMEYUWSgDndoI-VKFTMqJ1Dcu79SLYrGcLTZYwJ2eZ9mCTIVd4W4C3yAiCRur6E6Z1c1dr-HaokaMKU22n9X9S1Hj1g.b7ywoGoUPVUF9ahAYMcDHw)
图片展示了清晰展示了跳表的结构，可以到跳表与单链表的在表示上的区别主要为以下3点
1. 跳表中存在多级索引，每一级索引都可以看作一条单链表
2. 为了加速查找性能，跳表中的每级索引对应的单链表都是有序的
3. 传统单链表的节点中只包函一个值和一个指针，而跳表中的节点，包函一个值和若干个指针，用于指向当前位置的节点对应的不同层级索引的下一个节点
   先来看一下单链表的表示方式。
```c++
struct Node {
    string key;
    string value;
    Node *next;
}
```
针对上述区别，需要对单链表的表示结构作出如下改进。
* 将Node *next; 修改为 Node[N] level; 
```c++
//跳表中的节点
struch Node {
    string key;
    string value;
    // node->level[i]代表当前节点对应的第i级索引的下一个节点
    vector<Node> level; 
}
```
为了便于理解是如何采用改进后的数据结构对跳表进行表示的，下面的代码展示了上图中的节点A是如何进行表示的。
```c++
// 省略节点A的初始化操作，直接表示跳表结构
A.level[0] = B; // 第一级索引的下一个节点为B，以此类推
A.level[1] = C;
A.level[2] = E;
A.level[3] = E;
A.level[4] = G;
```
### 查找操作
跳表这个数据结构存在的意义就是为了缩短数据查询时间，提高查询性能。  
设计思想采用空间换取时间，所以在单链表的基础上构造了多级索引。   
那么如何利用多级索引实现高效的数据查找就成了关键所在，下面先看一下单链表的查找过程。
![](https://rte.weiyun.baidu.com/wiki/attach/image/api/imageDownloadAddress?attachId=a4e0964000854a58a41c0268d8110b9c&docGuid=4heTieHNzcfZXx&sign=eyJhbGciOiJkaXIiLCJlbmMiOiJBMjU2R0NNIiwiYXBwSWQiOjEsInVpZCI6IkE0WnJNYWFaYWQiLCJkb2NJZCI6IjRoZVRpZUhOemNmWlh4In0..lJny6gymJm6DDDKD.xaz1DxR0aPHe-usSX5Eza8d81hoW2BNioBy8rEAWglGmASSqmHSuVvViOcZhVcNDk_TIrA_d-HKsMO99yDuNk-JK0e_B1oOlj2voUzw8oZZxrN3f8eE0S8aVVnGbqQ5BMEYUWSgDndoI-VKFTMqJ1Dcu79SLYrGcLTZYwJ2eZ9mCTIVd4W4C3yAiCRur6E6Z1c1dr-HaokaMKU22n9X9S1Hj1g.b7ywoGoUPVUF9ahAYMcDHw)
使用代码表示上图查找过程
```c++
int x = 5;
Node *cur = A;
while (cur != NULL && cur->val != x) {
    cur = cur->next
}
// 如果cur不等于NULL则说明找到了目标元素
```
跳表的查找过程则充分发挥了多级索引和有序链表的优势，大大提升了查询效率
![](https://rte.weiyun.baidu.com/wiki/attach/image/api/imageDownloadAddress?attachId=375efa2c6fa8444bafdeed77f95d8b39&docGuid=4heTieHNzcfZXx&sign=eyJhbGciOiJkaXIiLCJlbmMiOiJBMjU2R0NNIiwiYXBwSWQiOjEsInVpZCI6IkE0WnJNYWFaYWQiLCJkb2NJZCI6IjRoZVRpZUhOemNmWlh4In0..lJny6gymJm6DDDKD.xaz1DxR0aPHe-usSX5Eza8d81hoW2BNioBy8rEAWglGmASSqmHSuVvViOcZhVcNDk_TIrA_d-HKsMO99yDuNk-JK0e_B1oOlj2voUzw8oZZxrN3f8eE0S8aVVnGbqQ5BMEYUWSgDndoI-VKFTMqJ1Dcu79SLYrGcLTZYwJ2eZ9mCTIVd4W4C3yAiCRur6E6Z1c1dr-HaokaMKU22n9X9S1Hj1g.b7ywoGoUPVUF9ahAYMcDHw&x-bce-process=image/resize,m_lfit,w_1920/ignore-error,i_1)
使用代码表示上图查找过程
```c++
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
```
### 优化查找过程
在大部分情况下，存储键值对的时候我们都把string作为key的类型，因为存在语义信息，便于大脑记忆。
但是字符串的比较操作时间复杂度是O(n)的，大大降低了跳表的查询性能，所以需要对此进行优化。
优化思路
1. 在Node结构体中补充int类型score字段
2. 每次构造新的跳表节点时都根据key的值计算出score(字符串的字典序越大，计算出的score越大)
3. 比较跳表的节点大小时，优先比较score，如果score相等再对字符串的值进行比较
   下面是根据字符串计算score的函数
```c++
int64_t SkipList::calculate_score(const std::string &key) {
    const int64_t BASE = 256; // 基数为256，保证字符的ASCII值在范围内
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
```
### 删除操作
插入操作依赖于查找操作
删除操作实现步骤
1. 根据key值查找对应的节点，分为如下两种情况
   a. 没有查找到对应的节点，说明要删除的节点不存在，直接返回即可
   b. 查找到对应的节点，进行删除操作
2. 删除操作
   a. 删除节点的前一个节点指向删除节点的下一个节点
   跳表的删除操作和单链表的删除操作实现思路上是一致的
先来看下单链表删除操作的流程
![](https://rte.weiyun.baidu.com/wiki/attach/image/api/imageDownloadAddress?attachId=d3bd784c5a9e477aafde1fc5163c39d0&docGuid=4heTieHNzcfZXx&sign=eyJhbGciOiJkaXIiLCJlbmMiOiJBMjU2R0NNIiwiYXBwSWQiOjEsInVpZCI6IkE0WnJNYWFaYWQiLCJkb2NJZCI6IjRoZVRpZUhOemNmWlh4In0..lJny6gymJm6DDDKD.xaz1DxR0aPHe-usSX5Eza8d81hoW2BNioBy8rEAWglGmASSqmHSuVvViOcZhVcNDk_TIrA_d-HKsMO99yDuNk-JK0e_B1oOlj2voUzw8oZZxrN3f8eE0S8aVVnGbqQ5BMEYUWSgDndoI-VKFTMqJ1Dcu79SLYrGcLTZYwJ2eZ9mCTIVd4W4C3yAiCRur6E6Z1c1dr-HaokaMKU22n9X9S1Hj1g.b7ywoGoUPVUF9ahAYMcDHw)
用代码表示上图删除过程
```c++
Node *B = A->next;
A->next = B->next;
delete B;
```
跳表和单链表的主要区别在于多级索引的存在，所以每一级索引中的对应节点都需要进行删除
下面是跳表中删除节点的流程
![](https://rte.weiyun.baidu.com/wiki/attach/image/api/imageDownloadAddress?attachId=7cc02dab0ea141f4ad3be6d07dc44426&docGuid=4heTieHNzcfZXx&sign=eyJhbGciOiJkaXIiLCJlbmMiOiJBMjU2R0NNIiwiYXBwSWQiOjEsInVpZCI6IkE0WnJNYWFaYWQiLCJkb2NJZCI6IjRoZVRpZUhOemNmWlh4In0..lJny6gymJm6DDDKD.xaz1DxR0aPHe-usSX5Eza8d81hoW2BNioBy8rEAWglGmASSqmHSuVvViOcZhVcNDk_TIrA_d-HKsMO99yDuNk-JK0e_B1oOlj2voUzw8oZZxrN3f8eE0S8aVVnGbqQ5BMEYUWSgDndoI-VKFTMqJ1Dcu79SLYrGcLTZYwJ2eZ9mCTIVd4W4C3yAiCRur6E6Z1c1dr-HaokaMKU22n9X9S1Hj1g.b7ywoGoUPVUF9ahAYMcDHw)
用代码表示上图删除过程
```c++
// levels_prev[i]表示插入节点的第i层对应的前一个节点
Node *del = levels_prev[0]->level[0]; // del代表要删除的节点
for (int i = 0; i < del->level.size(); i ++) {
    levels_prev[i]->level[i] = del->level[i];
}
delete del;
```
### 插入操作
插入操作依赖于查找操作，需要先查找到要插入位置的前一个节点
插入操作步骤
1. 查找到要插入位置的前一个节点
2. 执行具体的插入操作

先来看下单链表的插入流程
![](https://rte.weiyun.baidu.com/wiki/attach/image/api/imageDownloadAddress?attachId=7a4c15b8d45f42fe85a0999c96d22c39&docGuid=4heTieHNzcfZXx&sign=eyJhbGciOiJkaXIiLCJlbmMiOiJBMjU2R0NNIiwiYXBwSWQiOjEsInVpZCI6IkE0WnJNYWFaYWQiLCJkb2NJZCI6IjRoZVRpZUhOemNmWlh4In0..lJny6gymJm6DDDKD.xaz1DxR0aPHe-usSX5Eza8d81hoW2BNioBy8rEAWglGmASSqmHSuVvViOcZhVcNDk_TIrA_d-HKsMO99yDuNk-JK0e_B1oOlj2voUzw8oZZxrN3f8eE0S8aVVnGbqQ5BMEYUWSgDndoI-VKFTMqJ1Dcu79SLYrGcLTZYwJ2eZ9mCTIVd4W4C3yAiCRur6E6Z1c1dr-HaokaMKU22n9X9S1Hj1g.b7ywoGoUPVUF9ahAYMcDHw)
用代码表示上述插入过程
```c++
N->next = A->next; // dum->next即原来的真正头节点
A->next = N->next;
```
跳表的插入操作和单链表的插入操作实现思路上是一致的  
但是由于跳表是存在多级索引的，在插入节点的时候就要考虑建立索引的问题  
应该给新的节点建立几级索引呢？  
这里采用的建立索引策略是每个节点有50%的概率向上建立一级索引   
比如A节点建立第一级索引的过程是50%，则建立第二级索引的概率就是50% * 50% = 25%，以此类推  
下面是获取新插入节点索引层级的函数
```c++
// 一个随机函数，生成0或1的概率均为50%
int rand2() {
    static std::mt19937 gen(static_cast<unsigned int>(std::time(0))); // 使用 Mersenne Twister 作为随机数生成器
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(gen);
}

// 计算索引层级，层级为1的概率是50%，层级为2的概是25%，以此类推...
unsigned int calculate_level_num() {
    for (int level = 1; level <= MAX_LEVEL_NUM ; level ++) {
        if (condition) {
            return level;
        }
    }
}
```
确认好新插入节点的索引层级后，就可以进行插入操作了，下面是跳表中插入节点的流程
![](https://rte.weiyun.baidu.com/wiki/attach/image/api/imageDownloadAddress?attachId=a1bc0cdd2b7d43ef9df4cfe850953060&docGuid=4heTieHNzcfZXx&sign=eyJhbGciOiJkaXIiLCJlbmMiOiJBMjU2R0NNIiwiYXBwSWQiOjEsInVpZCI6IkE0WnJNYWFaYWQiLCJkb2NJZCI6IjRoZVRpZUhOemNmWlh4In0..-HBF0yET3KiKnhvg.pHzAUbBrIsQxPQy1jb61PMbyrlN61IUkQQmsgst2bEzm4Wl6uFWsp7409UzbQXmXeXpAkiZ-LgCkx87ugeX0yeJg9OknaEBUCqKuM3ad8p11CB8bP43zWdxB0tcj3LKIsk2AlLpYDk_j2v5Nm9xc3J-Lk-I23yPhgYYBAWqbAwkZlvMooIUeZL2TvfNRPTfKpDDBOGgELLXTZJdVus3Gb9v8eg.mbgNfbFj1w4GwT2V745KIQ)
用代码表示上述插入过程
```c++
int level_num = calculate_index_level_num() + 1; //level_num代表要插入节点的层级 
Node *node = new Node(key, val, calculate_score(key), level_num);
for (int i = 0; i < level_num; i ++) {
    node->level[i] = levels_prev[i]->level[i];
    levels_prev[i]->level[i] = node;
}
```
### 更新操作
更新操作依赖于查找操
更新操作实现步骤
* 根据key值查找对应的节点，分为如下两种情况 
  * a. 没有查找到对应的节点，说明要更新的节点不存在，直接返回即可 
  * b. 查找到对应的节点，进行更新操作 
* 更新操作 
  * a. 将查找到的节点的值赋值为新值即可
     由于更新操作实现非常简单，相比于查找操作只需要多出一步赋值的步骤，所以此处不做过多介绍。
### 时间复杂度分析(理想情况下)
计算跳表查找节点的时间复杂度需要确定下面两个关系
1. 需要遍历的总层级H(包括索引层级 + 原始数据)和跳表中的原始数据(也就是还没有建立索引时，初始的一条有序链表)中节点数量n的关系
2. 每个层级需要的比较次数N和跳表中的原始数据中节点数量n的关系
### 总层级H
由于跳表实现过程中采用建立索引的策略是每个节点有50%的概率向上建立一级索引  
假设跳表中的原始数据包函n个节点。  
那么理论上：一级索引有n / 2个节点，二级索引有n / 4个节点，k级索引有n / 2k个节点。  
一般情况下最高级索引只有两个节点，假设最高级索引的高度为h，那么一定满足2 = n * (1 / 2)^h，两边同时取对数，则可以推导出：h = (log₂n) - 1  
由于h只是索引的高度，并不包括跳表的原始数据，所以需要加上最底层的原始数据才是跳表的总高度H  
即：H = h + 1 = (log₂n) - 1 + 1 = log₂n
### 比较次数N
每个层级最多比较2个节点
![](https://rte.weiyun.baidu.com/wiki/attach/image/api/imageDownloadAddress?attachId=ff273438e8d14a95889e5bf7aba44a71&docGuid=4heTieHNzcfZXx&sign=eyJhbGciOiJkaXIiLCJlbmMiOiJBMjU2R0NNIiwiYXBwSWQiOjEsInVpZCI6IkE0WnJNYWFaYWQiLCJkb2NJZCI6IjRoZVRpZUhOemNmWlh4In0..-HBF0yET3KiKnhvg.pHzAUbBrIsQxPQy1jb61PMbyrlN61IUkQQmsgst2bEzm4Wl6uFWsp7409UzbQXmXeXpAkiZ-LgCkx87ugeX0yeJg9OknaEBUCqKuM3ad8p11CB8bP43zWdxB0tcj3LKIsk2AlLpYDk_j2v5Nm9xc3J-Lk-I23yPhgYYBAWqbAwkZlvMooIUeZL2TvfNRPTfKpDDBOGgELLXTZJdVus3Gb9v8eg.mbgNfbFj1w4GwT2V745KIQ&x-bce-process=image/resize,m_lfit,w_1920/ignore-error,i_1)
还是用上面的查找流程图为例，采用反证法来说明为什么每个层级最多比较3个节点。  
证明步骤：
1. 假设在第四层级比较了4个节点，分别是C，D，E，F。 
2. 如果与F节点进行了比较，则说明X.key > E.key
3. 但是如果X.key > E.key，在之前层级索引比较过程中(例如第一级索引)就会与E的下一个节点进行比较，不会轮到下面的层级
4. 所以假设不成立，即每个层级最多与3个节点进行比较
即N = 3
##### 时间复杂度：O(N * H) = O(3 * log₂n) = O(log₂n)
### 空间复杂度分析(理想情况下)
计算跳表的空间复杂度只需要确定下面一个关系即可
1. 需要存储的节点总数和跳表中的原始数据(也就是还没有建立索引时，初始的一条有序链表)中节点数量n的关系  

假设有跳表的原始数据有n个元素，  
那么需要建立的索引节点个数为：n / 2 + n / 4 + n / 8...   
该序列无限趋近于n  
所以跳表中总数据个数：N = n + n = 2n
##### 空间复杂度：O(N) = O(2n) = O(n)
## 3.2KV服务设计实现
![](https://rte.weiyun.baidu.com/wiki/attach/image/api/imageDownloadAddress?attachId=7cd423e525b44cf1857b5a55db693758&docGuid=4heTieHNzcfZXx&sign=eyJhbGciOiJkaXIiLCJlbmMiOiJBMjU2R0NNIiwiYXBwSWQiOjEsInVpZCI6IkE0WnJNYWFaYWQiLCJkb2NJZCI6IjRoZVRpZUhOemNmWlh4In0..-HBF0yET3KiKnhvg.pHzAUbBrIsQxPQy1jb61PMbyrlN61IUkQQmsgst2bEzm4Wl6uFWsp7409UzbQXmXeXpAkiZ-LgCkx87ugeX0yeJg9OknaEBUCqKuM3ad8p11CB8bP43zWdxB0tcj3LKIsk2AlLpYDk_j2v5Nm9xc3J-Lk-I23yPhgYYBAWqbAwkZlvMooIUeZL2TvfNRPTfKpDDBOGgELLXTZJdVus3Gb9v8eg.mbgNfbFj1w4GwT2V745KIQ&x-bce-process=image/resize,m_lfit,w_1920/ignore-error,i_1)
基于thrift框架搭建CS架构的KV服务，系统架构图如上所述
## 3.3多线程并发读取实现
搭配std::shared_mutex、std::shared_lock、std::unique_lock实现读写锁来支持多线程的并发读取操作  
所有读操作，std::shared_lock来获取锁，允许多个读操作并发执行。  
所有写操作，使用std::unique_lock来获取锁，确保在写入时没有其他读操作或写操作在执行。
## 三、单测覆盖率统计
![](https://rte.weiyun.baidu.com/wiki/attach/image/api/imageDownloadAddress?attachId=a9efd86ae23146158cf91cd04c906633&docGuid=4heTieHNzcfZXx&sign=eyJhbGciOiJkaXIiLCJlbmMiOiJBMjU2R0NNIiwiYXBwSWQiOjEsInVpZCI6IkE0WnJNYWFaYWQiLCJkb2NJZCI6IjRoZVRpZUhOemNmWlh4In0..-HBF0yET3KiKnhvg.pHzAUbBrIsQxPQy1jb61PMbyrlN61IUkQQmsgst2bEzm4Wl6uFWsp7409UzbQXmXeXpAkiZ-LgCkx87ugeX0yeJg9OknaEBUCqKuM3ad8p11CB8bP43zWdxB0tcj3LKIsk2AlLpYDk_j2v5Nm9xc3J-Lk-I23yPhgYYBAWqbAwkZlvMooIUeZL2TvfNRPTfKpDDBOGgELLXTZJdVus3Gb9v8eg.mbgNfbFj1w4GwT2V745KIQ)
主要代码文件skip_list.cpp行覆盖率93%，分支覆盖率81%
## 四、性能测试
### 测试环境
**操作系统**：MacOS
**CPU**: m1
**MEM**: 16G
**key/value size**: 4 / 9 byte
**skiplist max_height**: 32
### 测试结果
* 不同跳表数据总量下的数据操作时延

| 节点个数    | 插入(us) | 搜索(us) | 更新(us) | 删除(us) |
|---------|-----|--------|--------|--------|
| 3000    | 3   | 5      | 5      | 3      |
| 6000    | 4   | 8      | 8      | 4      |
| 10000   | 26  | 9      | 9      | 4      |
| 50000   | 22   | 29     | 25     | 12     |
| 100000  |   47  | 42     | 43     | 21     |
| 500000  | 86 | 203    | 183    | 87     |
| 1000000 | 158 | 470    | 469    | 238    |
从通过测试结果可以看出，跳表在大量节点情况下仍然能保持较为稳定的性能，符合其对数时间复杂度的理论特性，并且所有操作的时间都随着节点数量的增加而平滑增长，没有出现明显的非线性增长。这表明跳表在处理大数据量时具有良好的性能。