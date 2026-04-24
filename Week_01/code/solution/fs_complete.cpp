#include <iostream>
#include <cstdint>
#include <cstring>

struct File;

enum class Permission : uint8_t {
    READ = 1 << 0,
    WRITE = 1 << 1,
    TRUNC = 1 << 2,
    APP = 1 << 3,
    EXEC = 1 << 4
};

const char* enumToString(Permission p) {
    switch (p) {
        case Permission::READ: return "READ";
        case Permission::WRITE: return "WRITE";
        case Permission::TRUNC: return "TRUNC";
        case Permission::APP: return "APP";
        case Permission::EXEC: return "EXEC";
        default: return "UNKNOWN";
    }
    return nullptr;
}

using Command = bool(*)(const char*, File&);

bool hasPermission(File f, Permission p);

struct File {
    const char* name = nullptr;
    
    uint8_t permissions = 0; 

    char* content = nullptr;
    const size_t content_capacity = 1000;

    void info() {
        std::cout << name << " [";
        for (int i = 0; i < 5; i++) {
            Permission p = static_cast<Permission>(1 << i);
            if (hasPermission(*this, p)) {
                std::cout << enumToString(p) << " ";
            }
        }
        std::cout << "]\n";
    }

    bool execute(const char* to_add, Command c) {
        if (!hasPermission(*this, Permission::EXEC)) {
            std::cout << "Cant perform action, file has no such permission\n";
            return false;
        }

        return c(to_add, *this);
    }
};

void grantPermission(File& f, Permission p) {
    f.permissions |= static_cast<uint8_t>(p);
}

void revokePermission(File& f, Permission p) {
    f.permissions &= ~static_cast<uint8_t>(p);
}

bool hasPermission(File f, Permission p) {
    return f.permissions & static_cast<uint8_t>(p);
}

bool read(const char* s, File& f) {
    if (!hasPermission(f, Permission::READ)) {
        std::cout << "Cant perform action, file has no such permission\n";
        return false;
    }

    if (f.content == nullptr || f.content[0] == '\0') {
        std::cout << "empty\n";
    } else {
        std::cout << f.content << "\n";
    }
    return true;
}


bool write(const char* s, File& f) {
    if (!hasPermission(f, Permission::WRITE)) {
        std::cout << "Cant perform action, file has no such permission\n";
        return false;
    }

    size_t len = strlen(s);
    if (len >= f.content_capacity) {
        std::cout << "Not enough space to write\n";
        return false;
    }
    strcpy(f.content, s);
    return true;    
}

bool truncate(const char* s, File& f) {
    if (!hasPermission(f, Permission::TRUNC)) {
        std::cout << "Cant perform action, file has no such permission\n";
        return false;
    }

    if (strlen(s) >= f.content_capacity) {
        std::cout << "Not enough space to truncate\n";
        return false;
    }
    strcpy(f.content, s);
    return true;
}

bool append(const char* s, File& f) {
    if (!hasPermission(f, Permission::APP)) {
        std::cout << "Cant perform action, file has no such permission\n";
        return false;
    }

    size_t len = strlen(s);
    size_t currentLen = strlen(f.content);
    if (currentLen + len >= f.content_capacity) {
        std::cout << "Not enough space to append\n";
        return false;
    }
    strcat(f.content, s);
    return true;
}

