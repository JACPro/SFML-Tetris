#pragma once

#include "Observer.hpp"

template <typename T>
class Observable {
private:
    std::vector<IObserver<T>*> observers;

public:
    void AddObserver(IObserver<T>* observer) {
        observers.push_back(observer);
    }

    void RemoveObserver(IObserver<T>* observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void NotifyObservers(const T& value) {
        for (IObserver<T>* observer : observers) {
            observer->Notify(value);
        }
    }
};
