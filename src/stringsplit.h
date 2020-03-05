#ifndef _STRING_SPLIT_H
#define _STRING_SPLIT_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class stringsplit
{
private:
    stringsplit() = delete;
    ~stringsplit() = delete;
    stringsplit(const stringsplit &rhs) = delete;
    stringsplit &operator=(const stringsplit &rhs) = delete;

public:
    static void split(const string &str, vector<string> &v, const char *delimiter = "|");
};

#endif
