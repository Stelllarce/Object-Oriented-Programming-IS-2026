#include "Appliance.hpp"
#include <stdexcept>
#include <cstring>
#include <cstddef>
#include <iostream>

unsigned Appliance::NEXT_ID = 0;

unsigned Appliance::COUNT = 0;

Appliance::Appliance(char* descr, char* make, float power) 
    : power_(power), id_(NEXT_ID++), is_connected_(false), is_active_(false) {
        setDescr(descr);
        try
        {
            setMake(make);
        }
        catch(...)
        {
            clear();
            throw;
        }

        COUNT++;
}
// подходящи методи за осигуряване на валиден жизнен цикъл. При копиране на уред, копието задължително не е включено (не е plugged in).
// уникален сериен номер – цяло число без знак. Той трябва да е уникален за всеки уред и след създаване не се променя, дори при присвояване.


Appliance::Appliance(const Appliance& other) 
    : power_(other.power_), id_(NEXT_ID++), 
    is_connected_(false), is_active_(other.is_active_) {
    try
    {
        setDescr(other.descr_);
        setMake(other.make_);
    }
    catch(const std::exception& e)
    {
        clear();
        std::cerr << e.what() << '\n';
    }
    
}

Appliance& Appliance::operator=(const Appliance& other) {
    if (this == &other) return *this;

    Appliance temp(other);
    std::swap(descr_, temp.descr_);
    std::swap(make_, temp.make_);
    power_ = temp.power_;
    unplug();

    return *this;
}

void Appliance::setPower(float pow) {
    if (!is_connected_) power_ = pow;   
}

void Appliance::setStateActive() {
    if (is_connected_ && !is_active_) is_active_ = true; 
}

void Appliance::setStateInactive() {
    is_active_ = false;
}

void Appliance::plug() {
    is_connected_ = true;
}
void Appliance::unplug() {
    is_active_ = is_connected_ = false;
}

void Appliance::clear() {
    delete[] descr_;
    delete[] make_;
    descr_ = nullptr;
    make_ = nullptr;
}

char* Appliance::setString(const char* str) {
    if(!str) throw std::invalid_argument("Nullptr not allowed!");

    size_t len = strlen(str);
    char* temp = new char[len + 1];
    strcpy(temp, str);

    return temp;
}

void Appliance::setDescr(const char* descr) {
    char* temp = setString(descr);
    delete[] descr_;
    descr_ = temp;
}
void Appliance::setMake(const char* make) {
    char* temp = setString(make);
    delete[] make_;
    make_ = temp;
}

Appliance::~Appliance() noexcept {
    clear();
    COUNT--;
}
