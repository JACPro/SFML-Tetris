#pragma once

#include <iostream>
#include <vector>
#include <memory>

template <typename T>
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void Notify(const T& value) = 0;  // Notify with a parameter of type T
};
