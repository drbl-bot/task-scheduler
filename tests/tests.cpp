#include <gtest/gtest.h>

#include "..\lib\scheduler.h"

#include <string>

TEST(constructable, int_no_executeAll) {
    int a, b;
    a = 5; 
    b = 6;
    TTaskScheduler scheduler;
    auto id1 = scheduler.add([](int a, int c) {return a+4;}, a, b);
    auto id2 = scheduler.add([](int a, int c) {return c - 4;}, a, b);

    auto idF = scheduler.add([](int a, int b) { return a + b; }, scheduler.getFutureResult<int>(id1), scheduler.getFutureResult<int>(id2));
    ASSERT_EQ(scheduler.getResult<int>(idF), 11);

}

TEST(constructable, int_executeAll) {
    int a, b;
    a = 5; 
    b = 6;
    TTaskScheduler scheduler;
    auto id1 = scheduler.add([](int a, int c) {return a+4;}, a, b);
    auto id2 = scheduler.add([](int a, int c) {return c - 4;}, a, b);

    auto idF = scheduler.add([](int a, int b) { return a + b; }, scheduler.getFutureResult<int>(id1), scheduler.getFutureResult<int>(id2));
    ASSERT_EQ(scheduler.getResult<int>(idF), 11);

}

TEST(constructable, string_no_executeAll) {
    std::string a, b;
    a = "asd"; 
    b = "old";
    TTaskScheduler scheduler;
    auto id1 = scheduler.add([](std::string a, std::string c) {return a+c;}, a, b);
    auto id2 = scheduler.add([](std::string a, std::string c) {return a+c;}, b, a);

    auto idF = scheduler.add([](std::string a, std::string b) { return a < b; }, scheduler.getFutureResult<std::string>(id1), 
        scheduler.getFutureResult<std::string>(id2));

    ASSERT_TRUE(scheduler.getResult<bool>(idF));

}


TEST(constructable, strange_executeAll) {
    std::string a, b;
    a = "asd"; 
    b = "old";
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](std::string a, std::string c) {return a+c;}, a, b);
    auto id2 = scheduler.add([](std::string a, std::string c) {return a+c;}, b, a);
    

    auto idA = scheduler.add([](std::string a, std::string b) { return a < b; }, scheduler.getFutureResult<std::string>(id1), 
                        scheduler.getFutureResult<std::string>(id2));

    scheduler.executeAll();
    auto idE = scheduler.add([](std::string a, std::string c) {return a+c;}, scheduler.getFutureResult<std::string>(id1), 
                        scheduler.getFutureResult<std::string>(id2));

    auto idF = scheduler.add([](std::string a, bool b) { return ((a.size() == 12) & b) ? true : false; }, scheduler.getFutureResult<std::string>(idE), 1);
    ASSERT_TRUE(scheduler.getResult<bool>(idF));

}

TEST(constructable, saving_refs) {
    int a = 5;
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int a, int c) {return a+c;}, a, a);
    a = 7;
    ASSERT_EQ(scheduler.getResult<int>(id1), 14);

}

TEST(constructable, unusual_call) {
    int a = 5;
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int a) {return a*2;}, a);
    auto id2 = scheduler.add([](int a, int b, int c) {return a + b + c;}, a, a, a);
    ASSERT_EQ(scheduler.getResult<int>(id1), 10);
    ASSERT_EQ(scheduler.getResult<int>(id2), 15);

}

struct Helper1 {
    int add(int a, int b) { return a + b; }
};

TEST(constructable, other_class) {
    Helper1 obj;
    int a = 9;
    TTaskScheduler scheduler;
     
    auto id1 = scheduler.add(&Helper1::add, obj, a, 1);

    ASSERT_EQ(scheduler.getResult<int>(id1), 10);

}

struct Helper2 {
    Helper2() = default;
    Helper2(int val) : val_(val) {}
    Helper2(Helper2& other) = delete;
    Helper2& operator=(Helper2& other) = delete;
    Helper2(Helper2&& other) : val_(other.val_) {}
    Helper2& operator=(Helper2&& other)  { val_ = other.val_; return *this; }

    int get() const { return val_; }
    int add(int a, int b) { return a + b; }

    private:
    int val_ = 0;
};

TEST(constructable, other_uncopiable_class) {
    int a = 5;
    TTaskScheduler scheduler;
    Helper2 obj(6);

    auto id1 = scheduler.add(&Helper2::get, obj);
    auto id2 = scheduler.add(&Helper2::add, obj, a, scheduler.getFutureResult<int>(id1));
    ASSERT_EQ(scheduler.getResult<int>(id1), 6);
    ASSERT_EQ(scheduler.getResult<int>(id2), 11);

}