int main() {
    std::cout << "=== Unix File System Simulation Tests ===\n\n";
    
    // Test 1: File Creation and Basic Info
    std::cout << "Test 1: File Creation\n";
    File config;
    config.name = "config.txt";
    config.content = new char[config.content_capacity];
    config.content[0] = '\0';
    std::cout << "Created file: ";
    config.info();
    std::cout << "\n";

    // Test 2: Permission Management
    std::cout << "Test 2: Permission Management\n";
    grantPermission(config, Permission::READ);
    grantPermission(config, Permission::WRITE);
    
    if (hasPermission(config, Permission::READ)) std::cout << "Read access: OK\n";
    if (hasPermission(config, Permission::WRITE)) std::cout << "Write access: OK\n";
    if (hasPermission(config, Permission::EXEC)) std::cout << "Execute access: OK\n";
    else std::cout << "Execute access: NOT GRANTED (correct)\n";
    
    revokePermission(config, Permission::WRITE);
    if (!hasPermission(config, Permission::WRITE)) std::cout << "Write access revoked: OK\n";
    std::cout << "\n";

    // Test 3: File Operations - Read from empty file
    std::cout << "Test 3: Reading from empty file\n";
    grantPermission(config, Permission::READ);
    read("", config); // Should print "empty"
    std::cout << "\n";

    // Test 4: File Operations - Write without permission
    std::cout << "Test 4: Write without permission\n";
    revokePermission(config, Permission::WRITE);
    write("Hello World", config); // Should fail
    std::cout << "\n";

    // Test 5: File Operations - Write with permission
    std::cout << "Test 5: Write with permission\n";
    grantPermission(config, Permission::WRITE);
    if (write("Hello World", config)) {
        std::cout << "Write successful\n";
        read("", config); // Should print "Hello World"
    }
    std::cout << "\n";

    // Test 6: File Operations - Truncate
    std::cout << "Test 6: Truncate operation\n";
    grantPermission(config, Permission::TRUNC);
    if (truncate("New Content", config)) {
        std::cout << "Truncate successful\n";
        read("", config); // Should print "New Content"
    }
    std::cout << "\n";

    // Test 7: File Operations - Append
    std::cout << "Test 7: Append operation\n";
    grantPermission(config, Permission::APP);
    if (append(" - Appended", config)) {
        std::cout << "Append successful\n";
        read("", config); // Should print "New Content - Appended"
    }
    std::cout << "\n";

    // Test 8: Execute function without execute permission
    std::cout << "Test 8: Execute without permission\n";
    revokePermission(config, Permission::EXEC);
    config.execute("test data", write); // Should fail
    std::cout << "\n";

    // Test 9: Execute function with execute permission
    std::cout << "Test 9: Execute with permission\n";
    grantPermission(config, Permission::EXEC);
    grantPermission(config, Permission::WRITE);
    config.execute("executed content", write); // Should succeed
    std::cout << "\n";

    // Test 10: Permission enumToString function
    std::cout << "Test 10: Permission to String conversion\n";
    std::cout << "READ: " << enumToString(Permission::READ) << "\n";
    std::cout << "WRITE: " << enumToString(Permission::WRITE) << "\n";
    std::cout << "TRUNC: " << enumToString(Permission::TRUNC) << "\n";
    std::cout << "APP: " << enumToString(Permission::APP) << "\n";
    std::cout << "EXEC: " << enumToString(Permission::EXEC) << "\n";
    std::cout << "\n";

    // Test 11: Multiple permissions at once
    std::cout << "Test 11: Multiple permissions\n";
    File multiPerms;
    multiPerms.name = "multi.txt";
    multiPerms.content = new char[multiPerms.content_capacity];
    multiPerms.content[0] = '\0';
    
    grantPermission(multiPerms, Permission::READ);
    grantPermission(multiPerms, Permission::WRITE);
    grantPermission(multiPerms, Permission::APP);
    grantPermission(multiPerms, Permission::EXEC);
    
    std::cout << "File with multiple permissions: ";
    multiPerms.info();
    std::cout << "\n";

    // Test 12: Edge case - operations on file without proper permissions
    std::cout << "Test 12: Operations without permissions\n";
    File restricted;
    restricted.name = "restricted.txt";
    restricted.content = new char[restricted.content_capacity];
    restricted.content[0] = '\0';
    
    // Try all operations without any permissions
    read("", restricted);
    write("test", restricted);
    truncate("test", restricted);
    append("test", restricted);
    restricted.execute("test", read);
    std::cout << "\n";

    // Test 13: Final file info display
    std::cout << "Test 13: Final file states\n";
    std::cout << "Config file: ";
    config.info();
    std::cout << "Multi-permission file: ";
    multiPerms.info();
    std::cout << "Restricted file: ";
    restricted.info();

    // Cleanup
    delete[] config.content;
    delete[] multiPerms.content;
    delete[] restricted.content;

    std::cout << "\n=== All tests completed ===\n";
    return 0;
}