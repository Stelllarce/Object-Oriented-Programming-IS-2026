#pragma once

class Appliance {
public:
    Appliance() = delete;
    Appliance(char* descr, char* make, float power);
    Appliance(const Appliance& other);
    Appliance& operator=(const Appliance& other);

    char* getDescr() const { return descr_; }
    char* getMake() const { return make_; }
    float getPower() const { return power_; }
    // Getter for state

    bool getActiveStatus() const { return is_active_; }
    unsigned getID() const { return id_; }

    void setDescr(const char* descr);
    void setMake(const char* make);

    void setPower(float pow);
    void setStateActive();
    void setStateInactive();
    void plug();
    void unplug();


    ~Appliance() noexcept;
private:
    char* setString(const char* str);
    void clear();

    char* descr_ = nullptr;
    char* make_ = nullptr;
    float power_;
    // Where to connect
    bool is_connected_;
    bool is_active_; 
    const unsigned int id_;
    static unsigned NEXT_ID;
    static unsigned COUNT;
};