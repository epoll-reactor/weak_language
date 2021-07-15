#ifndef WEAK_OPTIMIZER_HPP
#define WEAK_OPTIMIZER_HPP

#include <boost/smart_ptr/local_shared_ptr.hpp>

#include <vector>

namespace ast { class Object; class RootObject; }

class Optimizer
{
public:
    Optimizer(boost::local_shared_ptr<ast::RootObject>& root);

    void optimize();

private:
    std::vector<boost::local_shared_ptr<ast::Object>>& m_input;
};

#endif // WEAK_OPTIMIZER_HPP
