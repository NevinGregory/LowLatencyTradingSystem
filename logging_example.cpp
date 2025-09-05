#include "logging.h"

int main(int, char **) {
    using namespace Common;

    char c = 'd';
    int i = 3;
    unsigned long ul = 67;
    float f = 3.14;
    double d = 3.1415926;
    const char* s = "c_string nevin";
    std::string ss = "regular string nevin";

    Logger logger("logging_example.log");

    logger.log("Logging a char:% an int:% and an unsigned:%\n", c, i, ul);
    logger.log("Logging a float:% and double:%\n", f, d);
    logger.log("Logging C-string: '%'\n", s);
    logger.log("Logging string: '%'\n", ss);
}
