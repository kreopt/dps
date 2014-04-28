#include <iostream>
#include "isubscriptionmanager.h"
#include "tablesubscriptionmanager.h"
using namespace std;
using namespace dps;
int main()
{
    try{
        ISubscriptionManager *sub_man = new TableSubscriptionManager();
        delete sub_man;
    } catch (const string& e) {
        cout << e;
        exit(-1);
    }

    return 0;
}

