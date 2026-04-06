struct ServerConfig {
    const char* host;
    int         port;
    int         timeout;
    bool        useTls;
    int         maxConnections;
};

class ServerConfigBuilder {
    ServerConfig config;

public:
    ServerConfigBuilder() {
        // set sensible defaults
        config.host           = "localhost";
        config.port           = 80;
        config.timeout        = 30;
        config.useTls         = false;
        config.maxConnections = 10;
    }

    ServerConfigBuilder& setHost(const char* host) {
        config.host = host;
        return *this; // enables method chaining
    }

    ServerConfigBuilder& setPort(int port) {
        config.port = port;
        return *this;
    }

    ServerConfigBuilder& setTimeout(int seconds) {
        config.timeout = seconds;
        return *this;
    }

    ServerConfigBuilder& enableTls() {
        config.useTls = true;
        config.port   = 443; // sensible side-effect
        return *this;
    }

    ServerConfigBuilder& setMaxConnections(int n) {
        config.maxConnections = n;
        return *this;
    }

    ServerConfig build() const {
        return config; // return by value – the product
    }
};