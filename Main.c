#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define VMAX 4
#define GMAX 80
#define SIZE 1000
#define NUM_THREADS 4

struct Car {
   int id;
   int velocity;
   int position;
   int nextPosition;
   int journeyTime;
};

pthread_barrier_t barrier;
int generation = 0;
int results[SIZE][GMAX];
struct Car *cells[SIZE] = {NULL};
struct Car *nextGen[SIZE] = {NULL};
struct Car *cars[GMAX] = {NULL};
FILE *outputFile;

/*
** Return the smaller of 2 numbers.
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
}//end min

/*
** Function to return the size of the gap in front of
** the car at the index of "position" in the cells array.
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
}//end findGap

/*
** Increases the speeds of each car in the cells towards VMAX.
** Doesn't actually move them, just changes the speeds & next positions.
*/
void* accelerationRule(int first, int last)
{
    int i;
    //loop through cells array
    for(i = first; i < last; i++)
    {
        if(cars[i] != NULL && cars[i]->position != -1)
        {
            int index = cars[i]->id - 1;
            cars[index]->velocity = min(cars[index]->velocity + 1, VMAX);
        }
    }
}//end accelerationRule

/*
** Slow the car down if it's going to collide with the car in front
*/
void* brakeRule(int first, int last)
{
    int i;
    //loop through cells array
    for(i = first; i < last; i++)
    {
        if(cars[i] != NULL && cars[i]->position != -1)
        {
            int gap = findGap(cars[i]->position);
            cars[i]->velocity = min(gap, cars[i]->velocity);
        }
    }
}//end brakeRule

/*
** Introduce an element of randomness to the car's velocity.
** Sets the car's speed to a random number between 1 and VMAX.
*/
void* randomisationRule(int first, int last)
{
    unsigned int seed = time(NULL);
    int i;
    //loop through cells array
    for(i = first; i < last; i++)
    {
        if(cars[i] != NULL && cars[i]->position != -1)
        {
            int r = rand()%10;
            if(r >= 7)
            {
                int position = cars[i]->position;
                //set the speed of a car to be a random number less than or equal to VMAX
                int speed = (rand() % VMAX) + 1;
                cells[position]->velocity = speed;
                printf("random %d, car %d\n", cells[position]->velocity, cells[position]->id);
            }
        }
    }
}//end randomisationRule

/*
** Checks to see if every cell in the array is empty
*/
bool isArrayEmpty()
{
    bool isEmpty = true;
    int i;

    //loop through array
    for(i = 0; i < SIZE; i++)
    {
        //return once a non-empty cell is found
        if (cells[i] != NULL)
        {
            return isEmpty = false;
        }
    }
    return isEmpty;
}//end isArrayEmpty

/*
** Function to create a default car structure to be placed into a cell
*/
struct Car *createCar()
{
    struct Car *c = (struct Car*)malloc(sizeof(struct Car));
    c->id = generation;
    c->velocity = 0;
    c->position = 0;
    c->journeyTime = 0;
    printf("creating car %d\n", c->id);
    return c;
}//end createCar

/*
** Moves the cars forwards in the cells array using a second temporary array
*/
void* moveCars()
{
    printf("Moving cars\n");
    int i;
    //loop through array
    for(i = 0; i < GMAX; i++)
    {
        if(cars[i] != NULL && cars[i]->position != -1)
        {
            cars[i]->journeyTime++;
            //set the car's nextPosition based off its speed
            cars[i]->nextPosition = cars[i]->position + cars[i]->velocity;
            //check that the nextPosition isn't outside the array
            if(!(cars[i]->nextPosition >= SIZE))
            {
                //change car's position field
                cars[i]->position = cars[i]->nextPosition;
                //put the car in its new cell in the temporary array
                nextGen[cars[i]->nextPosition] = cars[i];
            }
            //set car's position to -1 once it's crossed the array
            else
            {
                cars[i]->position = -1;
            }
            //reset the old cell
            cells[i] = NULL;
        }//end if
    }//end for loop
    //loop through nextGen array and transfer cars back to cells
    for(i=0; i < SIZE; i++)
    {
        cells[i] = nextGen[i];
        nextGen[i] = NULL;
    }
}//end moveCars

//thread to calculate the next positions for a portion of the cars
void* create_generation ( void* rank){
    //calculate how many jobs to be done, what indexes to start and finish at
    int numJobs = GMAX/NUM_THREADS;
    int myFirst = (int)rank * numJobs;
    int myLast = myFirst + numJobs;

    int i;
    for(i = myFirst; i < myLast; i++)
    {
        //increase the speed of cars (no movement yet)
        accelerationRule(myFirst, myLast);
        //introduce an element of randomness to the driving speeds
        randomisationRule(myFirst, myLast);
        //slow cars down to avoid collisions
        brakeRule(myFirst, myLast);
    }
    //wait for the other threads to finish their calculations before moving on to the movement stage
    pthread_barrier_wait(&barrier);
}

int main (int argc, char* argv[])
{
   // srand(time(NULL));
    int i;
    int s;
    int iteration = 0;
    bool run = true;
    long thread = 0;
	pthread_t* thread_handles;
	pthread_barrier_init(&barrier, NULL, 5);

	thread_handles = malloc(NUM_THREADS * sizeof(pthread_t));

    //clear the contents of the results file, close until ready to write again
    outputFile = fopen("results.dat", "w");
    fclose(outputFile);


    //initialise the results array with default values of -1
    for(s = 0; s < SIZE; s++)
    {
        for(i = 0; i < GMAX; i++)
        {
            results[s][i] = -1;
        }
    }

    while(run == true)
    {
        iteration++;
        //create cars until we meet the designated number
        if(generation < GMAX)
        {
            //check that no car is in the first cell
            if(cells[0] == NULL)
            {
                generation++;
                struct Car *car = createCar();
                cells[0] = car;
                cars[generation-1] = car;
            }
        }

        //create threads to calculate the next positions of the cars
        for (thread = 0; thread < NUM_THREADS; thread++)
        {
            pthread_create(&thread_handles[thread],NULL,create_generation,(void*)thread);
        }
        //wait for each thread to finish with their calculaions before moving any cars
        pthread_barrier_wait(&barrier);
        moveCars();

        //end while loop if the cars have all passed through the cells
    	if(isArrayEmpty())
    	{
    		run = false;
    		continue;
    	}
    	//output positions of cars  & iteration number to results array with each iteration
    	else
    	{
            //loop to display details of the cars in the cells after each run
    		for(i = 0; i < SIZE; i++)
    		{
                if(cells[i] != NULL)
                {
                    //output the position of the car to the results array, each column represents 1 car
                    int id = cells[i]->id - 1;
                    results[iteration - 1][id] = cells[i]->position;
                }
            }//end for
        }//end else
    }//end while

    //open results file for adding data
    outputFile = fopen("results.dat", "a");

    for (i = 0; i < GMAX; i++)
    {
        for (s = 0; s < SIZE; s++)
        {
            if(results[s][i] != -1)
            {

                int p = results[s][i];
                fprintf(outputFile, "%d %d\n", s, p);
            }
        }
        fprintf(outputFile, "\n");
    }
    printf("Done writing to file \"results.dat\", closing.");
    fclose(outputFile);
	return 0;
}//end main
