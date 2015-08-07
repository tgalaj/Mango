#include "SimpleGame.h"

SimpleGame::SimpleGame()
{

}

SimpleGame::~SimpleGame()
{

}

void SimpleGame::processInput()
{
    if (Input::getKeyDown(Input::Return))
        printf("GetKeyDown - Return\n");
}

void SimpleGame::update()
{
    printf("Game update!!\r");
}
