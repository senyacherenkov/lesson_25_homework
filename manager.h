#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <map>
#include <queue>
#include <cassert>
#include <list>

#include "observer.h"
#include "subject.h"
#include "utility.h"

struct Context {
    Context() = default;
    Context(size_t N):
        m_reader(N),
        m_bulkSize(N)
    {}

    std::string                     m_data;
    bool                            m_isValid = false;
    std::shared_ptr<Registrator>    m_registrator = nullptr;
    Reader                          m_reader;
    size_t                          m_bulkSize;
};

// 2. Manager class
class Manager {
public:
        Manager();
        ~Manager();

        static Manager& getInstance();

        size_t start(size_t N);
        void work(size_t handle, const char *data, std::size_t size);
        void stop(size_t handle);

private:

        std::mutex              m_mutex;
        static Manager*         m_instance;

        std::list<Context>      m_contexts;
        std::queue<size_t>      m_freeContextIDs;
        void subroutine(size_t handle, std::string &commands);
};
