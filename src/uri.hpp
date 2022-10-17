#pragma once

#include <string>
#include <algorithm>


class URI {
public:
    URI() = default;
    URI(const std::string& path) { 
        updatePath(path); 
    }
    ~URI() = default;

    inline bool operator<(const URI& other) const { 
        return mPath < other.mPath; 
    }
    inline bool operator>(const URI& other) const { 
        return mPath > other.mPath; 
    }
    inline bool operator==(const URI& other) const {
        return mPath == other.mPath;
    }

    void updatePath(const std::string& path) {
        mPath = std::move(path);
        std::transform(mPath.begin(), mPath.end(), mPath.begin(),
                    [](char c) { return tolower(c); });
    }

    std::string getPath() const { return mPath; }
    std::string getScheme() const { return mScheme; }
    std::string getHost() const { return mHost; }
    std::uint16_t getPort() const { return mPort; }
    

private:
    std::string mPath;
    std::string mScheme;
    std::string mHost;
    std::uint16_t mPort;
};