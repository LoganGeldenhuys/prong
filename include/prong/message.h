#ifndef PRONG_MESSAGE_H
#define PRONG_MESSAGE_H

#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>

namespace Prong {

// Simple message class, has content, role and << operator
class Message {
 public:
  std::string content;
  std::string role;

  Message(const std::string& role, const std::string& content)
      : role(role), content(content) {}
};

std::ostream& operator<<(std::ostream& os, const Message& msg) {
  std::ostringstream ss;
  ss << msg.role << ": " << msg.content << "\n";
  os << ss.str();
  return os;
}

}  // namespace Prong

#endif  // PRONG_MESSAGE_H
