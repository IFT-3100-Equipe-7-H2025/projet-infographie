#include <exprtk.hpp>
#include <memory>
#include <util.h>

std::function<double(double, double)> parseLambda(const std::string& expr)
{
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;

    struct Closure
    {
        double u = 0;
        double v = 0;
        symbol_table_t symbol_table;
        expression_t expression;

        explicit Closure(const std::string& expr_str)
        {
            symbol_table.add_variable("u", u);
            symbol_table.add_variable("v", v);
            symbol_table.add_constants();

            expression.register_symbol_table(symbol_table);
            parser_t parser;
            if (!parser.compile(expr_str, expression))
            {
                throw std::runtime_error("Failed to parse expression");
            }
        }

        double eval(double new_u, double new_v)
        {
            u = new_u;
            v = new_v;
            return expression.value();
        }
    };

    auto closure = std::make_shared<Closure>(expr);
    return [closure](double u, double v) {
        return closure->eval(u, v);
    };
}
