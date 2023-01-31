#pragma once
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <ranges>
#include <string>


// template <typename... Args>
using func = std::function<void()>;
using TEventInfo = struct SEventInfo
{
	func<> callback;
    uint32_t interval;
    std::chrono::time_point<std::chrono::system_clock> lastTime;
    bool isAlwaysUpdate;
};

namespace Interval
{
    class IntervalTimer final
    {
    public:
	    explicit IntervalTimer(TEventInfo info) : m_Info(std::move(info)) {}
        ~IntervalTimer() { Destroy(); }

        void Update()
        {
	        const auto currentTime = std::chrono::system_clock::now();
	        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_Info.lastTime).count();
            if (elapsed >= m_Info.interval)
            {
                m_Info.callback();
                if (m_Info.isAlwaysUpdate) {
                    m_Info.lastTime = currentTime;
                }
                else {
                    Destroy();
                }
            }
        }

        void Destroy()
        {
            m_Info = {};
        }

	    [[nodiscard]] uint32_t GetInterval() const { return m_Info.interval; }
	    [[nodiscard]] bool IsAlwaysUpdate() const { return m_Info.isAlwaysUpdate; }

    private:
        TEventInfo m_Info;
    };

    inline std::map<std::string, std::shared_ptr<IntervalTimer>> m_Timers;

    inline auto SetInterval(const std::string& name, const TEventInfo& info) -> void
    {
        const auto timer = std::make_shared<IntervalTimer>(info);
        m_Timers.insert_or_assign(name, timer);
#ifdef DEBUG
        fprintf(stderr, "Interval create ok, size: %u\n", m_Timers.size());
#endif
    }

    inline auto SetIntervalAlternative(const std::string& name, const func<>& callback, const uint32_t interval, const bool isAlwaysUpdate) -> void
    {
        SetInterval(name, { callback, interval, std::chrono::system_clock::now(), isAlwaysUpdate });
    }

    inline auto Update() -> void
    {
        for (const auto& timer : m_Timers | std::views::values)
        {
            timer->Update();
        }
    }

    inline auto Erase(const std::string& name) -> void
    {
        const auto& it = m_Timers.find(name);
        if (it != m_Timers.end()) { 
            m_Timers.erase(it);
        }
    }

    inline auto Destroy() -> void
    {
        m_Timers.clear();
    }
}

#define SetInterval(name, info) Interval::SetInterval(name, info)
#define setInterval(name, callback, interval, isAlwaysUpdate) (Interval::SetIntervalAlternative(name, callback, interval, isAlwaysUpdate))
static constexpr auto INTERVAL_SEC = [] (const uint32_t sec) { return 1000 * sec; };