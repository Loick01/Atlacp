using Callback = std::function<void()>;

// https://trussel.ch/cpp/design%20patterns/2020/11/11/observer-simple.html
class Notifier
{
    public:
        std::vector<Callback> m_callbacks;

        void Notify()
        {
            for (Callback& c : m_callbacks)
                c();
        }

        void AddCallback(Callback c)
        {
            m_callbacks.push_back(c);
        }

};