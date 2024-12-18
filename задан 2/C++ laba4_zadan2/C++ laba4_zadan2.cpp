﻿#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <string>
#include <sstream>
#include <clocale>

// Определение класса Card (КАРТА)
class Card {
public:
    enum Rank { two = 2, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace };

    Card(Rank rank) : rank(rank) {}

    int getValue() const {
        if (rank >= jack && rank <= king) {
            return 10;
        }
        else if (rank == ace) {
            return 11; // Туз как 11
        }
        return rank;
    }
    // Возвращает значение ранга карты
    Rank getRank() const {
        return rank;
    }

    // Объявление дружественной функции для вывода объекта класса Card
    friend std::ostream& operator<<(std::ostream& os, const Card& card);

private:
    Rank rank; // Поле для хранения ранга карты
};

// Определение дружественной функции для вывода карты
std::ostream& operator<<(std::ostream& os, const Card& card) {
    // Массив строк для отображения рангов карт
    const char* ranks[]{ "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
    // Выводит строковое представление ранга карты в поток вывода
    os << ranks[card.rank - 2];
    return os;
}


// Определение интерфейса IFormattable
class IFormattable {
public:
    virtual ~IFormattable() = default;
    virtual std::string format() const = 0;
};

// Функция prettyPrint
void prettyPrint(const IFormattable& object) {
    std::cout << object.format() << std::endl;
}

// Определение класса Deck (КОЛОДА)
class Deck {
public:
    Deck() {
        for (int rank = Card::two; rank <= Card::ace; ++rank) {
            cards.emplace_back(static_cast<Card::Rank>(rank));
        }
        std::shuffle(cards.begin(), cards.end(), std::default_random_engine(std::time(0)));
    }

    const std::vector<Card>& getCards() const {
        return cards;
    }

private:
    std::vector<Card> cards;
};

// Адаптер объекта для Deck
class DeckAdapter : public IFormattable {
public:
    DeckAdapter(const Deck& deck) : deck(deck) {}

    std::string format() const override {
        std::ostringstream formattedDeck;
        for (const auto& card : deck.getCards()) {
            formattedDeck << "[" << card << "]";
        }
        return formattedDeck.str();
    }

private:
    const Deck& deck;
};

// Адаптер класса для Deck
class FormattableDeck : public Deck, public IFormattable {
public:
    std::string format() const override {
        std::ostringstream formattedDeck;
        for (const auto& card : getCards()) {
            formattedDeck << "[" << card << "]";
        }
        return formattedDeck.str();
    }
};

// Основная программа
int main() {
    std::setlocale(LC_ALL, "Russian");

    Deck deck;
    DeckAdapter deckAdapter(deck);

    std::cout << "Адаптер объекта:\n";
    prettyPrint(deckAdapter);

    FormattableDeck formattableDeck;

    std::cout << "\nАдаптер класса:\n";
    prettyPrint(formattableDeck);

    return 0;
}