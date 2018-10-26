#pragma once
#include <memory>
#include <iostream>
#include <cstdlib>
#include <list>
#include <chrono>
#include "observer.h"
#include "utility.h"

using seconds_t = std::chrono::seconds;

class Subject {
public:
    Subject() = default;
    virtual ~Subject();

    void addObserver(const std::shared_ptr<Observer>& observer);

    void removeObserver(const std::shared_ptr<Observer>& observer);

    virtual void notifyObservers() = 0;

protected:
    std::list<std::shared_ptr<Observer>> m_observers;
};



class Reader: public Subject {
public:
    Reader() = default;
    Reader(std::size_t N):
        m_N(N)
    {}

    ~Reader() = default;

    void readCommands(const std::string &input);
    void setCommandNumber(size_t N) { m_N = N; }

    ThreadData& getThreadData() { return m_threadData; }
private:
    virtual void notifyObservers();
    void printSummary();

private:
    std::size_t                 m_N = 0;
    std::vector<std::string>    m_commands;
    long                        m_timeOfFirstCommand = 0;
    ThreadData                  m_threadData;
    bool                        m_dynamicMode = false;
    int                         m_openBracketNumber = 0;
    int                         m_closeBracketNumber = 0;
};

std::ostream& operator << (std::ostream& out, ThreadData const& data);
