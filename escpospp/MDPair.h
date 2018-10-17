#ifndef INCLUDE_MDPAIR_H
#define INCLUDE_MDPAIR_H

#include <string>
#include <map>
#include <utility>

/*
Class to pass from markdown to escpos commands

Author: Carlos Malanche, October 2018.
*/
class MDPair: public std::pair<std::string, std::string>{
public:
    static MDPair getMatching(std::string toMatch);
private:
    MDPair(std::string id);
    static void initializeMap();
    static std::map<std::string, MDPair *> mdPairMap;
    std::string id;
};

#endif //INCLUDE_MDPAIR_H