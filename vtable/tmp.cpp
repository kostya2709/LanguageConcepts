#include <iostream>

struct Base1 {
    virtual void foo() {
        std::cout << "Base1\n";
    }
};

struct Base2 {
    virtual void foo() {
        std::cout << "Base2\n";
    }

    virtual void Base2Method() {
        std::cout << "Are you Base2?\n";
    }
};

struct Derived: Base1, Base2 {
    void foo() {
        std::cout << "Derived\n";
    }
};

int main() {
    Derived d;
    d.Base2Method();
    Base1* db1 = &d;
    Base2* db2 = &d;
    db1->Base2Method();
    db2->Base2Method();
}