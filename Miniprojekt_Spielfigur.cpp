#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>

const int FIELDS = 12;

struct Move {
  unsigned short from;
  unsigned short to;
  std::string info;
  Move(unsigned short _from, unsigned short _to, const std::string &_info)
      : from(_from), to(_to), info{_info} {}
};

class Playground {
  char playfields[FIELDS]; // Spielfelder
  unsigned short enemies;  // Anzahl gegnerischer Spielsteine
  unsigned short recentPlayerfield = 0;
  bool passed =
      false; // Variable wird true gesetzt, sobald eine Seite nicht ziehen kann.
  std::default_random_engine rand; // Zufallszahlengenerator
  bool game_over = false;

  void setEnemy() {
    unsigned short setField = 0;
    do {
      setField = (rand() % (FIELDS - 2)) +
                 1; // Feld 0 und das letzte Feld sollen soll frei bleiben
    } while (playfields[setField] != '_');
    playfields[setField] =
        enemyChar; // Feld wird mit seiner Feldnummer beschrieben. Das bedeutet,
                   // dass das Feld nun besetzt ist
  }

  void init() {
    for (int i = 0; i < FIELDS; i++) {
      playfields[i] = '_';
    }
  }

  void distributeEnemies() {
    for (int i = 0; i < enemies; i++) {
      setEnemy();
    }
  }

public:
  Playground(unsigned short _enemies = 3, char pChar = 'P', char eChar = 'E')
      : enemies(_enemies), playerChar(pChar), enemyChar(eChar) {
    rand.seed(
        std::chrono::high_resolution_clock{}.now().time_since_epoch().count());
    init();
    distributeEnemies();
    playfields[0] = playerChar;
  }

  const char playerChar;
  const char enemyChar;

  void changeEnemyField(unsigned short from, unsigned short to) {
    playfields[from] = '_';     // "Altes" Feld freigeben
    playfields[to] = enemyChar; // "Neues" Feld belegen
  }

  // Gibt den Inhalt des Feldes zurück.
  char operator[](unsigned short field) const {
    return playfields[field];
  } // ersetzt den Getter
  char &operator[](unsigned short field) { return playfields[field]; }

  void setPlayerField(unsigned short field) {
    playfields[recentPlayerfield] =
        '_'; // Bisher besetztes Feld des Spielers wird freigegeben
    playfields[field] = playerChar; // Spieler wird auf neues Feld gesetzt
    recentPlayerfield = field;      // Membervariable wird aktualisiert
    if (recentPlayerfield == FIELDS - 1) {
      game_over = true;
    }
  }

  unsigned short getPlayerfield() const { return recentPlayerfield; }

  bool gameOver() const { return game_over; }

  std::string toString() const {
    std::ostringstream oss;
    for (int i = 0; i < FIELDS; i++) {
      oss << ' ' << playfields[i] << ' ';
    }
    oss << '\n';
    return oss.str();
  }

  /// @brief Diese Methode liefert ein zufälliges Feld, das vom Gegner belegt
  /// ist UND in der Lage ist zu ziehen...
  Move getEnemyField() {
    std::vector<Move> legalMoves;

    for (int i = 1; i < FIELDS; i++) {
      if (playfields[i] == enemyChar) // Gegnerfigur gefunden
      {
        if (playfields[i - 1] == '_') // wenn Folgefeld frei
        {
          legalMoves.emplace_back(
              i, i - 1,
              "MOVE " + std::to_string(i) + " to " +
                  std::to_string(i -
                                 1)); // Nimm diesen Gegner in "legalFields" auf
        } else if (playfields[i - 1] ==
                   playerChar) // Wenn Folgefeld mit Spielerfigur besetzt...
        {
          if (i >= 2 && playfields[i - 2] ==
                            '_') // Prüfe, ob ein Sprung möglich ist (also zweik
                                 // Felder weiter frei ist und wir uns noch auf
                                 // dem Spielfeld befinden
          {
            legalMoves.emplace_back(
                i, i - 2,
                "JUMP " + std::to_string(i) + " to " +
                    std::to_string(
                        i - 2)); // Wenn dem so ist, in legalFields aufnehmen
          }
        }
      }
    }

    if (legalMoves.empty()) {
      return Move{0, 0, "PASS"};
    }

    std::shuffle(legalMoves.begin(), legalMoves.end(), rand);

    return legalMoves.back();
  }

  void move(unsigned short playernumber) {
    if (playernumber == 0) // Spieler ist dran
    {
      if (playfields[recentPlayerfield + 1] ==
          enemyChar) // Wenn nächstes Feld von Gegner besetzt
      {
        if (playfields[recentPlayerfield + 2] ==
            '_') // Und dahinter frei ist...
        {
          setPlayerField(recentPlayerfield + 2);
        }
        // Zweite if-Bedingung nicht zutrifft, darf der Spieler kein Feld bewegt
        // werden
        else {
          if (passed) {
            std::cout << "Game over! No moves possible anymore.";
            game_over = true;
            return;
          } else {
            passed = true;
          }
        }
      } else // Andernfalls ist das Folgefeld frei und wird besetzt
      {
        setPlayerField(recentPlayerfield + 1);
      }
    }

    else // Computer ist dran
    {
      Move randomMove = getEnemyField();
      std::cout << randomMove.info << "\n";
      if (randomMove.to == randomMove.from) // Wenn kein Zug möglich ist
      {
        if (passed) {
          std::cout << "Game Over! No moves possible anymore!\n";
          game_over = true;
          return;
        }
        passed = true;
      } else {
        changeEnemyField(randomMove.from, randomMove.to);
        passed = false;
      }
    }
  }
};

class Game {
  Playground &playground;

public:
  Game(Playground &p) : playground(p) {}

  void start() {
    unsigned short playernumber = 1;

    int count = 0; // Zählt die Anzahl der Züge
    while (!playground.gameOver()) {
      std::cout << playground.toString();
      playground.move(playernumber = 1 - playernumber);
      std::cout << playground.toString();
      std::cout << '\n';
      count++;
    }
    std::cout << "count: " << count << '\n';
  }
};

int main() {
  Playground playground{3};
  Game game{playground};
  game.start();

  return 0;
}