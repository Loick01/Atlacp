#pragma once

#include <functional>

enum UselessEvent // Rename
{
    None
};

// https://trussel.ch/cpp/design%20patterns/2020/11/11/observer-simple.html
template<typename Event>
class Notifier
{
    public:
        using Callback = std::function<void(const Event)>;
        std::vector<Callback> m_callbacks;

        void Notify(const Event event)
        {
            for (Callback& c : m_callbacks)
                c(event);
        }

        void AddCallback(Callback c)
        {
            m_callbacks.push_back(c);
        }

        void ClearCallback()
        {
            m_callbacks.clear();
        }

};