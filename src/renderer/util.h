#include <functional>
#include <string>

std::function<double(double, double)> parseLambda(const std::string& expr);
