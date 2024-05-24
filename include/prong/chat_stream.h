#ifndef PRONG_STREAM_H
#define PRONG_STREAM_H

#include <streambuf>
#include <iostream>
#include <sstream>
#include <memory>
#include "runnable.h" 

namespace Prong {

class ChatStreamBuf : public std::streambuf {
private:
    ChatModel &chatModel; 
    std::ostream &outputStream;
    std::ostringstream buffer; 

public:
    ChatStreamBuf(ChatModel &model, std::ostream &os) : chatModel(model), outputStream(os) {}

protected:
    // Synchronize buffer with the chat model
    virtual int sync() {
        // Extract content from buffer
        std::string content = buffer.str();

        // Clear the buffer after copying its content
        buffer.str("");
        buffer.clear();

        // Process the content through the chat model
        Message input("user", content); 
        Message output = chatModel({input}); 

        // Write output to the associated stream
        outputStream << output; 
        return 0; 
    }

    // Collects characters into the buffer
    virtual int overflow(int c) {
        if (c != EOF) {
            // Append single character to the buffer
            buffer.put(static_cast<char>(c));
        }
        return c;
    }
};

class ChatStream : public std::ostream {
public:
    ChatStream(ChatModel &model, std::ostream &os)
        : std::ostream(new ChatStreamBuf(model, os)), outputStream(os) {} 

    ~ChatStream() {
        delete this->rdbuf(); 
    }

private:
    std::ostream &outputStream;
};

} 

#endif // PRONG_STREAM_H

