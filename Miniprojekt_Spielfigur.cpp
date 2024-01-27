#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>

const int FIELDS = 12;

enum class Playfield { START = 0, DESTINATION = FIELDS - 1 };

class Playground {
  char playfields[FIELDS]; // Spielfelder
  unsigned short enemies;  // Anzahl gegnerischer Spielsteine
  unsigned short recentPlayerfield = 0;

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
    init();
    srand(time(nullptr)); // Zufallsgeneraator anschmeißen
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
  char operator[](unsigned short field) const { return playfields[field]; } // ersetzt den Getter
  char& operator[](unsigned short field) // ersetzt den Setter
  {
    return playfields[field];
  }

  void setPlayerField(unsigned short field) {
    playfields[recentPlayerfield] =
        '_'; // Bisher besetztes Feld des Spielers wird freigegeben
    playfields[field] = playerChar; // Spieler wird auf neues Feld gesetzt
    recentPlayerfield = field;      // Membervariable wird aktualisiert
  }

  unsigned short getPlayerfield() const { return recentPlayerfield; }

  bool gamOver() const {
    if (playfields[FIELDS - 1] == playerChar) {
      return true;
    }
    return false;
  }

  std::string toString() const 
  {
    std::ostringstream oss;
    for (int i = 0; i < FIELDS; i++) 
    {
      oss << ' ' << playfields[i] << ' ';
    }
    oss << '\n';
    return oss.str();
  }

  /// @brief Diese Methode liefert ein zufälliges Feld, das vom Gegner belegt
  /// ist UND in der Lage ist zu ziehen...
  unsigned short getEnemyField() 
  {
    unsigned short index = 0;
    while (true) 
    {
      index = (rand() % (FIELDS - 1)) + 1;
      if (playfields[index] == enemyChar && playfields[index - 1] != enemyChar) {
        if (playfields[index - 1] == playerChar && playfields[index - 2] == enemyChar) 
        {
          continue;
        }
        return index;
      }
    }
  }


  void move(unsigned short playernumber)
  {
    if(playernumber == 0) // Spieler ist dran
    {
      if (playfields[recentPlayerfield + 1] == enemyChar) // Wenn nächstes Feld von Gegner besetzt
      {
        if (playfields[recentPlayerfield + 2] == '_') // Und dahinter frei ist...
        {
          setPlayerField(recentPlayerfield + 2);
        }
        // Zweite if-Bedingung nicht zutrifft, darf der Spieler kein Feld bewegt
        // werden
      } else // Andernfalls ist das Folgefeld frei und wird besetzt
      {
        setPlayerField(recentPlayerfield + 1);
      }
    }

    else // Computer ist dran
    {
      unsigned short randomEnemyFigure = getEnemyField();

      if (playfields[randomEnemyFigure - 1] == '_') // Wenn das nächste Feld frei ist
      {
        changeEnemyField(randomEnemyFigure, randomEnemyFigure - 1);
      } else if (playfields[randomEnemyFigure - 1] == playerChar) // wenn nächstes Feld vom Spieler
                                        // besetzt ist...
      {
        changeEnemyField(randomEnemyFigure, randomEnemyFigure - 2);
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
    while (!playground.gamOver()) 
    {
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