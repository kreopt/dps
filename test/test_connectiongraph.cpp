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
                           },1);
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
                           },1);

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
        cg->add_connection<int64_t>({
                               {1,{dps::Operation::Lt, 200}},
                               {2,{dps::Operation::Lt, 300}}
                           },1);
        dps::chain_index_t c2 = cg->add_connection<int64_t>({
                               {1,{dps::Operation::Lt, 200}},
                               {3,{dps::Operation::Lt, 300}}
                           },1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid amount of parameters in param set",3ul, cg->params_.size());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid amount of parameters in tier graph",2ul, cg->param_graph_.size());
        // remove condition
        cg->remove_connection(c2);
        // make sure removed parameters has disappeared
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid amount of parameters in result param set",2ul, cg->params_.size());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid amount of parameters in result tier graph",1ul, cg->param_graph_.size());
        // make sure removed chain has disapperared
        CPPUNIT_ASSERT_MESSAGE("Chain reference remains in parameter after deletion", cg->params_.at(1)->chains().find(c2)==cg->params_.at(1)->chains().end());
    }

    void test_eval_node(){
        /*dps::connection_list_t<dps::string_t> s_connection_list={
            {
                {6,{dps::Operation::Eq, 10}},
                {7,{dps::Operation::Contains, "test"}}
            }
        };*/
        dps::connection_list_t<int64_t> connection_list={
            {
                {1,{dps::Operation::Eq, 10}},
                {2,{dps::Operation::Lt, 3}},
                {5,{dps::Operation::Gt, 7}},
            },
            {
                {2,{dps::Operation::Eq, 0}},
                {3,{dps::Operation::Eq, 4}},
                {4,{dps::Operation::Neq, 2}}
            },
            {
                {1,{dps::Operation::Gt, 6}},
                {2,{dps::Operation::Eq, 8}},
                {5,{dps::Operation::Eq, 4}}
            },
            {
                {2,{dps::Operation::Lt, 7}},
                {4,{dps::Operation::Eq, 6}},
            },
            {
                {1,{dps::Operation::Eq, 0}},
                {2,{dps::Operation::Eq, 1}},
                {3,{dps::Operation::Eq, 2}},
                {4,{dps::Operation::Eq, 3}},
                {5,{dps::Operation::Eq, 4}},
            }
        };
        int i=0;
        std::vector<dps::chain_index_t> chains;
        for (auto conn: connection_list){
            chains.push_back(cg->add_connection(conn, i));
            i++;
        }
        dps::connection_t<dps::string_t> p = {{7,{dps::Operation::Contains, "test"}}};
        cg->add_connection(p, 0, chains.at(0));

        std::unordered_set<dps::subscriber_index_t> subscribers;
        dps::param_list_t<int64_t> param_list={
            {1,3},
            {2,5},
            {3,2},
            {4,6},
            {5,3}
        };
        cg->eval<int64_t>(param_list, subscribers);
        std::vector<dps::subscriber_index_t> correct_subscribers={3};

        CPPUNIT_ASSERT_MESSAGE("Bad subscribers result", correct_subscribers.size()==subscribers.size());
        for (auto sub: correct_subscribers){
            if (subscribers.find(sub)==subscribers.end()){
                CPPUNIT_FAIL("Subscribers results not match");
            }
        }
    }

    void test_eval_graph(){

    }

};
