#ifndef SIMPLECOUNTER_H
#define SIMPLECOUNTER_H

// 类的声明
class SimpleCounter
{
public:
    // 构造函数：在创建对象时自动调用，用于初始化
    SimpleCounter();

    // 公有(public)成员函数: 外部代码可以直接调用
    void increment();       // 计数加一
    void clear();           // 计数清零
    int getCount() const;   // 获取当前计数值

private:
    // 私有(private)成员变量: 外部代码无法直接访问，实现了封装和保护
    int m_count;
};

#endif // SIMPLECOUNTER_H
