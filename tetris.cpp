#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <ostream>
#include <time.h>

// Tetromino consists of 4 blocks
class Block
{
public:
    int x, y;
    Block() : x(0), y(0) {}
    Block(int x, int y) : x(x), y(y) {}
};

// Flying blocks used for clear-lines visual effect
class FxBlock
{
public:
    int x, y, colorId;
    float startVelocity, startAngle;
    float vx0, vy0;
    float timer;

    FxBlock(int x, int y, int colorId) : x(x), y(y), colorId(colorId)
    {
        timer = 0;
        startVelocity = rand() % 50 + 40;
        startAngle = rand() % 180 + 1;
        vx0 = startVelocity * cos(startAngle * (3.14159265 / 180));
        vy0 = startVelocity * sin(startAngle * (3.14159265 / 180));
    }

    void update(float elapsedTime)
    {
        timer += elapsedTime;
        float g = 9.81;

        x += vx0 * timer;
        y -= vy0 * timer - 0.5 * g * timer * timer;

        vy0 -= g * timer;
    }
};

// Color palette with easy-to-remember enums
class Colors
{
public:
    enum ColorId
    {
        Black,
        Silver,
        Grey,
        Navy,
        Cyan,
        Magenta,
        Red,
        Green,
        Blue,
        TRed,
        TViolet,
        TOrange,
        TYellow,
        TGreen,
        TBlue,
        TBlueDark
    };

    static sf::Color getColor(int colorId, int alpha = 255)
    {
        switch (colorId)
        {
        case Red:
            return sf::Color(255, 0, 0, alpha);
        case Green:
            return sf::Color(0, 255, 0, alpha);
        case Blue:
            return sf::Color(0, 0, 255, alpha);
        case Black:
            return sf::Color(0, 0, 0, alpha);
        case Silver:
            return sf::Color(192, 192, 192, alpha);
        case Grey:
            return sf::Color(128, 128, 128, alpha);
        case Navy:
            return sf::Color(0, 0, 128, alpha);
        case Cyan:
            return sf::Color(0, 255, 255, alpha);
        case TRed:
            return sf::Color(220, 20, 60, alpha);
        case TViolet:
            return sf::Color(186, 85, 211, alpha);
        case TOrange:
            return sf::Color(255, 165, 0, alpha);
        case TYellow:
            return sf::Color(255, 215, 0, alpha);
        case TGreen:
            return sf::Color(173, 255, 47, alpha);
        case TBlue:
            return sf::Color(135, 206, 250, alpha);
        case TBlueDark:
            return sf::Color(30, 144, 255, alpha);
        }
        return sf::Color(255, 0, 255, alpha); // Magenta
    }
};

// Tetromino entity that stores the shape, current and previous position
class Tetromino
{
public:
    enum ShapeId
    {
        Shape_O = 1,
        Shape_S = 2,
        Shape_Z = 3,
        Shape_I = 4,
        Shape_L = 5,
        Shape_J = 6,
        Shape_T = 7,
    };

    Block blocksCurrent[4];
    Block blocksPrevious[4];

    int rotationIndex;
    int shapeId;
    int colorId;
    int currentHardDropMaxDistance;

    void restorePreviousPosition()
    {
        for (int i = 0; i < 4; i++)
        {
            blocksCurrent[i] = blocksPrevious[i];
        }
    }

    void dropDown()
    {
        for (int i = 0; i < 4; i++)
        {
            blocksPrevious[i] = blocksCurrent[i]; // backup
            blocksCurrent[i].y++;
        }
    }

    void moveX(int dx)
    {
        for (int i = 0; i < 4; i++)
        {
            blocksPrevious[i] = blocksCurrent[i]; // backup
            blocksCurrent[i].x += dx;
        }
    }

    void moveUp(int y)
    {
        for (int i = 0; i < 4; i++)
        {
            blocksCurrent[i].y = blocksCurrent[i].y - y;
        }
    }

