#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <chrono>

// Include the source file directly for easy compilation in a single unit test file
#define main test_main
#include "Snake.cpp"
#undef main

// Fake Random Source to precisely control fruit placement
class DeterministicFakeRandomSource : public IRandomSource {
public:
    int sequenceIndex = 0;
    std::vector<int> returnedValues;

    void setNextReturns(std::vector<int> values) {
        returnedValues = values;
        sequenceIndex = 0;
    }

    int fallbackCounter = 0;
    int getInt(int minVal, int maxVal) override {
        if (sequenceIndex < returnedValues.size()) {
            return returnedValues[sequenceIndex++];
        }
        int val = minVal + (fallbackCounter % (maxVal - minVal + 1));
        fallbackCounter++;
        return val;
    }
};

// Fake Input Source to precisely control keystrokes
class FakeInputSource : public IInputSource {
public:
    std::vector<int> keys;
    int index = 0;
    
    void pushKey(int k) {
        keys.push_back(k);
    }
    
    bool kbhit() override {
        return index < keys.size();
    }
    
    int getch() override {
        if (index < keys.size()) return keys[index++];
        return 0;
    }
};

void test_rule3() {
    DeterministicFakeRandomSource fakeRng;
    FakeInputSource fakeInput;
    
    // We want the fruit to spawn at (16, 7) right next to the snake which starts at (15, 7) (WIDTH/2, HEIGHT/2)
    // WIDTH is 30, HEIGHT is 15.
    // snake starts at (15, 7)
    fakeRng.setNextReturns({
        16, 7, // GenerateFruit: x, y
        0,     // GenerateFruit: fruitSymbol index
        0, 0,  // GenerateBomb
        0, 0   // GenerateObstacles (if called)
    });

    SnakeGame game("Test", &fakeRng, &fakeInput);
    
    int initialScore = game.score;
    int initialLength = game.snake.size();
    
    assert(initialScore == 0);
    assert(initialLength == 1);
    
    // Fruit should be at 16, 7
    assert(game.fruit.x == 16);
    assert(game.fruit.y == 7);
    
    // Move snake RIGHT to eat it
    fakeInput.pushKey(224);
    fakeInput.pushKey(77);
    
    game.Input();
    
    // Setup RNG for the *next* fruit spawn after eating
    fakeRng.setNextReturns({
        2, 2, // New fruit location
        0
    });
    
    // Tick the logic
    game.Logic();
    
    // Assert Rule 3:
    std::cout << "Score after eating: " << game.score << std::endl;
    std::cout << "Length after eating: " << game.snake.size() << std::endl;
    
    assert(game.score == initialScore + 10);
    assert(game.snake.size() == initialLength + 1);
    
    std::cout << "Rule 3 test passed successfully!" << std::endl;
}

int main() {
    std::cout << "Running Tests..." << std::endl;
    test_rule3();
    return 0;
}
