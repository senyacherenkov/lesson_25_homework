#include "manager.h"

Manager::Manager()
{}

Manager::~Manager()
{}

Manager* Manager::m_instance = nullptr;

Manager& Manager::getInstance()
{
    if(m_instance == nullptr)
        m_instance = new Manager();
    return *m_instance;
}

size_t Manager::start(size_t N)
{
   Context data(N);
   data.m_registrator = std::make_shared<Registrator>();
   data.m_reader.addObserver(data.m_registrator);

   if(!m_freeContextIDs.empty()) {
       std::lock_guard<std::mutex> guard{m_mutex};

       size_t temp = m_freeContextIDs.front();
       m_freeContextIDs.pop();

       *(std::next(m_contexts.begin(), static_cast<long>(temp))) = data;
       return temp;
   }

   std::unique_lock<std::mutex> guard{m_mutex};
   m_contexts.push_back(std::move(data));
   guard.unlock();

   return m_contexts.size() - 1;
}

void Manager::work(size_t handle, const char *data, std::size_t size)
{
    assert(handle <= m_contexts.size());
    std::unique_lock<std::mutex> guard{m_mutex};
    std::string commands = std::string(data, size);
    std::next(m_contexts.begin(), static_cast<long>(handle))->m_data += commands;

    if(commands[commands.size() - 1] == escChar)
    {
        std::next(m_contexts.begin(), static_cast<long>(handle))->m_isValid = true;

        guard.unlock();
        auto preparedData = parseData(std::next(m_contexts.begin(), static_cast<long>(handle))->m_data);
        for(auto& command: preparedData)
            std::next(m_contexts.begin(), static_cast<long>(handle))->m_reader.readCommands(command);

        guard.lock();
        std::next(m_contexts.begin(), static_cast<long>(handle))->m_data.clear();
    }
}

void Manager::stop(size_t handle)
{
    std::lock_guard<std::mutex> guard{m_mutex};
    std::string stopStr("");
    std::next(m_contexts.begin(), static_cast<long>(handle))->m_reader.readCommands(stopStr);
    *(std::next(m_contexts.begin(), static_cast<long>(handle))) = Context();
    m_freeContextIDs.push(handle);
}
