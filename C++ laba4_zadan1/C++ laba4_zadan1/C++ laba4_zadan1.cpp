#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <clocale>

// Класс Card (КАРТА)
class Card {
public:
    enum Suit { clubs, diamonds, hearts, spades };
    enum Rank { two = 2, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace };

    Card(Suit suit, Rank rank) : suit(suit), rank(rank) {}

    int getValue() const {
        if (rank >= jack && rank <= king) {
            return 10;
        }
        else if (rank == ace) {
            return 11;
        }
        return rank;
    }

    Rank getRank() const {
        return rank;
    }

    friend std::ostream& operator<<(std::ostream& os, const Card& card);

private:
    Suit suit;
    Rank rank;
};

std::ostream& operator<<(std::ostream& os, const Card& card) {
    const char* suits[]{ "♣", "♦", "♥", "♠" };
    const char* ranks[]{ "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
    os << ranks[card.rank - 2] << suits[card.suit];
    return os;
}

// Класс Deck (КОЛОДА)
class Deck {
public:
    Deck() {
        for (int suit = Card::clubs; suit <= Card::spades; ++suit) {
            for (int rank = Card::two; rank <= Card::ace; ++rank) {
                cards.emplace_back(static_cast<Card::Suit>(suit), static_cast<Card::Rank>(rank));
            }
        }
        std::shuffle(cards.begin(), cards.end(), std::default_random_engine(std::time(0)));
    }

    Card dealCard() {
        if (cards.empty()) {
            throw std::runtime_error("Колода пуста!");
        }
        Card dealtCard = cards.back();
        cards.pop_back();
        return dealtCard;
    }

private:
    std::vector<Card> cards;
};

// Класс Hand (Рука для игрока и дилера)
class Hand {
public:
    void addCard(const Card& card) {
        cards.push_back(card);
    }

    int getTotal() const {
        int total = 0;
        int numAces = 0;

        for (const auto& card : cards) {
            total += card.getValue();
            if (card.getValue() == 11) {
                ++numAces;
            }
        }

        while (total > 21 && numAces > 0) {
            total -= 10;
            --numAces;
        }

        return total;
    }

    void clear() {
        cards.clear();
    }

    bool canSplit() const {
        return cards.size() == 2 && cards[0].getRank() == cards[1].getRank();
    }

    Card split() {
        if (!canSplit()) {
            throw std::runtime_error("Нельзя выполнить сплит.");
        }
        Card card = cards.back();
        cards.pop_back();
        return card;
    }

    friend std::ostream& operator<<(std::ostream& os, const Hand& hand);

private:
    std::vector<Card> cards;
};

std::ostream& operator<<(std::ostream& os, const Hand& hand) {
    for (const auto& card : hand.cards) {
        os << card << " ";
    }
    return os;
}

// Класс Gamer (ИГРОК)
class Gamer {
public:
    void addCard(const Card& card) {
        hand.addCard(card);
    }

    int getTotal() const {
        return hand.getTotal();
    }

    void clearHand() {
        hand.clear();
    }

    bool canSplit() const {
        return hand.canSplit();
    }

    Card split() {
        return hand.split();
    }

    void playHand(Deck& deck) {
        while (true) {
            std::cout << "1. Хватит\n2. Еще\n";
            int choice;
            std::cin >> choice;

            if (choice == 1) {
                break;
            }
            else if (choice == 2) {
                addCard(deck.dealCard());
                std::cout << "Вы: " << *this << std::endl;
                std::cout << "Сумма: " << getTotal() << std::endl;

                if (getTotal() > 21) {
                    throw std::runtime_error("Перебор!");
                }
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Gamer& player);

private:
    Hand hand;
};

std::ostream& operator<<(std::ostream& os, const Gamer& player) {
    os << player.hand;
    return os;
}

// Класс Dealer (ДИЛЕР)
class Dealer : public Gamer {
public:
    void play(Deck& deck) {
        while (getTotal() < 17) {
            addCard(deck.dealCard());
        }
    }
};

// Класс Game (ИГРА)
class Game {
public:
    Game() {
        decks.resize(4); // Создаем 4 колоды
    }

    void play() {
        try {
            while (true) {
                // Установка локали
                std::setlocale(LC_ALL, "Russian");

                // Выбор типа игры
                std::cout << "Выберите тип игры:\n1. Блекджек\n2. Сплит\n";
                int gameChoice;
                std::cin >> gameChoice;

                // Ставка
                std::cout << "Ваша ставка? ";
                int bet;
                std::cin >> bet;

                // Раздача карт
                player.clearHand();
                dealer.clearHand();

                for (int i = 0; i < 2; ++i) {
                    player.addCard(decks[rand() % 4].dealCard());
                    dealer.addCard(decks[rand() % 4].dealCard());
                }

                // Показ карт
                std::cout << "Колоды: [52] [51] [50] [51]\n";
                std::cout << "Дилер: " << dealer << " ??" << std::endl;
                std::cout << "Сумма: " << dealer.getTotal() << " (скрыта)\n";
                std::cout << "Вы: " << player << std::endl;
                std::cout << "Сумма: " << player.getTotal() << std::endl;

                // Проверка на возможность сплита
                if (gameChoice == 2 && player.canSplit()) {
                    std::cout << "Вы можете выполнить сплит. Хотите?\n1. Да\n2. Нет\n";
                    int splitChoice;
                    std::cin >> splitChoice;

                    if (splitChoice == 1) {
                        // Выполнение сплита
                        splitPlayer.clearHand();
                        splitPlayer.addCard(player.split());
                        splitPlayer.addCard(decks[rand() % 4].dealCard());
                        player.addCard(decks[rand() % 4].dealCard());

                        std::cout << "Первая рука: " << player << std::endl;
                        std::cout << "Сумма: " << player.getTotal() << std::endl;
                        std::cout << "Вторая рука: " << splitPlayer << std::endl;
                        std::cout << "Сумма: " << splitPlayer.getTotal() << std::endl;

                        // Игрок ходит с двумя руками
                        std::cout << "Ход первой руки.\n";
                        player.playHand(decks[rand() % 4]);

                        std::cout << "Ход второй руки.\n";
                        splitPlayer.playHand(decks[rand() % 4]);
                    }
                    else {
                        player.playHand(decks[rand() % 4]);
                    }
                }
                else {
                    player.playHand(decks[rand() % 4]);
                }

                // Дилер ходит
                dealer.play(decks[rand() % 4]);

                // Результат
                std::cout << "Колоды: [52] [51] [50] [51]\n";
                std::cout << "Дилер: " << dealer << std::endl;
                std::cout << "Сумма: " << dealer.getTotal() << std::endl;

                // Проверка результата для каждой руки игрока
                checkResult(player, bet);
                if (gameChoice == 2 && player.canSplit()) {
                    checkResult(splitPlayer, bet);
                }
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "Исключение: " << ex.what() << std::endl;
        }
    }

private:
    std::vector<Deck> decks;
    Gamer player;
    Dealer dealer;
    Gamer splitPlayer;  // Добавляем второго игрока для сплита

    void checkResult(const Gamer& player, int bet) {
        if (player.getTotal() > 21) {
            std::cout << "Вы проиграли! Перебор." << std::endl;
        }
        else if (dealer.getTotal() > 21 || player.getTotal() > dealer.getTotal()) {
            std::cout << "Поздравляем! Вы выиграли! Ваш выигрыш: " << bet << ". Всего: " << bet * 2 << "." << std::endl;
        }
        else if (player.getTotal() < dealer.getTotal()) {
            std::cout << "Вы проиграли! Ваш выигрыш: 0." << std::endl;
        }
        else {
            std::cout << "Ровно. Ваш выигрыш: " << bet << "." << std::endl;
        }
    }
};

int main() {
    Game game;
    game.play();
    return 0;
}