TEST(constructable, other_const_class) {
    int a = 5;
    TTaskScheduler scheduler;
    const Helper2 obj(6);
    Helper2 obj_e(6);


    auto id1 = scheduler.add(&Helper2::get, obj);
    auto id2 = scheduler.add(&Helper2::add, obj_e, a, scheduler.getFutureResult<int>(id1));
    ASSERT_EQ(scheduler.getResult<int>(id1), 6);
    ASSERT_EQ(scheduler.getResult<int>(id2), 11);

}

TEST(constructable, double_class) {
    TTaskScheduler scheduler;

    Helper1 obj1;
    Helper2 obj2(6);

    auto id1 = scheduler.add(&Helper2::get, obj2);
    auto id2 = scheduler.add(&Helper2::add, obj2, 5, scheduler.getFutureResult<int>(id1));

    auto id3 = scheduler.add(&Helper1::add, obj1, scheduler.getFutureResult<int>(id1), scheduler.getFutureResult<int>(id2));
    auto id4 = scheduler.add([](int a, int b) { return a + b; }, 9, 5);
    ASSERT_EQ(scheduler.getResult<int>(id3), 17);
}

TEST(constructable, double_getResult) {
    int a, b;
    a = 5; 
    b = 6;
    TTaskScheduler scheduler;
    auto id1 = scheduler.add([](int a, int c) {return a+c;}, a, b);
    
    int ans1 = scheduler.getResult<int>(id1);
    int ans2 = scheduler.getResult<int>(id1);

    
    ASSERT_EQ(ans1, ans2);

}

TEST(just_calc, test1) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([]() { return 3; });

    auto id2 = scheduler.add([](int a) { return a + 4; }, scheduler.getFutureResult<int>(id1));
    auto id3 = scheduler.add([](int a) { return a * 2; }, scheduler.getFutureResult<int>(id1));

    auto id4 = scheduler.add([](int a, int b) { return a + b; }, scheduler.getFutureResult<int>(id2), scheduler.getFutureResult<int>(id3));

    ASSERT_EQ(scheduler.getResult<int>(id4), (3 + 4) + (3 * 2)); // 7 + 6 = 13

}

TEST(just_calc, test2) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([]() { return 2; });
    auto id2 = scheduler.add([](int x) { return x * 2; }, scheduler.getFutureResult<int>(id1));
    auto id3 = scheduler.add([](int x) { return x + 1; }, scheduler.getFutureResult<int>(id2));

    auto id4 = scheduler.add([]() { return 3; });
    auto id5 = scheduler.add([](int y) { return y * 2; }, scheduler.getFutureResult<int>(id4));
    auto id6 = scheduler.add([](int y) { return y * 3; }, scheduler.getFutureResult<int>(id5));

    auto id7 = scheduler.add([](int a, int b) { return a + b; },
                            scheduler.getFutureResult<int>(id3),
                            scheduler.getFutureResult<int>(id6));

    ASSERT_EQ(scheduler.getResult<int>(id7), (2 * 2 + 1) + (3 * 2 * 3)); 

}

struct AddNumber {
    float add(float a) const {
        return a + number;
    }

    float number;
};

TEST(main_test, main_test) {
    float a = 1;
    float b = -2;
    float c = 0;
    AddNumber add{
        .number = 3
    };

    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](float a, float c) {return -4 * a * c;}, a, c);

    auto id2 = scheduler.add([](float b, float v) {return b * b + v;}, b, scheduler.getFutureResult<float>(id1));

    auto id3 = scheduler.add([](float b, float d) {return -b + std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

    auto id4 = scheduler.add([](float b, float d) {return -b - std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

    auto id5 = scheduler.add([](float a, float v) {return v / (2 * a);}, a, scheduler.getFutureResult<float>(id3));

    auto id6 = scheduler.add([](float a, float v) {return v / (2 * a);}, a, scheduler.getFutureResult<float>(id4));

    auto id7 = scheduler.add(&AddNumber::add, add, scheduler.getFutureResult<float>(id6));

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<float>(id1), -0);
    ASSERT_EQ(scheduler.getResult<float>(id2), 4);
    ASSERT_EQ(scheduler.getResult<float>(id3), 4);
    ASSERT_EQ(scheduler.getResult<float>(id4), 0);
    ASSERT_EQ(scheduler.getResult<float>(id5), 2);
    ASSERT_EQ(scheduler.getResult<float>(id6), 0);
    ASSERT_EQ(scheduler.getResult<float>(id7), 3);
    
}