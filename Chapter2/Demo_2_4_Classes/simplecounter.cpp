#include "simplecounter.h"

// 构造函数的实现
SimpleCounter::SimpleCounter()
{
    // 在这里进行初始化工作
    m_count = 0;
}

// 成员函数的实现
void SimpleCounter::increment()
{
    m_count++;
}

void SimpleCounter::clear()
{
    m_count = 0;
}

// const关键字表示这个函数不会修改类的任何成员变量
int SimpleCounter::getCount() const
{
    return m_count;
}
