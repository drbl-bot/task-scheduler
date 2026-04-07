#pragma once

#include <list>
#include "invoke.h"
#include "tuple.h"
#include "any.h"
#include "index_sequence.h"

class TTaskScheduler {
    public:
    class TaskSource{
        public:
        virtual void execute() {}
    };

    template<typename T>
    class TaskBase : public TaskSource{
        public:
        virtual void execute() override {}
        virtual T Get() = 0;
        private:
    };
    using iterator = std::list<std::shared_ptr<TaskSource>>::iterator;
    
    template<typename T>
    struct Holder { 
        Holder(const std::shared_ptr<TaskSource>& val) : ptr_(val) { /*std::cout << "|hc|";*/ }
        std::shared_ptr<TaskSource> ptr_;
        operator T() { 
            auto ptr = dynamic_cast<TaskBase<T>*>(ptr_.get());
            return ptr->Get();
        }
        
        using type = T;
    };


    template <typename Func, typename ...Args>
    class Task : public TaskBase<decltype(Invoke(std::declval<Func>(), std::declval<Args>()...))> {
        public:
        Task(Func f, Args ...args) : func_args_(f, args...) {}
        
        decltype(Invoke(std::declval<Func>(), std::declval<Args>()...)) Get() override {
            if (calc_)
                return AnyCast<decltype(Invoke(std::declval<Func>(), std::declval<Args>()...))>(result_);
            calc_ = 1;
            result_ = InvokeTup(func_args_, MakeIndexSequence<TupleSize<Tuple<Func, Args...>>>());
            
            return AnyCast<decltype(Invoke(std::declval<Func>(), std::declval<Args>()...))>(result_);
        }

        void execute() override {
            if (!calc_) {
                calc_ = 1;
                result_ = InvokeTup(func_args_, MakeIndexSequence<TupleSize<Tuple<Func, Args...>>>());
            }
        }

        private:
        bool calc_ = 0;
        Tuple<Func, Args...> func_args_;
        Any result_;

    };

    template <typename Func, typename ...Args>
    iterator add(Func&& f, Args&& ...args) {
        cont_.push_front(std::make_shared<Task<Func, Args...>>(std::forward<Func>(f), std::forward<Args>(args)...));

        return cont_.begin();
    }

    template <typename T>
    Holder<T> getFutureResult(iterator val) { 
        // std::cout << "|gfr|";
        return Holder<T>{*val}; }

    template <typename T>
    T getResult(iterator val) {
        // std::cout << "|gr|";
        auto ptr = dynamic_cast<TaskBase<T>*>(val->get());
        return ptr->Get();
    }

    void executeAll() {
        for (auto i: cont_) 
            i->execute();
    }

    private:
    std::list<std::shared_ptr<TaskSource>> cont_;
};

template<typename Tuple, std::size_t... Is>
auto InvokeTup(Tuple&& tuple_val, IndexSequence<Is...>) {
    return Invoke(Get<Is>(tuple_val)...);
}
