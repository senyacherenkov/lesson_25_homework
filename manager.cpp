#include <cassert>
#include "manager.h"

namespace {
    struct InputData {
        std::string mixedData;
        std::string nonmixedData;
    };

    int isDataWithBracket(std::string data) {
        std::string openBracketCommand("{\n");
        size_t pos = 0;
        if((pos = data.find(openBracketCommand)) != std::string::npos)
            return static_cast<int>(pos);
        return -1;
    }

    InputData extractDataInBracket(std::string data, int startPos) {
        InputData result;        
        std::string closeBracketCommand("}");
        size_t endPos = 0;
        if((endPos = data.find_last_of(closeBracketCommand)) == std::string::npos)
            return result;

        result.nonmixedData = data.substr(static_cast<size_t>(startPos),
                                          endPos - static_cast<size_t>(startPos) + closeBracketCommand.size() + 1);

        std::string mixedDataPart1;
        std::string mixedDataPart2;

        if(startPos != 0)
            mixedDataPart1 = data.substr(0, static_cast<size_t>(startPos));

        if(endPos != data.size() - 1)
            mixedDataPart2 = data.substr(endPos + 2);

        result.mixedData = mixedDataPart1 + mixedDataPart2;
        return result;
    }
}

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

void Manager::subroutine(size_t handle, std::string& commands)
{
    std::unique_lock<std::mutex> guard{m_mutex};
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

void Manager::work(size_t handle, const char *data, std::size_t size)
{
    assert(handle <= m_contexts.size());    
    std::string commands = std::string(data, size);
    InputData tempData;
    tempData.mixedData = commands;

    int posOfBracket = 0;
    if((posOfBracket = isDataWithBracket(commands)) != -1 ) {
        tempData = extractDataInBracket(commands, posOfBracket);

        std::unique_lock<std::mutex> guard{m_mutex};
        if(m_contexts.size() > 1) {
            subroutine(m_contexts.size(), tempData.nonmixedData);
        }
        else {
            guard.unlock();
            size_t newContextId = start(std::next(m_contexts.begin(), static_cast<long>(handle))->m_bulkSize);
            subroutine(newContextId, tempData.nonmixedData);
        }
    }

    if(!tempData.mixedData.empty())
        subroutine(handle, tempData.mixedData); //if string with data is empty it triggers stop condition
}

void Manager::stop(size_t handle)
{
    std::lock_guard<std::mutex> guard{m_mutex};
    std::string stopStr("");
    std::next(m_contexts.begin(), static_cast<long>(handle))->m_reader.readCommands(stopStr);
    *(std::next(m_contexts.begin(), static_cast<long>(handle))) = Context();
    m_freeContextIDs.push(handle);
}
