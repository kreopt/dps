#ifndef MATCHER_H
#define MATCHER_H
#include <vector>
#include "signal.h"
#include "subscription.h"
class Matcher
{
    Subscription*   sub;
    dps::Signal*         evt;
    std::map<std::string, Node*>        pathMap;
    std::map<std::string, bool>        skip;

    bool    checkCond(const dps::Signal * event, std::string cond) const;
    void    handleSubscriber(int mark);

    void    newRoots(const Node* node, int mark);
    void    moveParents(const Node* node, int mark);
    void    dropParents(const Node* node, int mark);
    void    disablePrimary(const Node* node);
public:
    Matcher();
    std::vector<int> matchSimple(const dps::Signal*);
    std::vector<int> matchBetter(const dps::Signal*);

    void addSubscription(std::string sub, bool final=false);
    int  subCount(){return this->sub->subCount();}
};

#endif // MATCHER_H
