#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>

const int FIELDS = 12;

enum class Playfield
{
    START = 0,
    DESTINATION = FIELDS - 1
};


class Playground
{
    char playfields[FIELDS];  // Spielfelder
    unsigned short enemies;         // Anzahl gegnerischer Spielsteine
    char playerChar;
    char enemyChar;
    unsigned short recentPlayerfield = 0;

    void setEnemy()
    {
        unsigned short setField = 0;
        do
        {
            setField = (rand() % (FIELDS-2)) + 1; // Feld 0 und das letzte Feld sollen soll frei bleiben
        }while(playfields[setField] != '_');
        playfields[setField] = enemyChar;    // Feld wird mit seiner Feldnummer beschrieben. Das bedeutet, dass das Feld nun besetzt ist
    }

    void init()
    {
        for(int i = 0; i < FIELDS; i++)
        {
            playfields[i] = '_';
        }
    }

    void distributeEnemies()
    {
        for(int i = 0; i < enemies; i++)
        {
            setEnemy();
        }
    }

    public:
    Playground(unsigned short _enemies = 3, char pChar = 'P', char eChar = 'E') : enemies(_enemies), playerChar(pChar), enemyChar(eChar)
    {
        init();
        srand(time(nullptr));   // Zufallsgeneraator anschmeißen
        distributeEnemies();
        playfields[0] = playerChar; 
    }

    void changeEnemyField(unsigned short from, unsigned short to)
    {
        playfields[from] = '_'; // "Altes" Feld freigeben
        playfields[to] = enemyChar; // "Neues" Feld belegen
    }

    // Gibt den Inhalt des Feldes zurück. 
    unsigned short getField(unsigned int field)const
    {       
        return playfields[field];
    }

    void setPlayerField(unsigned short field)
    {
        playfields[recentPlayerfield] = '_'; // Bisher besetztes Feld des Spielers wird freigegeben
        playfields[field] = playerChar; // Spieler wird auf neues Feld gesetzt
        recentPlayerfield = field; // Membervariable wird aktualisiert
    }

    unsigned short getPlayerfield()const{return recentPlayerfield;}

    bool gamOver()const
    {
        if(playfields[FIELDS-1] == playerChar)
        {
            return true;
        }
        return false;
    }

    void printField()const
    {
        for(int i = 0; i < FIELDS; i++)
        {
            std::cout << ' ' << playfields[i] << ' ';
        }
        std::cout << '\n';
    }

friend class Game;
};

class Game
{
    Playground& playground;


    /// @brief Diese Methode liefert ein zufälliges Feld, das vom Gegner belegt ist UND in der Lage ist zu ziehen...  
    unsigned short getEnemyField()
    {
        unsigned short index = 0;
        while(true)
        {
            index = (rand() % (FIELDS - 1)) + 1;
            if(playground.getField(index) == playground.enemyChar && playground.getField(index - 1) != playground.enemyChar)
            {
                if(playground.getField(index-1) == playground.playerChar && playground.getField(index-2) == playground.enemyChar)
                {
                    continue;
                }
                return index;
            }
        }
    }

    void move(unsigned short playernumber)
    {   
        if(playernumber == 0)   // Spieler ist dran
        {
            unsigned short recentField = playground.getPlayerfield();
            if(playground.getField(recentField + 1) == playground.enemyChar)   // Wenn nächstes Feld von Gegner besetzt
            {
                if(playground.getField(recentField + 2) == '_')   // Und dahinter frei ist...
                {
                    playground.setPlayerField(recentField + 2);
                }
                // Zweite if-Bedingung nicht zutrifft, darf der Spieler kein Feld bewegt werden
            }
            else    // Andernfalls ist das Folgefeld frei und wird besetzt
            {
                playground.setPlayerField(recentField + 1);
            }
        }

        else    // Compuer ist dran
        {
            unsigned short randomEnemyFigure = getEnemyField();

            if(playground.getField(randomEnemyFigure - 1) == '_')   // Wenn das nächste Feld frei ist
            {
                playground.changeEnemyField(randomEnemyFigure, randomEnemyFigure-1);                
            }
            else if(playground.getField(randomEnemyFigure - 1) == playground.playerChar) // wenn nächstes Feld vom Spieler besetzt ist... 
            {
                playground.changeEnemyField(randomEnemyFigure, randomEnemyFigure-2);
            }
        }
    }

    public:
    Game(Playground& p) : playground(p){}

    void start()
    {
        unsigned short playernumber = 1;

        int count = 0;
        while(!playground.gamOver())
        {
            playground.printField();
            move(playernumber = 1 - playernumber);
            playground.printField();
            std::cout << '\n';
            count++;
            getchar();
        }
        std::cout << "count: " << count << '\n';
    }
};


int main()
{
    Playground playground{3};
    Game game{playground};
    game.start();

    return 0;
}