#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "json_fwd.hpp"

#include <string>

nlohmann::json preprocess(std::string const &rootFilename);

#endif // PREPROCESSOR_H
