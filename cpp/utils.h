#ifndef PHMASK_UTILS_H
#define PHMASK_UTILS_H

#include <iostream>
#include <memory>
#include <vector>
#include <string>

namespace Phmask
{
    std::unique_ptr<std::istream>
    table_stream_ptr(std::string &);

    void
    fields_from_row(std::istream &, std::vector<std::string> &);

    std::vector<std::string>
    fields_from_row(std::istream &);
}

#endif

