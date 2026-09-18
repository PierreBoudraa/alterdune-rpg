#pragma once
#include <string>
#include <map>

struct ActAction {
    std::string id;
    std::string text;
    int         mercyImpact;
};

class ActCatalogue {
public:
    static const std::map<std::string, ActAction>& getCatalogue();
    static const ActAction* getAction(const std::string& id);
};
