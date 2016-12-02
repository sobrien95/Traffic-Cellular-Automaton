#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define VMAX 4
#define GMAX 10
#define SIZE 50

struct Car {
   int id;
   int velocity;
   int position;
   int nextPosition;
};

int generation = 0;
struct Car *cells[SIZE] = {NULL};
struct Car *nextGen[SIZE] = {NULL};

/*
** Return the smaller of 2 numbers
*/
int min(int a, int b)
{
	int c = 0;

	if( a < b )
	{
		c = a;;
	}

	else
	{
		c = b;
	}

	return c;
}

/*
**function to return the size of the gap in front of
**the car at the index of "position" in the cells array
*/
int findGap(int position)
{
    //start off with no gap
    int gap = 0;

    int i = 1;
    //check the cells that the car will reach at this velocity
    while(i <= cells[position]->velocity)
    {
        //increase gap if cell is empty
        if(cells[position + i] == NULL)
        {
            gap++;
        }
        //return the gap once a cell isn't null
        else
        {
            return gap;
        }
        i++;
    }
    return gap;
}

/*
**increases the speeds of each car in the cells towards VMAX
**doesn't actually move them, just changes the speeds & next positions
*/
void accelerationRule()
{
    int i;
    for (i = 0; i < SIZE; i ++)
    {
        if(cells[i] != NULL)
        {
            //accelerate car
            cells[i]->velocity = min(cells[i]->velocity + 1, VMAX);
        }
    }
}

/*
**Slow the car down if it's going to collide with the car in front
*/
void brakeRule()
{
    int i;
    for(i = 0; i < SIZE; i++)
    {
        if(cells[i] != NULL)
        {
            int gap = findGap(cells[i]->position);
            cells[i]->velocity = min(gap, cells[i]->velocity);
        }
    }
}

/*
** Introduce an element of randomness to the car's velocity
*/
void randomisationRule()
{
    int i;
    for(i = 0; i < SIZE; i++)
    {

        if(cells[i] != NULL)
        {
            if(rand()%10 >= 8)
            {
                int speed = (rand() % VMAX) + 1;
                cells[i]->velocity = speed;
                printf("random %d, car %d\n", cells[i]->velocity, cells[i]->id);
            }
        }
    }
}

bool isArrayEmpty()
{
    int i;
    bool isEmpty = true;
    for(i = 0; i < SIZE; i++)
    {
        if (cells[i] != NULL)
        {
            isEmpty = false;
        }
    }
    return isEmpty;
}

struct Car *createCar()
{
    struct Car *c = (struct Car*)malloc(sizeof(struct Car));
    //struct Employee* ret = (struct Employee*)malloc(sizeof(struct Employee));
    c->id = generation;
    c->velocity = 0;
    c->position = 0;
    printf("creating car %d\n", c->id);
    return c;
}

void moveCars()
{
    printf("Moving cars\n");
    int i;
    for(i = 0; i < SIZE; i++)
    {
        if(cells[i] != NULL)
        {
            cells[i]->nextPosition = cells[i]->position + cells[i]->velocity;
            if(!(cells[i]->nextPosition >= SIZE))
            {
                cells[i]->position = cells[i]->nextPosition;
                nextGen[cells[i]->nextPosition] = cells[i];
            }
            cells[i] = NULL;
        }
    }
    for(i=0; i < SIZE; i++)
    {
        cells[i] = nextGen[i];
        nextGen[i] = NULL;
    }
}

int main ()
{
    srand(time(NULL));
    bool run = true;

    while(run == true)
    {
        //create cars until we meet the designated number
        if(generation < GMAX)
        {
            //check that no car is in the first cell
            if(cells[0] == NULL)
            {
                generation++;
                struct Car *car = createCar();
                cells[0] = car;
            }
        }
        //increase the speed of cars (no movement yet)
        accelerationRule();
        //introduce an element of randomness to the driving speeds
        randomisationRule();
        //slow cars down to avoid collisions
        brakeRule();

        //end while loop if the cars have all passed through the cells
        if(isArrayEmpty())
        {
            run = false;
            continue;
        }
        else
        {
            //loop to display car details after each run
            int i;
            for(i = 0; i < SIZE; i++)
            {
                if(cells[i] != NULL)
                {
                    printf("i = %d. Car id: %d, position: %d, speed: %d\n", i, cells[i]->id, cells[i]->position, cells[i]->velocity);
                }
            }
            //move the cars to their next positions
            moveCars();
        }//end else
    }//end while
	return 0;
}
