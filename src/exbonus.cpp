#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <windows.h> // for UTF-8 support on Windows

enum class CardKind {
    Heart,
    Diamond,
    Club,
    Spade,
};

enum class CardValue {
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,
    Queen,
    King,
    Ace,
};

struct Card {
    CardKind kind;
    CardValue value;

    bool operator==(Card const& b) const {
        return kind == b.kind && value == b.value;
    }

    size_t hash() const {
        // no collision for 52, because 
        // 4 kinds (0-3), 13 values (0-12)
        // max: 4*13 = 52 = the numbers of cards in a deck
        // intervals for hash based on kind : [0; 12], [13; 25], [26; 38], [39; 51]
        return static_cast<size_t>(kind) * 13 + static_cast<size_t>(value);
    }
};

///////////////// given stuffs

std::vector<Card> get_cards(size_t const size) {
    std::vector<Card> cards {};
    cards.reserve(size);
    for (size_t i {0}; i < size; ++i) {
        cards.push_back({static_cast<CardKind>(rand() % 4), static_cast<CardValue>(rand() % 13)});
    }
    return cards;
}

namespace std {
    template<>
    struct hash<Card> {
        size_t operator()(Card const& card) const {
            return card.hash();
        }
    };
}

//rewrote it because it felt clumsy ('00' for the ace)
//however it suck because the order of the stuffs in enum matter
std::string card_name(Card const& card) {
    static const char* value_names[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    static const char* kind_names[] = {"♥", "♦", "♣", "♠"};

    std::string name = value_names[static_cast<size_t>(card.value)];
    name += " of ";
    name += kind_names[static_cast<size_t>(card.kind)];
    return name;
}

// so I can do std::cout << card directly
std::ostream& operator<<(std::ostream& os, Card const& c) {
    os << card_name(c);
    return os;
}

///////////////// pretty test stuff I used on tictactoe

template <typename T>
void TEST_RESULT_OUTPUT(T value,T oracle) {
    if (oracle == value) {
        std::cout << "\033[32mTest passé, " << 
        "Oracle: " << std::to_string(oracle) <<
        " Valeur: " << std::to_string(value) <<
        "\033[0m\n";
    } else {
        std::cout << "\033[31mTest raté, " << 
        "Valeur: " << std::to_string(value) <<
        " Oracle: " << std::to_string(oracle) <<
        "\033[0m\n";
    }
};

/////////////////

int main() {
    #ifdef _WIN32
        // Tell Windows console to use UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    /////////////


    std::unordered_map<Card, int> card_count {};
    std::vector<Card> cards = get_cards(100);
    //inc if in
    for (Card const& card : cards) {
        card_count[card]++;
    }

    for (auto const& pair : card_count) {
        std::cout << pair.first << " : " << pair.second << " fois\n";
    }

    return 0;
}
