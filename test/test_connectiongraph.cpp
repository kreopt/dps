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
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Connection param set is not empty before insertion",0ul,cg->params_.size());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Connection tier graph is not empty before insertion",0ul,cg->param_graph_.size());
        // insert some conditions
        dps::chain_index_t c1 = cg->add_connection<int64_t>({
                               {1,{dps::Operation::Lt, 200}},
                               {2,{dps::Operation::Lt, 300}}
                           });
        // check if all inserted parameters exists
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid amount of parameters in result param set",2ul, cg->params_.size());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid chain size (1)", 1ul, cg->params_.at(1)->chains().size());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid chain size (2)", 1ul, cg->params_.at(2)->chains().size());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid amount of parameters in result tier graph",1ul, cg->param_graph_.size());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong chain id returned", 1u, *cg->params_.at(1)->chains().begin());

        dps::chain_index_t c2 = cg->add_connection<int64_t>({
                               {1,{dps::Operation::Lt, 100}},
                               {1,{dps::Operation::Lt, 300}},
                               {1,{dps::Operation::Gt, 100}}
                           });

        CPPUNIT_ASSERT_MESSAGE("Chain identifiers are equal", c1!=c2);

        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid amount of parameters in result param set (2)",2ul, cg->params_.size());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid chain size (1) (2)", 2ul, cg->params_.at(1)->chains().size());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid chain size (2) (2)", 1ul, cg->params_.at(2)->chains().size());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid amount of parameters in result tier graph (2)",2ul, cg->param_graph_.size());
        // check tiers are correctly sorted
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid tier sorting", (int)1, (int)cg->param_graph_.at(0).at(0)->id());
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