    void rotate()
    {
        if (rotationIndex >= 0)
        {
            for (int i = 0; i < 4; i++)
            {
                blocksPrevious[i] = blocksCurrent[i]; // backup
            }
            Block p = blocksCurrent[rotationIndex];
            for (int i = 0; i < 4; i++)
            {
                int x = blocksCurrent[i].y - p.y;
                int y = blocksCurrent[i].x - p.x;
                blocksCurrent[i].x = p.x - x;
                blocksCurrent[i].y = p.y + y;
            }
        }
    }
};

// Generates tetrominos with random shapes and colors
class Generator
{
public:
    static Tetromino getTetromino(int lastColorId = -1)
    {
        int shape_1[4][2] = {
            {1, 1},
            {1, 1},
            {0, 0},
            {0, 0}};

        int shape_2[4][2] = {
            {1, 0},
            {1, 2},
            {0, 1},
            {0, 0}};

        int shape_3[4][2] = {
            {0, 1},
            {2, 1},
            {1, 0},
            {0, 0}};

        int shape_4[4][2] = {
            {1, 0},
            {1, 0},
            {2, 0},
            {1, 0}};

        int shape_5[4][2] = {
            {1, 0},
            {2, 0},
            {1, 1},
            {0, 0}};

        int shape_6[4][2] = {
            {0, 1},
            {0, 2},
            {1, 1},
            {0, 0}};

        int shape_7[4][2] = {
            {1, 0},
            {2, 1},
            {1, 0},
            {0, 0}};

        Tetromino tetromino;
        tetromino.rotationIndex = -1;
        tetromino.shapeId = (rand() % 7) + 1;
        tetromino.currentHardDropMaxDistance = -1;

        do
        {
            tetromino.colorId = (rand() % 7) + 9;
        } while (tetromino.colorId == lastColorId);

        for (int i = 0; i < 4; i++)
        {
            tetromino.blocksPrevious[i].x = 0;
            tetromino.blocksPrevious[i].y = 0;
            tetromino.blocksCurrent[i].x = 0;
            tetromino.blocksCurrent[i].y = 0;
        }

        switch (tetromino.shapeId)
        {
        case Tetromino::Shape_O:
            configureBlocks(tetromino, shape_1);
            break;

        case Tetromino::Shape_S:
            configureBlocks(tetromino, shape_2);
            break;

        case Tetromino::Shape_Z:
            configureBlocks(tetromino, shape_3);
            break;

        case Tetromino::Shape_I:
            configureBlocks(tetromino, shape_4);
            break;

        case Tetromino::Shape_L:
            configureBlocks(tetromino, shape_5);
            break;

        case Tetromino::Shape_J:
            configureBlocks(tetromino, shape_6);
            break;

        case Tetromino::Shape_T:
            configureBlocks(tetromino, shape_7);
            break;
        }

        return tetromino;
    }

    static void configureBlocks(Tetromino &tetromino, int shape[4][2])
    {
        int blocksDoneNum = 0;
        tetromino.rotationIndex = -1;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (blocksDoneNum < 4 && shape[i][j] > 0)
                {
                    if (shape[i][j] == 2)
                    {
                        tetromino.rotationIndex = blocksDoneNum;
                    }
                    tetromino.blocksCurrent[blocksDoneNum].x = j;
                    tetromino.blocksCurrent[blocksDoneNum].y = i;
                    blocksDoneNum++;
                    if (blocksDoneNum == 4)
                        break;
                }
                if (blocksDoneNum == 4)
                    break;
            }
        }
    }
};

// Represents the game board
class Grid
{
public:
    static const int rows = 20;
    static const int cols = 10;
    int grid[rows][cols];

    Grid()
    {
        clear();
    }

    void clear()
    {
        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < cols; ++x)
            {
                grid[y][x] = 0;
            }
        }
    }

    int getValue(int x, int y)
    {
        if (y < rows and x < cols)
        {
            return grid[y][x];
        }
        return -1;
    }
};

// Identifies the user's intended action
class Input
{
public:
    int moveX;
    bool rotate;
    bool spacebar;
    bool fastDrop;
    bool paused;
    bool shadowSwitch;

