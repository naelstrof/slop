#ifndef N_OPTIONS_H_
#define N_OPTIONS_H_

#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
#include <vector>
#include <glm/glm.hpp>

static std::string validStringArguments[] = { "borderSize", "padding", "color", "shader", "highlight", "format" };
static char validCharArguments[] = { 'b', 'p', 'c', 's', 'h', 'f' };
// 0 for flag, 1 for how many arguments to eat up
static unsigned int isFlagArgument[] = { false, false, false, false, true, false };
static unsigned int validArgumentCount = 6;
static unsigned int maxFloatingValues = 0;

class Options {
private:
    std::vector<std::string> arguments;
    std::vector<std::string> values;
    std::vector<std::string> floatingValues;
    int parseCharOption( int argc, char** argv, int argumentIndex, int validIndex );
    int parseStringOption( int argc, char** argv, int argumentIndex, int validIndex );
    int validateStringOption( int argc, char** argv, int argumentIndex );
    int validateCharOption( int argc, char** argv, int argumentIndex );
    void validate( int argc, char** argv );
public:
    Options( int argc, char** argv );
    bool getFloat( std::string name, char namec, float& found );
    bool getInt( std::string name, char namec, int& found );
    bool getString( std::string name, char namec, std::string& found );
    bool getColor( std::string name, char namec, glm::vec4& found );
    bool getBool( std::string name, char namec, bool& found );
};

#endif // N_OPTIONS_H_
