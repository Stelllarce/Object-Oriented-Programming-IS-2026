#include <iostream>
#include <cstdint>
#include <cstring>

struct File;

// Make this as small as possible
enum class Permission {
    // Set these appropriate integers, so they can be used as masks later
    READ,
    WRITE,
    TRUNC,
    APP,
    EXEC
};

const char* enumToString(Permission p) {
    // Complete the function that converts a permission enum to string
    return nullptr;
}

using Command = bool(*)(const char*, File&);

bool hasPermission(File f, Permission p);

struct File {
    const char* name = nullptr;

    // Uncomment and add appropriate type, 
    // Try to make it as memory efficient as possible
    
    // <type> permissions = 0; 

    const size_t content_capacity = 1000;
    char* content = nullptr;

    void info() {
        // Print the name here

        // List all permissions the file has
        // enumToString might come in handy here
    }

    bool execute(const char* to_add, Command c) {
        // *this is magic for now, that's why its prewritten
        if (!hasPermission(*this, Permission::EXEC)) {
            std::cout << "Cant perform action, file has no such permission\n";
            return false;
        }

        return c(to_add, *this);
    }
};

void grantPermission(File& f, Permission p) {
    // Grant permission to a file
}

void revokePermission(File& f, Permission p) {
    // Remove a permission from a file
}

bool hasPermission(File f, Permission p) {
    // Check if a file has permission
    return false;
}

bool read(const char* s, File& f) {
    // Check if file has the permission
    if (true) {
        std::cout << "Cant perform action, file has no such permission\n";
        return false;
    }

    // Print the content of f, if it has none, print "empty"
}

bool write(const char* s, File& f) {
    // Check if file has the permission
    if (true) {
        std::cout << "Cant perform action, file has no such permission\n";
        return false;
    }

    // Add s to the start of the content, if the size is enough
    // If its not just report failure (return false)
}

bool truncate(const char* s, File& f) {
    // Check if file has the permission
    if (true) {
        std::cout << "Cant perform action, file has no such permission\n";
        return false;
    }

    // Delete old content and write the new one 
}

bool append(const char* s, File& f) {
    // Check if file has the permission
    if (true) {
        std::cout << "Cant perform action, file has no such permission\n";
        return false;
    }

    // Add s to the start of the content, if the size is enough
    // If the current size is smaller, resize and try to append again
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