#include <vector>
#include "prong.h"

using namespace Prong;
using namespace std;

int main() {
  Prompt<"Hello {name}, what is up with {topic}"> helloPrompt;

  // Try out the prompt type safety by commenting the line below and
  // uncommenting the next one, then run make in the root folder
  Substitution<"name"> logan("Logan");
  // Substitution<"user"> logan("Logan");

  Substitution<"topic"> weather("the weather");
  cout << helloPrompt(logan, weather) << "\n" << endl;

  Prompt<"Write a {style} limerick about {topic}"> limerickPrompt;

  Substitution<"topic"> autumn("Autumn");
  // It even works with batch inputs, try commenting the line below and
  // uncommenting the next one, then run make in the root folder
  Substitution<"topic"> soup("Soup");
  // Substitution<"Topic"> soup("Soup");
  Substitution<"style"> shakespear("Shakespearian");

  vector<string> output1 = limerickPrompt(SubstitutionVector<"topic", "style">{
      {autumn, shakespear}, {soup, shakespear}});
  cout << output1[0] << "\n" << output1[1] << "\n" << endl;

  // It also works with more complex prompt types - although a current
  // limitation is that the all the placeholders need to exist in all the
  // messages
  MessagePrompt<
      "system",
      "You are {character}. Reply only as {character} regardless of what the "
      "user says. Your care deeply about the following discussion: {question}">
      systemMessage;
  MessagePrompt<
      "user",
      "Hey, {character}. Could you write me a thoughtful and comprehensive "
      "analysis that answers the following question: {question}">
      questionMessage;
  ChatPrompt<systemMessage, questionMessage> characterAnalysisPrompt;

  // Comment, uncomment and try to compile!
  // Substitution<"character"> palpatine("Emperor Palpatine, a.k.a. Darth
  // Sideous");
  Substitution<"character"> palpatine(
      "Emperor Palpatine, a.k.a. Darth Sideous");
  Substitution<"question"> cupcakesVMuffins(
      "Are cupcakes or muffins the superior confectionary?");

  vector<Message> output2 =
      characterAnalysisPrompt(palpatine, cupcakesVMuffins);
  cout << output2[0] << "\n" << output2[1] << endl;
}