    Input() : moveX(0),
              rotate(0),
              spacebar(0),
              fastDrop(0),
              paused(0),
              shadowSwitch(0) {}
};

// Manages game states and holds current score
class GameState
{
public:
    int currentState;
    int difficultyLevel;
    int difficultyLevelStep;
    int shadowEnabled;
    Grid *grid;
    Tetromino *tetromino;
    Tetromino *nextTetromino;
    const char *highscoreFilename;

    GameState(Grid *gridPtr, Tetromino *tetrominoPtr, Tetromino *nextTetrominoPtr) : grid(gridPtr), tetromino(tetrominoPtr), nextTetromino(nextTetrominoPtr)
    {
        difficultyLevel = 1;
        difficultyLevelStep = 5;
        shadowEnabled = 1;
        highscoreFilename = "score.txt";
    }

    enum StateId
    {
        Title,
        Playing,
        Pause,
        GameOver
    };

    int currentScore;
    int highestScore;

    void loadHighScore()
    {
        std::ifstream inputFile(highscoreFilename);
        inputFile >> highestScore;
        inputFile.close();
    }

    void saveHighScore()
    {
        std::ofstream outputFile(highscoreFilename);
        outputFile << highestScore;
        outputFile.close();
    }

    void update(Input &input)
    {
        switch (currentState)
        {
        case Title:

            if (input.spacebar)
            {
                loadHighScore();
                currentState = Playing;
                input.spacebar = 0;
                generateNewTetromino();
            }

            break;

        case Playing:

            if (currentScore > highestScore)
            {
                highestScore = currentScore;
                saveHighScore(); // TODO render congratulations
            }

            if (input.paused)
            {
                currentState = Pause;
                input.paused = 0;
            }

            if (input.shadowSwitch)
            {
                shadowEnabled *= -1;
                input.shadowSwitch = 0;
            }

            if (currentScore >= difficultyLevelStep)
            {
                difficultyLevel++;
                difficultyLevelStep += 5;
            }

            break;

        case Pause:

            if (input.paused)
            {
                currentState = Playing;
                input.paused = 0;
            }

            break;

        case GameOver:

            if (input.spacebar)
            {
                grid->clear();
                currentState = Playing;
                input.spacebar = 0;
                currentScore = 0;
                difficultyLevel = 1;
                difficultyLevelStep = 5;
                generateNewTetromino();
                loadHighScore();
            }

            break;

        default:
            break;
        }
    }

    void generateNewTetromino() // TODO fix duplicated Logic
    {
        int lastColorId = tetromino->colorId;
        *tetromino = *nextTetromino;

        switch (tetromino->shapeId)
        {
        case Tetromino::Shape_I:
            tetromino->moveUp(4);
            break;
        case Tetromino::Shape_O:
            tetromino->moveUp(2);
            break;
        default:
            tetromino->moveUp(3);
            break;
        }
        tetromino->moveX((rand() % (grid->cols - 2)) + 1);
        *nextTetromino = Generator::getTetromino(lastColorId);
    }
};

// Flying blocks container
class SpecialEffects
{
public:
    std::vector<FxBlock *> fxBlocks;

    SpecialEffects()
    {
        fxBlocks.resize(Grid::rows * Grid::cols, NULL);
    }

    void removeBlock(FxBlock *block)
    {
        for (std::vector<FxBlock *>::iterator it = fxBlocks.begin(); it != fxBlocks.end(); ++it)
        {
            if (*it == block)
            {
                delete *it;
                fxBlocks.erase(it);
                break;
            }
        }
    }

    void createFxBlock(int x, int y, int c)
    {
        FxBlock *newBlock = new FxBlock(x * 32, y * 32, c);
        fxBlocks.push_back(newBlock);
    }

    void removeFxBlocks()
    {
        for (std::vector<FxBlock *>::iterator it = fxBlocks.begin(); it != fxBlocks.end(); ++it)
        {
            if (*it and ((*it)->y > (Grid::rows * 32) or (*it)->x < 0 || (*it)->x > (Grid::cols * 32)))
            {
                removeBlock((*it));
                break;
            }
        }
    }

