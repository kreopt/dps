#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <unordered_map>
#include <vector>
#include <string>
#include "dps.h"
using namespace dps;

struct Node{
    Node(const sig_condition_t& cond): condition(cond){}

    const sig_condition_t        condition;
    unordered_map<int, Node*>    ownPaths;
    unordered_map<int, Node*>    paths;
    unordered_map<int, Node*>    parents;
};

class Subscription
{
    int subIndex;
    unordered_map<sig_param_t, int, decltype(&sig_param_hash)>              cond_freq;
    vector<sig_condition_t>                              subscriptions;
    unordered_map<sig_param_t, Node*,decltype(&sig_param_hash)>           primaryNodes;
    unordered_map<sig_param_t, Node*,decltype(&sig_param_hash)>           secondaryNodes;

    bool sort_conditions(int, int);
public:
    Subscription();
    const unordered_map<sig_condition_item_t, Node*>&   getPrimary() const {return primaryNodes;}
    bool   inPrimary(const sig_condition_item_t& cond) const {return primaryNodes.count(cond)>0;}
    bool   inSecondary(const sig_condition_item_t& cond) const {return secondaryNodes.count(cond)>0;}

    //void subscribeSimple(std::string&)
    void subscribe(const sig_condition_t& condition, bool final=true);
    int  subCount(){return subscriptions.size();}
    //std::vector<std::vector<std::string>> getSubscriptions(){return this->subscriptions;}
};

#endif // SUBSCRIPTION_H
