#pragma once

#include <memory>
#include "remove_cv_ref.h"

class Any {
    public:
    Any() = default;

    template<typename T>
    Any(const T& val) : ptr_(new Derived(val)) {}
    Any(const Any& other) : ptr_(other.ptr_->GetCopy()) {}
    ~Any() = default;
    
    template<typename T>
    Any& operator=(const T& val) {
        ptr_ = std::make_shared<Derived<T>>(val);
        return *this;
    }
    Any& operator=(const Any& other) {
        ptr_ = other.ptr_->GetCopy();
        return *this;
    }

    private:
    template<typename T>
    friend constexpr T& AnyCast(Any& other);

    struct Base {
        virtual std::shared_ptr<Base> GetCopy() const = 0;
        virtual ~Base() {};
    };

    template<typename T>
    struct Derived : Base {
        Derived(const T& val) : val_(val) {}
        Derived(T&& val) : val_(std::move(val_)) {}
        ~Derived() override = default;
        std::shared_ptr<Base> GetCopy() const override { return std::make_shared<Derived>(val_); }

        T val_;
    };

    std::shared_ptr<Base> ptr_ = nullptr;
};

template<typename T>
constexpr T& AnyCast(Any& value) {
    if (value.ptr_ == nullptr) {
        throw std::bad_cast();
    }
    auto* ptr = dynamic_cast<Any::Derived<RemoveCVRefT<T>>*>(value.ptr_.get());
    if (!ptr){
        throw std::bad_cast();
    }

    return ptr->val_;
}