    void updateFxBlocks()
    {
        for (std::vector<FxBlock *>::iterator it = fxBlocks.begin(); it != fxBlocks.end(); ++it)
        {
            if (*it)
            {
                (*it)->update(0.01);
            }
        }
    }
};

// Rendering functions
class View
{
public:
    static const int tileSize = 32;
    sf::Font font;
    sf::RenderWindow *window;
    Grid *grid;
    Tetromino *tetromino;
    Tetromino *nextTetromino;
    GameState *state;
    SpecialEffects *specialEffects;

    View(sf::RenderWindow *windowPtr, Grid *gridPtr, Tetromino *tetrominoPtr, Tetromino *nextTetrominoPtr, GameState *statePtr, SpecialEffects *specialEffectsPtr)
        : window(windowPtr), grid(gridPtr), tetromino(tetrominoPtr), nextTetromino(nextTetrominoPtr), state(statePtr), specialEffects(specialEffectsPtr)
    {
        font.loadFromFile("retro.ttf");
    }

    static int getWindowWidth()
    {
        return tileSize * Grid::cols + 1 + (tileSize * 6);
    }

    static int getWindowHeight()
    {
        return tileSize * Grid::rows + 1;
    }

    void renderGameOver()
    {
        sf::Text text;
        text.setFont(font);
        text.setString("Game Over");
        text.setCharacterSize(28);
        text.setPosition(65, 250);
        window->draw(text);
    }

    void renderPause()
    {
        sf::Text text;
        text.setFont(font);
        text.setString("Paused");
        text.setCharacterSize(32);
        text.setPosition(90, 250);
        window->draw(text);
    }

    void renderTitleScreen()
    {
        sf::Text text;
        text.setFont(font);
        text.setString("Tetris");
        text.setCharacterSize(55);
        text.setPosition(135, 250);
        window->draw(text);
    }

