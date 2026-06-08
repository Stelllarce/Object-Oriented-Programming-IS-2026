#include <iostream>

void f();

void f() {}

class Base {
    public: 
    virtual void f() {return;}
};

class Derived1 : public Base {
    public:
    void f() override {std::cout << "der1::f()";}
    void der1() {std::cout << "der1";}
};

class Derived2 : public Base {
    public:
    void f() override {std::cout << "der2::f()";}
};

class Container {
public:

    Container() { 
        cont = new Base*[10];
        cont[0] = new Derived1();
        size++;
    }
    ~Container() {  for (size_t i = 0; i < size; i++)
    {
        delete cont[i];
    }
        delete[] cont;
     }
    void container() {
        if (Derived1* d = dynamic_cast<Derived1*>(cont[0])) {
            d->der1();
        }
        else
            d->f();
    }
private:
    Base** cont;
    int size = 0;
    int capacity = 10;
};

int main() {
    Container a;
    a.container();
    return 0;
}