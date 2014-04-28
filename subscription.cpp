#include "subscription.h"
#include <boost/algorithm/string.hpp>
#include <algorithm>
Subscription::Subscription(): subIndex(0)
{
}


void Subscription::subscribe(const sig_condition_t &condition, bool final)
{
    // count condition frequencies
    for (auto condition_item: condition){
        if (this->cond_freq.count(condition_item)<1){
            this->cond_freq[condition_item]=1;
        } else {
            this->cond_freq[condition_item]++;
        }
    }

    // add new subscription
    this->subscriptions.push_back(condition);

    // clear paths (maybe unnecessary?)
    for (auto node: this->primaryNodes){
        node.second->ownPaths.clear();
        node.second->paths.clear();
    }
    for (auto node: this->secondaryNodes){
        node.second->ownPaths.clear();
        node.second->paths.clear();
    }

    if (!final) return;
    // sort conditions within subscriptions
    this->subIndex=0;
    for (auto subscription: this->subscriptions){
        std::sort(subscription.begin(), subscription.end(), [this](sig_condition_item_t i, sig_condition_item_t j)-> bool {
            return this->cond_freq[i] > this->cond_freq[j];
        });

        sig_condition_item_t& newPrimary=subscription[0];
        if (this->primaryNodes.count(newPrimary)<1) {
            if (this->secondaryNodes.count(newPrimary)<1) {
                this->primaryNodes[newPrimary]=new Node(newPrimary);
            } else {
                this->primaryNodes[newPrimary]=this->secondaryNodes[newPrimary];
            }
        }
        // recreate path
        unordered_map<sig_condition_item_t, Node*>* paths = &this->primaryNodes[newPrimary]->ownPaths;
        Node* parent=this->primaryNodes[newPrimary];
        Node* child;
        for (auto it=subscription.cbegin()+1, end=subscription.cend(); it!=end; it++){
            if (this->primaryNodes.count(*it)) {
                child=this->primaryNodes[*it];
            } else if (this->secondaryNodes.count(*it)) {
                child=this->secondaryNodes[*it];
            } else {
                child=this->secondaryNodes[*it]=new Node(*it);
            }

            (*paths)[this->subIndex]=child;
            child->parents[this->subIndex]=parent;
            parent=child;
            paths=&(child->paths);
        }
        this->subIndex++;
    }
}
