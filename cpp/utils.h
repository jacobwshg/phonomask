#ifndef PHMASK_UTILS_H
#define PHMASK_UTILS_H

#include <iostream>
#include <vector>
#include <string>

namespace Phmask
{
    void
    fields_from_row(std::istream &, std::vector<std::string> &);

    std::vector<std::string>
    fields_from_row(std::istream &);
}

#endif