    void renderTetromino()
    {
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(tileSize - 1, tileSize - 1));
        tile.setFillColor(Colors::getColor(tetromino->colorId));
        for (int i = 0; i < 4; i++)
        {
            tile.setPosition(tetromino->blocksCurrent[i].x * tileSize, tetromino->blocksCurrent[i].y * tileSize);
            tile.move(1, 1);
            window->draw(tile);
        }
    }

    void renderFlyingBlocks()
    {
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(tileSize - 1, tileSize - 1));
        for (std::vector<FxBlock *>::iterator it = specialEffects->fxBlocks.begin(); it != specialEffects->fxBlocks.end(); ++it)
        {
            if (*it)
            {
                tile.setFillColor(Colors::getColor((*it)->colorId, 200));
                tile.setPosition((*it)->x, (*it)->y);
                window->draw(tile);
            }
        }
    }

    void renderNextTetromino()
    {
        sf::Text text;
        text.setFont(font);
        text.setString("next");
        text.setCharacterSize(22);
        text.setPosition((12 * tileSize), 32);
        window->draw(text);

        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(tileSize - 1, tileSize - 1));
        tile.setFillColor(Colors::getColor(nextTetromino->colorId));
        for (int i = 0; i < 4; i++)
        {
            tile.setPosition(nextTetromino->blocksCurrent[i].x * tileSize + (12 * tileSize), nextTetromino->blocksCurrent[i].y * tileSize + (3 * tileSize));
            tile.move(1, 1);
            window->draw(tile);
        }
    }

    void renderCurrentTetrominoShadow()
    {
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(tileSize - 1, tileSize - 1));
        tile.setFillColor(Colors::getColor(Colors::Grey, 80));

        bool show = true;

        if (state->shadowEnabled != 1)
        {
            show = false;
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                if (tetromino->blocksCurrent[i].y < 0)
                {
                    show = false;
                    break;
                }
            }
        }

        if (show and tetromino->currentHardDropMaxDistance > 0)
        {
            for (int i = 0; i < 4; i++)
            {
                tile.setPosition(tetromino->blocksCurrent[i].x * tileSize, tetromino->blocksCurrent[i].y * tileSize);
                tile.move(1, tetromino->currentHardDropMaxDistance * tileSize + 1);
                window->draw(tile);
            }
        }
    }

    void renderScore()
    {
        sf::Text text;
        text.setFont(font);

        text.setString("score");
        text.setCharacterSize(22);
        text.setPosition(tileSize * 11 + 24, tileSize * 8 + 12);
        window->draw(text);

        text.setString(std::to_string(state->currentScore));
        text.setCharacterSize(28);
        text.setPosition(tileSize * 11 + 24, tileSize * 9 + 5);
        window->draw(text);

        text.setString("high");
        text.setCharacterSize(22);
        text.setPosition(tileSize * 11 + 24, tileSize * 10 + 12);
        window->draw(text);

        text.setString(std::to_string(state->highestScore));
        text.setCharacterSize(28);
        text.setPosition(tileSize * 11 + 24, tileSize * 11 + 5);
        window->draw(text);

        text.setString("level");
        text.setCharacterSize(20);
        text.setPosition(tileSize * 11 + 24, tileSize * 13);
        window->draw(text);

        text.setString(std::to_string(state->difficultyLevel));
        text.setCharacterSize(20);
        text.setPosition(tileSize * 11 + 24, tileSize * 14);
        window->draw(text);
    }

    void renderGrid()
    {
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(tileSize - 1, tileSize - 1));
        tile.setFillColor(Colors::getColor(Colors::Black));

        sf::RectangleShape background;
        background.setSize(sf::Vector2f(grid->cols * tileSize + 1, grid->rows * tileSize + 1));
        background.setFillColor(Colors::getColor(Colors::Blue));
        window->draw(background);

        int value;

        for (int i = 0; i < grid->rows; i++)
        {
            for (int j = 0; j < grid->cols; j++)
            {
                value = grid->getValue(j, i);

                if (value > 0)
                {
                    tile.setFillColor(Colors::getColor(value));
                }
                else
                {
                    tile.setFillColor(Colors::getColor(Colors::Black));
                }

                tile.setPosition(j * tileSize + 1, i * tileSize + 1);
                window->draw(tile);
            }
        }
    }

    void renderHelp()
    {
        int offsetY = 500;
        int offsetX = 345;
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(13);
        text.setString("up       - rotate");
        text.setPosition(offsetX, offsetY);
        window->draw(text);
        offsetY += 22;
        text.setString("down   - soft drop");
        text.setPosition(offsetX, offsetY);
        window->draw(text);
        offsetY += 22;
        text.setString("space - hard drop");
        text.setPosition(offsetX, offsetY);
        window->draw(text);
        offsetY += 32;
        text.setString("p - pause");
        text.setPosition(offsetX, offsetY);
        window->draw(text);
        offsetY += 22;
        text.setString("q - quit");
        text.setPosition(offsetX, offsetY);
        window->draw(text);
    }

    void render()
    {
        window->clear(sf::Color::Black);

        if (state->currentState == GameState::Title)
        {
            renderTitleScreen();
        }
        else
        {
            renderGrid();
            renderNextTetromino();
            renderScore();
            renderTetromino();
            renderCurrentTetrominoShadow();
            renderHelp();
            renderFlyingBlocks();

            if (state->currentState == GameState::Pause)
            {
                renderPause();
            }
            else if (state->currentState == GameState::GameOver)
            {
                renderGameOver();
            }
        }

        window->display();
    }
};

// Game logic goes here
class Logic
{
public:
    Grid *grid;
    Input *input;
    Tetromino *tetromino;
    GameState *state;
    Tetromino *nextTetromino;
    SpecialEffects *specialEffects;

    float dropTimer;
    float dropDelay;
    float scoreTimer;

