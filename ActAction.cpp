#include "ActAction.h"

const std::map<std::string, ActAction>& ActCatalogue::getCatalogue() {
    static const std::map<std::string, ActAction> catalogue = {
        {"JOKE",        {"JOKE",        "Tu racontes une blague... Le monstre rit nerveusement.",          +20}},
        {"COMPLIMENT",  {"COMPLIMENT",  "Tu dis qu'il a de beaux yeux. Il rougit sous ses ecailles.",      +25}},
        {"INSULT",      {"INSULT",      "Tu insultes sa maman. Il prend ca TRES mal.",                     -30}},
        {"DISCUSS",     {"DISCUSS",     "Vous parlez de la pluie et du beau temps. Ambiance bizarre.",     +15}},
        {"OBSERVE",     {"OBSERVE",     "Tu l'observes. Il se sent tres mal a l'aise.",                    +10}},
        {"PET",         {"PET",         "Tu tentes de le caresser. Il ne sait pas quoi faire.",            +20}},
        {"OFFER_SNACK", {"OFFER_SNACK", "Tu sors un sandwich. Meme les monstres ont faim.",               +30}},
        {"REASON",      {"REASON",      "Tu expliques que se battre est inutile. Il reflechit...",         +20}},
        {"DANCE",       {"DANCE",       "Tu danses. Le monstre est tellement gene qu'il oublie tout.",    +15}},
        {"TAUNT",       {"TAUNT",       "Tu le nargues avec une grimace. Il devient rouge de rage.",       -20}},
        {"MOCK",        {"MOCK",        "Tu te moques de sa coupe. Il n'a pas de cheveux. Encore pire.",  -25}},
        {"SING",        {"SING",        "Tu fredonnes une chanson d'amour. Silence genant.",              +10}},
    };
    return catalogue;
}

const ActAction* ActCatalogue::getAction(const std::string& id) {
    const auto& cat = getCatalogue();
    auto it = cat.find(id);
    if (it != cat.end()) return &it->second;
    return nullptr;
}
