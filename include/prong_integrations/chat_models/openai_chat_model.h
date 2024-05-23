#ifndef PRONG_OPENAI_CHAT_MODEL_H
#define PRONG_OPENAI_CHAT_MODEL_H

#include <cstdlib>
#include "../runnables/http_runnable.h"  // Include the base class header

namespace Prong {

class OpenAIChatModel : public HTTPRunnable<std::vector<Message>, Message> {
 private:
  std::string apiKey_ = "";
  const std::string baseUrl_ = "api.openai.com";
  const std::string port_ = "443";
  const std::string basePath_ = "/v1/chat/completions";
  std::string modelIdentifier_;
  const int version_ = 11;

 public:
  enum class ModelType { GPT_4_32K, GPT_4, GPT_4_TURBO_PREVIEW, GPT_3_5_TURBO };

  using ModelIdentifier = std::variant<std::string, ModelType>;

 private:
  static std::string getModelIdentifierAsString(ModelIdentifier m) {
    if (std::holds_alternative<ModelType>(m)) {
      switch (std::get<ModelType>(m)) {
        case ModelType::GPT_4_32K:
          return "gpt-4-32k";
        case ModelType::GPT_4:
          return "gpt-4";
        case ModelType::GPT_4_TURBO_PREVIEW:
          return "gpt-4-turbo-preview";
        case ModelType::GPT_3_5_TURBO:
          return "gpt-3.5-turbo";
        default:
          return "unknown";
      }
    } else {
      return std::get<std::string>(m);
    }
  }

 protected:
  virtual std::string getBaseUrl_() const override { return baseUrl_; }

  virtual std::string getPort_() const override { return port_; }

  virtual std::string getBasePath_() const override { return basePath_; }

  virtual boost::beast::http::verb getHTTPVerb_() const override {
    return boost::beast::http::verb::post;
  }

  virtual int getVersion_() const override { return version_; }

  std::string getRequestBody_(
      const std::vector<Message>& messages) const override {
    boost::property_tree::ptree pt;
    pt.put("model", modelIdentifier_);

    boost::property_tree::ptree messagesNode;
    for (const auto& message : messages) {
      boost::property_tree::ptree messageNode;
      messageNode.put("role", message.role);
      messageNode.put("content", message.content);
      messagesNode.push_back(std::make_pair("", messageNode));
    }

    pt.add_child("messages", messagesNode);

    std::ostringstream buf;
    boost::property_tree::write_json(
        buf, pt, false);  // Set false for no pretty printing
    return buf.str();
  }

  std::map<std::string, std::string> getHeaders_() const override {
    return {{"Authorization", "Bearer " + apiKey_},
            {"Content-Type", "application/json"}};
  }

  // Function to parse the response from the OpenAI API and construct a Message
  // object
  virtual Message parseResponse_(
      const boost::property_tree::ptree& pt) const override {
    // Navigate through the JSON structure to extract the response content
    std::string content;
    try {
      auto choices = pt.get_child("choices");
      for (const auto& choice : choices) {
        auto message = choice.second.get_child("message");
        content = message.get<std::string>("content");
        break;  // Assuming we only care about the first choice
      }
    } catch (const std::exception& e) {
      content = "Error parsing response: " + std::string(e.what());
    }

    // Construct and return the Message object with the assistant's response
    return Message("assistant", content);
  }

  virtual std::string getRequestBody_Stream(
      const std::vector<Message>& messages) const override {
    std::string jsonBody = getRequestBody_(messages);
    // Remove the trailing brace
    jsonBody.pop_back();
    jsonBody += ",\"stream\":true}";
    return jsonBody;
  }

  // TODO implement parsing for streaming

 public:
  explicit OpenAIChatModel(ModelIdentifier modelIdentifier)
      : modelIdentifier_(getModelIdentifierAsString(modelIdentifier)) {
    // Attempt to retrieve the API key from the environment variable
    const char* envApiKey = std::getenv("OPENAI_API_KEY");

    if (envApiKey) {
      // If the environment variable is found, use it
      apiKey_ = std::string(envApiKey);
    } else {
      throw std::runtime_error("OPENAI_API_KEY not set in environment");
    };
  }
};

}  // namespace Prong

#endif  // OPENAI_CHAT_MODEL_H