    Logic(Grid *gridPtr, Input *inputPtr, Tetromino *tetrominoPtr, GameState *statePtr, Tetromino *nextTetrominoPtr, SpecialEffects *specialEffectsPtr) : grid(gridPtr), input(inputPtr), tetromino(tetrominoPtr), state(statePtr), nextTetromino(nextTetrominoPtr), specialEffects(specialEffectsPtr)
    {
        dropTimer = 0;
        dropDelay = 1;
        scoreTimer = 0;
    }

    void update(float time)
    {
        if (state->currentState == GameState::Playing)
        {
            // MOVE LEFT OR RIGHT
            if (input->moveX != 0)
            {
                tetromino->moveX(input->moveX);

                input->moveX = 0;

                if (!isCurrentPositionValid())
                {
                    tetromino->restorePreviousPosition();
                }

                tetromino->currentHardDropMaxDistance = getHardDropOffsetY();
            }

            // ROTATE
            if (input->rotate)
            {
                tetromino->rotate();

                input->rotate = 0;

                int wallKickDistanceX = getWallKickDistanceX();
                if (wallKickDistanceX != 0)
                {
                    tetromino->moveX(wallKickDistanceX);
                }

                if (!isCurrentPositionValid())
                {
                    tetromino->restorePreviousPosition();
                }

                tetromino->currentHardDropMaxDistance = getHardDropOffsetY();
            }

            // HARD DROP
            if (input->spacebar)
            {
                doHardDrop();

                input->spacebar = 0;

                if (!isPositionAfterNextDropValid())
                {
                    if (isAtTopRow())
                    {
                        state->currentState = GameState::GameOver;
                        return; // break the update
                    }
                    else
                    {
                        placeTetrominoHere();
                        state->currentScore += clearFullRows();
                        generateNewTetromino();
                    }
                }

                tetromino->currentHardDropMaxDistance = getHardDropOffsetY();
            }

            // FREE DROP AND SOFT DROP
            if (state->difficultyLevel == 2)
            {
                dropDelay = 0.8;
            }
            else if (state->difficultyLevel == 3)
            {
                dropDelay = 0.6;
            }
            else if (state->difficultyLevel == 4)
            {
                dropDelay = 0.4;
            }
            else if (state->difficultyLevel >= 5)
            {
                dropDelay = 0.2;
            }
            else
            {
                dropDelay = 1;
            }

            dropTimer += time;

            float dropTimerDelay = dropDelay;

            if (input->fastDrop)
            {
                dropTimerDelay = 0.02;
            }

            if (dropTimer > dropTimerDelay)
            {
                tetromino->dropDown();
                dropTimer = 0;

                if (!isCurrentPositionValid())
                {
                    tetromino->restorePreviousPosition();
                }

                if (!isPositionAfterNextDropValid())
                {
                    if (isAtTopRow())
                    {
                        state->currentState = GameState::GameOver;
                        return; // break the update loop
                    }
                    else
                    {
                        placeTetrominoHere();
                        state->currentScore += clearFullRows();
                        generateNewTetromino();
                    }
                }

                tetromino->currentHardDropMaxDistance = getHardDropOffsetY();
            }

            // SPECIAL EFFECTS
            scoreTimer += time;
            if (scoreTimer > 0.01)
            {
                specialEffects->updateFxBlocks();
                scoreTimer = 0;
            }

            specialEffects->removeFxBlocks();
        }
    }

    void doHardDrop()
    {
        int dropOffset = 0;
        dropOffset = getHardDropOffsetY();

        if (dropOffset)
        {
            for (int j = 1; j <= dropOffset + 1; j++)
            {
                for (int i = 0; i < 4; i++)
                {
                    tetromino->blocksPrevious[i] = tetromino->blocksCurrent[i];
                    tetromino->blocksCurrent[i].y = tetromino->blocksCurrent[i].y + 1;
                }

                if (!isCurrentPositionValid())
                {
                    tetromino->restorePreviousPosition();
                }
            }
        }
    }

    bool isTetrominoInViewPort()
    {
        for (int i = 0; i < 4; i++)
        {
            if (tetromino->blocksCurrent[i].y < 0)
            {
                return 0;
            }
        }
        return 1;
    }

