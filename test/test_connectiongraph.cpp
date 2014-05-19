#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../connectiongraph.h"

CPPUNIT_TEST_SUITE_REGISTRATION( ConnectionGraphTest );
class ConnectionGraphTest: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ConnectionGraphTest);
    CPPUNIT_TEST(test_add_param_condition);
    CPPUNIT_TEST(test_remove_param_condition);
    CPPUNIT_TEST(test_add_connection);
    CPPUNIT_TEST(test_remove_connection);
    CPPUNIT_TEST(test_eval_node);
    CPPUNIT_TEST(test_eval_graph);
    CPPUNIT_TEST_SUITE_END();

    dps::ConnectionGraph *cg;
public:
    void setUp(){
        cg =  new dps::ConnectionGraph();
    }

    void tearDown(){
        delete cg;
    }

    void test_add_param_condition(){

    }

    void test_remove_param_condition(){

    }

    void test_add_connection(){
        // check if we have no parameters
        CPPUNIT_ASSERT_MESSAGE("Connection graph is not empty before insertion",cg->params_.size()==0);
        CPPUNIT_ASSERT_MESSAGE("Connection graph is not empty before insertion",cg->param_graph_.size()==0);
        // insert some conditions
        cg->add_connection<int64_t>({
                               {1,{dps::Operation::Lt, 200}},
                               {2,{dps::Operation::Lt, 300}}
                           });
        // check if all inserted parameters exists
        // check tiers are correctly sorted
    }

    void test_remove_connection(){
        // requires test_add_connection success

        // insert some conditions
        // remove condition
        // make sure removed parameters has disappeared
        // make sure removed chain has disapperared
    }

    void test_eval_node(){

    }

    void test_eval_graph(){

    }

};
