#pragma once

// 멀티 스레드 프로그래밍을 염두한 경량 커맨드 큐
class CommandQueue
{
public:
    CommandQueue()  = default;
    ~CommandQueue() = default;

    CommandQueue(const CommandQueue&)                = delete;
    CommandQueue(CommandQueue&&) noexcept            = delete;
    CommandQueue& operator=(const CommandQueue&)     = delete;
    CommandQueue& operator=(CommandQueue&&) noexcept = delete;

    void SubmitCommand(std::function<void()> _command);   // thread-safe
    void Execute();                                       // non-thread-safe (only call one thread)

private:
    std::vector<std::function<void()>> m_queue;
    std::vector<std::function<void()>> m_pendingQueue;
    std::mutex                         m_mtx;
};
