#ifndef BUFFER_HPP
#define BUFFER_HPP
#include <string>
#include <vector>

// Enum for managing the buffer's mode (Insert, Normal, Command, etc.)
enum class BufferMode {
    Normal,
    Insert,
    Command
};

// The Buffer class which represents the text buffer
class Buffer {
    public:
        // Constructor
        Buffer();

        // Buffer initialization
        void initBuffer();
        
        // File management
        void loadFile(const std::string& path);
        void saveFile(const std::string& path);

        // Character operations
        void insertChar(int row, int col, char c);
        void deleteChar(int row, int col);

        // Line operations
        std::string getLine(int row);

        // Mode management
        BufferMode getMode();
        void setMode(BufferMode mode);

    private:
        std::vector<std::string> lines;  // Stores the content of the buffer as lines of text
        BufferMode mode;  // Current mode of the buffer (e.g., Normal, Insert, Command)
};

#endif // BUFFER_HPP