#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define VMAX 3
#define GMAX 5
#define SIZE 20

struct Car {
   int id;
   int velocity;
   int position;
   int nextPosition;
   bool alreadyMoved;
};

int generation;
int idCount = 1;
int v = 0;
int u = 0;
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

 /*int * make_array()
 {
	 int i;
	 for (i = 0; i < SIZE; i++) {

		 cells[i] = 0;
	 }

	 return cells;
 }*/

/*
** Increase speed of the car towards VMAX
*/
int accelerate_rule(int velocity)
{
	int v = min(velocity + 1,VMAX);


	int random = rand() % 10;
	if(random > 7)
    {
        v = randomisation_rule();
    }

	return v;
}

int findGap(int position)
{
    int gap = 0;
    int i = 1;
    while(i < cells[position]->velocity)
    {
        if(cells[position + i] == NULL)
        {
            gap++;
        }
        else
        {
            return gap;
        }
        i++;
    }
}

/*
**Slow the car down if it's going to collide with the car in front
*/
int brake_rule(int speed, int position)
{
    int gap = findGap(position);
    return min(speed, gap);
}

/*
** Introduce an element of randomness to the car's velocity
*/
int randomisation_rule()
{
    printf("Random Rule\n");
    int speed = rand() % (VMAX + 1);
    return speed;
}

int rules()
{
	v = min(u,v);
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
    c->id = idCount;
    c->velocity = 0;
    c->position = 0;
    printf("creating car %d\n", c->id);
    idCount ++;
    return c;
}

/*
**accelerates and randomises the speeds of each car in the cells
**doesn't actually move them, just changes the speeds & next positions
*/
void accelerateAllCars()
{
    int i;
    for (i = 0; i < SIZE; i ++)
    {
        if(cells[i] != NULL)
        {
            //reset boolean for moveCars method to know if it has moved a car
            cells[i]->alreadyMoved = false;
            //accelerate cars (not actually moving yet)
            cells[i]->velocity = accelerate_rule(cells[i]->velocity);
            //brake to avoid collisions
            cells[i]->velocity = brake_rule(cells[i]->velocity, cells[i]->position);
            //set next positions
            cells[i]->nextPosition = cells[i]->position + cells[i]->velocity;
        }
    }
}

void moveCars()
{
    printf("Moving cars\n");
    accelerateAllCars();
    int i;
    for(i = 0; i < SIZE; i++)
    {
        if(cells[i] != NULL)
        {
            if(!(cells[i]->nextPosition >= SIZE))
            {
                cells[i]->position = cells[i]->nextPosition;
                nextGen[cells[i]->nextPosition] = cells[i];
            }
            cells[i] = NULL;
        }
       /* if (cells[i] != NULL && !cells[i]->alreadyMoved)
        {
            int pos = cells[i]->position;
            int next = cells[i]->nextPosition;
            cells[next] = cells[pos];
            cells[next]->alreadyMoved = true;
            cells[pos] = NULL;
        }*/
    }
    for(i=0; i < SIZE; i++)
    {
        cells[i] = nextGen[i];
        nextGen[i] = NULL;
    }

    int j;
    for(j = 0; j < SIZE; j++)
    {
        if(cells[j] != NULL)
        {
            printf("j = %d. Car id: %d, position: %d, speed: %d\n", j, cells[j]->id, cells[j]->position, cells[j]->velocity);
        }
    }

}

int main ()
{
    srand(time(NULL));

	//	int nextgen [SIZE];

    bool run = true;
    while(run == true)
    {
        while(generation < GMAX)
        {
            generation ++;
            int r = 1;//rand() % 4;
            if (r == 1 && cells[0] == NULL)
            {
                struct Car *car = createCar();
                cells[0] = car;
            }
            moveCars();
        }

        moveCars();
        if(isArrayEmpty())
        {
            run = false;
        }
    }
	return 0;
}
