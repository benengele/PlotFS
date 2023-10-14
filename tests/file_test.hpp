#define MOCK_ALL_METHODS() \
    MOCK_METHOD(void, close, (), (override)); \
    MOCK_METHOD(bool, is_open, (), (const, override)); \
    MOCK_METHOD(struct stat, stat, (), (const, override)); \
    MOCK_METHOD(uint64_t, size, (), (const, override)); \
    MOCK_METHOD(bool, seek, (off64_t), (const, override)); \
    MOCK_METHOD(bool, truncate, (off64_t), (override)); \
    MOCK_METHOD(bool, lock, (int), (override)); \
    MOCK_METHOD(bool, sync, (), (override)); \
    MOCK_METHOD(int, read, (uint8_t*, size_t), (override)); \
    MOCK_METHOD(int, write, (const uint8_t*, size_t), (override)); \
    MOCK_METHOD(int, write, (const std::string&), (override)); \
    MOCK_METHOD(ssize_t, write, (std::shared_ptr<FileHandle>, off_t*, size_t), (override)); \
    MOCK_METHOD(int, release, (), (override));
