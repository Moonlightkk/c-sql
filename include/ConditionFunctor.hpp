//
// Created by Wu Mianzhi on 2023/2/7.
//

#ifndef C_SQL_CONDITIONFUNCTOR_HPP
#define C_SQL_CONDITIONFUNCTOR_HPP

#include <string>
#include <string_view>
#include <functional>

namespace cond {
    using std::function;
    using std::string;
/* Do condition judgement */
    template<class ValType>
    class ConditionFunctor {
    public:
        bool operator() (const ValType& var)const
        {
            return _operator(var, _constVar);
        }
        //void init(const ValType& constVar, function<bool(ValType, ValType)> op);
        ConditionFunctor(const ValType& constVar, function<bool(ValType, ValType)> op):_constVar(constVar),_operator(op){}
    private:
        const ValType&  _constVar;
        function<bool(ValType, ValType)> _operator;
        //bool isInited = false;
    };
}//namespace cond
#endif //C_SQL_CONDITIONFUNCTOR_HPP