    int getLowestYoffset()
    {
        int lowestY = -10;
        for (int i = 0; i < 4; i++)
        {
            if (tetromino->blocksCurrent[i].y > lowestY)
            {
                lowestY = tetromino->blocksCurrent[i].y;
            }
        }
        return lowestY;
    }

    int getOffsetBlockedByGrid(int dropDist)
    {
        int offsetY = 0;
        int maxOffset = 0;

        while (offsetY < dropDist)
        {
            offsetY++;
            for (int i = 0; i < 4; i++)
            {
                if (maxOffset == 0 and tetromino->blocksCurrent[i].y + offsetY >= (grid->rows - 1))
                {
                    if (offsetY > maxOffset)
                    {
                        maxOffset = offsetY;
                    }
                }
            }
        }

        return maxOffset;
    }

    int getOffsetBlockedByMino(int dropDist)
    {
        int offsetY = 0;
        int maxOffset = 20;

        while (offsetY < dropDist)
        {
            offsetY++;
            for (int i = 0; i < 4; i++)
            {
                if (grid->getValue(tetromino->blocksCurrent[i].x, tetromino->blocksCurrent[i].y + offsetY))
                {
                    if (offsetY < maxOffset)
                    {
                        maxOffset = offsetY;
                    }
                }
            }
        }

        if (maxOffset != 20)
        {
            maxOffset--;
        }

        return maxOffset;
    }

    int getHardDropOffsetY()
    {
        if (!isTetrominoInViewPort())
        {
            return 0;
        }

        int lowestY = getLowestYoffset();
        int maxDropDist = grid->rows - lowestY - 1;

        if (maxDropDist > 0)
        {
            int maxOffsetByMino = getOffsetBlockedByMino(maxDropDist);
            int maxOffsetByGrid = getOffsetBlockedByGrid(maxDropDist);

            if (maxOffsetByMino < maxOffsetByGrid)
            {
                return maxOffsetByMino;
            }
            else if (maxOffsetByGrid)
            {
                return maxOffsetByGrid;
            }
        }

        return 0;
    }

    void placeTetrominoHere()
    {
        for (int i = 0; i < 4; i++)
        {
            grid->grid[tetromino->blocksCurrent[i].y][tetromino->blocksCurrent[i].x] = tetromino->colorId;
        }
    }

    bool isAtTopRow()
    {
        for (int i = 0; i < 4; i++)
        {
            if (tetromino->blocksCurrent[i].y == 0)
            {
                return 1;
            }
        }
        return 0;
    }

    int getWallKickDistanceX()
    {
        int dist = 0;
        for (int i = 0; i < 4; i++)
        {
            if (tetromino->blocksCurrent[i].x < 0)
            {
                if (0 - tetromino->blocksCurrent[i].x > dist)
                {
                    dist = 0 - tetromino->blocksCurrent[i].x;
                }
            }
            else if (tetromino->blocksCurrent[i].x > (grid->cols - 1))
            {
                if ((grid->cols - 1) - tetromino->blocksCurrent[i].x < dist)
                {
                    dist = (grid->cols - 1) - tetromino->blocksCurrent[i].x;
                }
            }
            else
            {
                // TODO wallkick from other blocks on grid
            }
        }
        return dist;
    }

    bool isRowFull(int r)
    {
        for (int c = 0; c < grid->cols; c++)
        {
            if (grid->grid[r][c] == 0)
            {
                return 0;
            }
        }
        return 1;
    }

    void moveRowDown(int r, int num)
    {
        for (int c = 0; c < grid->cols; c++)
        {
            grid->grid[r + num][c] = grid->grid[r][c];
            grid->grid[r][c] = 0;
        }
    }

    void clearRow(int r)
    {
        for (int c = 0; c < grid->cols; c++)
        {
            specialEffects->createFxBlock(c, r, grid->grid[r][c]);
            grid->grid[r][c] = 0;
        }
    }

