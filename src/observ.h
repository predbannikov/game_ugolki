#ifndef OBSERV_H
#define OBSERV_H
#include <vector>

/* Класс для View объектов, получает уведомления если необходимо перерисовать объект*/
class Observer
{
public:
    virtual void update() = 0;
};

/* Класс для модели, где будут храниться все данные и алгоритмы */
class Observable
{
public:
    void addObserver(Observer* observer)
    {
        _observers.push_back(observer);
    }
    void notifyUpdate()
    {
        int size = _observers.size();
        for (int i = 0; i < size; i++)
        {
            _observers[i]->update();
        }
    }
private:
    std::vector<Observer*> _observers;
};

#endif // OBSERV_H

