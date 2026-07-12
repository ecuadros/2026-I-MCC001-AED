#ifndef __BASE_NODE_H__
#define __BASE_NODE_H__

#include <iostream>
#include <sstream>
#include <utility>

#include "../types.h"

template <typename T>
class BaseNode {
public:
    using value_type = T;

protected:
    value_type m_data;
    Ref m_ref;

public:
    BaseNode() : m_data(value_type()), m_ref(Ref()) {}
    BaseNode(value_type data, Ref ref) : m_data(data), m_ref(ref) {}
    BaseNode(const BaseNode& other) : m_data(other.m_data), m_ref(other.m_ref) {}
    BaseNode(BaseNode&& other) noexcept
        : m_data(std::move(other.m_data)), m_ref(std::exchange(other.m_ref, Ref())) {}
    BaseNode& operator=(const BaseNode& other) {
        if (this != &other) {
            m_data = other.m_data;
            m_ref = other.m_ref;
        }
        return *this;
    }
    BaseNode& operator=(BaseNode&& other) noexcept {
        if (this != &other) {
            m_data = std::move(other.m_data);
            m_ref = std::exchange(other.m_ref, Ref());
        }
        return *this;
    }

    

    value_type GetData() const { return m_data; }
    value_type& GetDataRef() { return m_data; }
    void SetData(value_type data) { m_data = data; }

    Ref GetRef() const { return m_ref; }
    Ref& GetRefRef() { return m_ref; }
    void SetRef(Ref ref) { m_ref = ref; }

    virtual std::string ToString() const {
        std::ostringstream oss;
        oss << "(" << m_data << "," << m_ref << ")";
        return oss.str();
    }

    virtual std::istream& fromIstream(std::istream& is) {
        char ch1, ch2, ch3;
        if (is >> ch1 && ch1 == '(' &&
            is >> m_data &&
            is >> ch2 && ch2 == ',' &&
            is >> m_ref &&
            is >> ch3 && ch3 == ')') {
            return is;
        }

        is.setstate(std::ios::failbit);
        return is;
    }

    virtual ~BaseNode() = default;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const BaseNode<T>& node) {
    return os << node.ToString();
}

template <typename T>
std::istream& operator>>(std::istream& is, BaseNode<T>& node) {
    return node.fromIstream(is);
}

#endif // __BASE_NODE_H__
