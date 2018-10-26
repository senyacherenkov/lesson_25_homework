#pragma once
#include <iostream>
#include <vector>
#include <string>

constexpr const char escChar = '\n';

struct ThreadData {
    int m_nStrings = 0;
    int m_nBlocks = 0;
    int m_nCommands = 0;
};

std::vector<std::string> parseData(std::string data);

std::ostream& operator << (std::ostream& out, ThreadData const& data);
