#include "pch.h"

#include "CommandQueue.h"

void CommandQueue::SubmitCommand(const std::function<void()>& _command)
{
    std::lock_guard lock { m_mtx };
    m_queue.emplace_back(_command);
}

void CommandQueue::Execute()
{
    // 짧은 락
    {
        std::lock_guard lock { m_mtx };
        m_pendingQueue.swap(m_queue);
    }

    // 커맨드 실행
    for (const std::function<void()>& cmd: m_pendingQueue)
        cmd();

    m_pendingQueue.clear();
}