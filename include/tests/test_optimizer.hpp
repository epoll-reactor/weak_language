#ifndef TEST_OPTIMIZER_HPP
#define TEST_OPTIMIZER_HPP

#include "../../include/optimizer/optimizer.hpp"

#include "../../include/tests/test_semantic.hpp"

namespace optimizer_detail {

void expect_error(std::string_view data)
{
    trace_error(data, [&data]{
        boost::local_shared_ptr<ast::RootObject> parsed_trees = semantic_detail::create_parse_tree(data);

        SemanticAnalyzer analyzer(parsed_trees);
        analyzer.analyze();

        Optimizer optimizer(parsed_trees);
        optimizer.optimize();

        /// Will be skipped if exception thrown from analyzer function
        assert(false && "Error expected");
    });
}

void assert_correct(std::string_view data)
{
    boost::local_shared_ptr<ast::RootObject> parsed_trees = semantic_detail::create_parse_tree(data);

    SemanticAnalyzer analyzer(parsed_trees);
    analyzer.analyze();

    Optimizer optimizer(parsed_trees);
    optimizer.optimize();
}
} // namespace semantic_detail

void optimizer_test_binary()
{
    optimizer_detail::assert_correct("1 + 1;");
}

void run_optimizer_tests()
{
    std::cout << "Running optimizer tests...\n====\n";



    std::cout << "Optimizer tests passed successfully\n";
}

#endif // TEST_OPTIMIZER_HPP
