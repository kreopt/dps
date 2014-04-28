#include "matcher.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

bool Matcher::checkCond(const dps::Signal * event, std::string cond) const
{
    std::vector<std::string> arg;
    boost::split(arg, cond, boost::is_any_of(" "));
    /*for (int i=0; i<10000; i++){
        arg[1].compare("=");
    }*/
    int val=event->value(arg.at(0));
    if (arg[1].compare("=")==0){
        if (val != std::stoi(arg[2])){
            return false;
        }
    } else if (arg[1].compare("!=")==0) {
        if (val == std::stoi(arg[2])){
            return false;
        }
    } else if (arg[1].compare(">")==0) {
        if (val < std::stoi(arg[2])){
            return false;
        }
    } else if (arg[1].compare("<")==0) {
        if (val > std::stoi(arg[2])){
            return false;
        }
    }
    return true;
}

void Matcher::handleSubscriber(int mark)
{
    /*auto subs = this->sub->getSubscriptions()[mark];
    for (auto sub: subs){
        std::cout << sub << " ";
    }
    std::cout<<std::endl;*/
}

void Matcher::newRoots(const Node *node, int mark)
{
    this->skip[node->condition]=true;
}

void Matcher::moveParents(const Node *node, int mark)
{
    //this->pathMap[node->condition+std::string(":")+std::to_string(mark)]=node->paths[mark];

}

void Matcher::dropParents(const Node *node, int mark)
{
    this->skip[node->condition]=false;
}

void Matcher::disablePrimary(const Node *node)
{
    this->skip[node->condition]=false;
}

Matcher::Matcher()
{
    this->sub=new Subscription();
    // add subscriptions
}

std::vector<int> Matcher::matchSimple(const dps::Signal * event)
{
    std::vector<int> subscribers;
    bool match=false;
    int mark=0;
    for (auto subscription: this->sub->getSubscriptions()){
        match=true;
        for (auto cond: subscription){
            if (!this->checkCond(event, cond)){
                match=false;
                break;
            }
        }
        if (match){
            subscribers.push_back(mark);
            handleSubscriber(mark);
        }
        mark++;
    }
    return subscribers;
}

std::vector<int> Matcher::matchBetter(const dps::Signal * event)
{
    Node *next;
    int mark;
    std::vector<int> subscribers;
    this->skip.clear();
    for (auto cond: this->sub->getPrimary()){
        try {
            if (this->skip.at(cond.first) == false) {
                continue;
            }
        } catch (const std::out_of_range& oor){
            if (!checkCond(event, cond.first)){
                continue;
            }//TODO: add to cache
        }
        for (auto path: cond.second->ownPaths){
            next = path.second;
            mark = path.first;
            while (next!=nullptr) {
                try {
                    if (this->skip.at(next->condition) == false) {
                        next=nullptr;
                        continue;
                    } else {
                        if (next->paths.count(mark)>0) {
                            next=next->paths[mark];
                        } else {
                            next=nullptr;
                            subscribers.push_back(mark);
                            handleSubscriber(mark);
                        }
                    }
                } catch (const std::out_of_range& oor){
                    if (checkCond(event, next->condition)){
                        if (this->sub->inPrimary(next->condition)) {
                            this->newRoots(next, mark);
                            this->moveParents(next, mark);
                        } else if (this->sub->inSecondary(next->condition)) {
                            this->moveParents(next, mark);
                        }
                        if (next->paths.count(mark)>0) {
                            next = next->paths[mark];
                            /*try{
                                Node* newNode = this->pathMap.at(next->condition+std::string(":")+std::to_string(mark)) ;
                                next = newNode;
                            } catch (const std::out_of_range& oor){
                                // leave next at the same state
                            }*/
                        } else {
                            next = nullptr;
                            subscribers.push_back(mark);
                            handleSubscriber(mark);
                        }
                    } else {
                        if (this->sub->inPrimary(next->condition)) {
                            this->disablePrimary(next);
                        } else if (this->sub->inSecondary(next->condition)) {
                            this->dropParents(next, mark);
                        }
                        next = nullptr;
                        continue;
                    }
                }
            }
        }
    }
    return subscribers;
}

void Matcher::addSubscription(std::string sub, bool final)
{
    this->sub->subscribe(sub, final);
}