    int clearFullRows()
    {
        int cleared = 0;
        for (int r = grid->rows - 1; r >= 0; r--)
        {
            if (isRowFull(r))
            {
                clearRow(r);
                cleared++;
            }
            else if (cleared > 0)
            {
                moveRowDown(r, cleared);
            }
        }
        return cleared;
    }

    void generateNewTetromino()
    {
        int lastColorId = nextTetromino->colorId;

        *tetromino = *nextTetromino;

        switch (tetromino->shapeId)
        {
        case Tetromino::Shape_I:
            tetromino->moveUp(4);
            break;
        case Tetromino::Shape_O:
            tetromino->moveUp(2);
            break;
        default:
            tetromino->moveUp(3);
            break;
        }

        do
        {
            tetromino->moveX((rand() % (grid->cols - 2)) + 1);
        } while (!isCurrentPositionValid());

        *nextTetromino = Generator::getTetromino(lastColorId);
    }

    bool isCurrentPositionValid(int offsetY = 0)
    {
        for (int i = 0; i < 4; i++)
        {
            if (tetromino->blocksCurrent[i].x < 0)
            {
                return 0;
            }

            if (tetromino->blocksCurrent[i].x >= grid->cols)
            {
                return 0;
            }

            if (tetromino->blocksCurrent[i].y + offsetY >= grid->rows)
            {
                return 0;
            }

            if (tetromino->blocksCurrent[i].y + offsetY >= 0 and grid->getValue(tetromino->blocksCurrent[i].x, tetromino->blocksCurrent[i].y + offsetY))
            {
                return 0;
            }
        }
        return 1;
    }

    bool isPositionAfterNextDropValid()
    {
        return isCurrentPositionValid(1);
    }
};

class Tetris
{
public:
    sf::RenderWindow window;

    Grid grid;

    Tetromino tetromino;
    Tetromino nextTetromino;

    GameState state;

    SpecialEffects specialEffects;

    View view;
    Input input;
    Logic logic;

    Tetris() : window(sf::VideoMode(View::getWindowWidth(), View::getWindowHeight()), "Tetris"),
               state(&grid, &tetromino, &nextTetromino),
               view(&window, &grid, &tetromino, &nextTetromino, &state, &specialEffects),
               logic(&grid, &input, &tetromino, &state, &nextTetromino, &specialEffects)
    {
        state.currentState = GameState::Title;
        nextTetromino = Generator::getTetromino();
    }

    void run()
    {
        sf::Clock clock;
        window.setFramerateLimit(60);

        while (window.isOpen())
        {
            float time = clock.getElapsedTime().asSeconds();
            clock.restart();

            sf::Event e;
            while (window.pollEvent(e))
            {
                switch (e.type)
                {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed:
                    if (e.key.code == sf::Keyboard::Space)
                    {
                        input.spacebar = 1;
                    }
                    else if (e.key.code == sf::Keyboard::Right)
                    {
                        input.moveX = 1;
                    }
                    else if (e.key.code == sf::Keyboard::Left)
                    {
                        input.moveX = -1;
                    }
                    else if (e.key.code == sf::Keyboard::Up)
                    {
                        input.rotate = 1;
                    }
                    else if (e.key.code == sf::Keyboard::Down)
                    {
                        input.fastDrop = 1;
                    }
                    else if (e.key.code == sf::Keyboard::P)
                    {
                        input.paused = 1;
                    }
                    else if (e.key.code == sf::Keyboard::Q)
                    {
                        window.close();
                    }
                    else if (e.key.code == sf::Keyboard::S)
                    {
                        input.shadowSwitch = 1;
                    }
                    break;

                case sf::Event::KeyReleased:
                    if (e.key.code == sf::Keyboard::Down)
                    {
                        input.fastDrop = 0;
                    }
                    else if (e.key.code == sf::Keyboard::P)
                    {
                        input.paused = 0;
                    }
                    break;

                default:
                    break;
                }
            }

            state.update(input);
            logic.update(time);
            view.render();
        }
    }
};

int main()
{
    Tetris game;
    game.run();
    return 0;
}
