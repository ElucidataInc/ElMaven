#ifndef __NODES_H__
#define __NODES_H__

#include <vector>
#include <algorithm>
#include <utility>
#include <map>
#include <string>

namespace mgf {

namespace detail {

struct Node
{
    virtual ~Node() {}
};

struct DoublePairNode : public Node
{
    typedef std::pair<double, double> value_type;
    explicit DoublePairNode(double mz, double ab) : Node(), value_(std::make_pair(mz, ab)) {}
    explicit DoublePairNode(double mz) : Node(), value_(std::make_pair(mz, 0.0)) {}
    value_type value_;
};

struct DoublePairListNode : public Node
{
    typedef std::vector<DoublePairNode::value_type> value_type;
    explicit DoublePairListNode(DoublePairNode* dpn) : Node() {
        // create new vector of pairs
        pairlist_ = new std::vector<DoublePairNode::value_type>;
        pairlist_->push_back(dpn->value_);
        delete dpn;
    }
    explicit DoublePairListNode(DoublePairListNode* dpns, DoublePairNode* dpn)
      : Node() {
        // steal pointer
        pairlist_ = dpns->pairlist_;
        dpns->pairlist_ = 0;
        delete dpns;
        // evaluate instantly: copy rhs pointer,
        pairlist_->push_back(dpn->value_);
        delete dpn;
    }
    virtual ~DoublePairListNode() {
        if (pairlist_) delete pairlist_;
    }

    std::vector<DoublePairNode::value_type>* pairlist_;
};


struct IntegerNode : public Node
{
    explicit IntegerNode(int i) : Node(), i_(i) {}
    int i_;
};

struct IntegerListNode : public Node
{
    explicit IntegerListNode(int i) : Node() {
        integers_ = new std::vector<int>;
        integers_->push_back(i);
    }
    explicit IntegerListNode(IntegerNode* node) : Node() {
        integers_ = new std::vector<int>;
        integers_->push_back(node->i_);
        delete node;
    }
    explicit IntegerListNode(IntegerListNode* integers) : Node() {
        integers_ = integers->integers_;
        integers->integers_ = 0;
        delete integers;
    }
    explicit IntegerListNode(IntegerListNode* integers, IntegerNode* integer)
      : Node() {
        integers_ = integers->integers_;
        integers->integers_ = 0;
        delete integers;
        integers_->push_back(integer->i_);
        delete integer;
    }
    explicit IntegerListNode(IntegerListNode* integers, int integer) : Node() {
        integers_ = integers->integers_;
        integers->integers_ = 0;
        delete integers;
        integers_->push_back(integer);
    }
    ~IntegerListNode() {
        if (integers_) delete integers_;
    }
    std::vector<int>* integers_;
};

} // namespace mgf::detail

} // namespace mgf

#endif // __NODES_H__
