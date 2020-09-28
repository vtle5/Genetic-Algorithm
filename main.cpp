/*
Author: Vincent Le
Date: 4/4/2020
Description: A genetic algorithm application that determines the seating of a theatre based on user input.
 */

#include <iostream>
#include <math.h>
#include <time.h>

struct Point        //a point that will represent a seated person's position
{
    int x,y;
};
struct Individual   //each individual in the genetic algorithm
{
    int fitness;
    int** array;    //the "chromosome"
    int seatCount;
    Point* seatArray;   //array that holds the position over every seated person
};

float RandomFloat()     //returns a random float ranging from 0 to 1
{
    return ((float)rand())/RAND_MAX;
}

int Random1or0()        //returns one or zero randomly
{
    if (RandomFloat()<.5)
    {
        return 0;
    } else{
        return 1;
    };
}

float Distance(int x1,int y1,int x2,int y2) //returns the distance between 2 points
{
    float ans=powf(x2-x1,2) + powf(y2-y1,2);
    ans=sqrt(ans);
    return ans;
}

int RandomInRange(int min,int max)      //returns a number between 2 arguments
{
    float num=RandomFloat();            //generates a random float number
    num=num*(max+1-min)+min;            //restricts the number between min and max
    return (int)num;
}

Point* GenerateSeatArray(int** array,int seatCount,int row,int col,int seatDist,int rowDist)
{
    Point* newArray = new Point[seatCount];   //creates the seat array based on the amount of seats assigned
    int i,j;
    int k=0;                              //for transversing the seatArray
    for(i=0;i<row;i++)                  //this re runs again to generate the seatArray
    {
        for(j=0;j<col;j++)
        {
            if (array[i][j]==1)         //add to seat count if a person was assigned
            {
                newArray[k].x=i*seatDist;       //sets the coords of the point
                newArray[k].y=j*rowDist;        //these points are multiplied by the distance between seats and rows
                k++;                        //moves k forward.
            }
        }
    }
    return newArray;
}

int ScoreFitness(Point* seatArray, int count,float minDist)    //determines the fitness of an Individual
{
    int score=count;    //give a point for every seated individual
    int penalizeCount=0;
    int i,j;
    int violation=0;    //for checking if the array is 100% safe at the end
    for(i=0;i<count;i++)      //for loop to go through the seating array
    {
        for(j=0;j<count;j++)
        {
            if (i!=j)       //make sure it isn't comparing to itself
            {
                if (Distance(seatArray[i].x,seatArray[i].y,seatArray[j].x,seatArray[j].y) < minDist)
                {                           //check for social distancing violation
                    score -= 2;       //score penalty
                    violation = 1;    //triggers the check for 100% safety
                    penalizeCount++;
                    j = count;        //exit the inner loop as we cannot penalize a single point multiple times
                }
            }
        }
    }
    if (violation==0)   //if violation hasn't triggered, award bonus points
    {
        score+=10;
    }
    return score;
}

Individual GenerateIndividual(int row, int col,int seatDist,int rowDist,float minDist)
{                                   //creates an individual with a random array
    Individual gen;
    int i,j;            //for arrays
    gen.seatCount=0;    //for counting the amount of people seated

    gen.array = new int*[row];          //generates the array/chromosome
    for(i=0;i<row;i++)                  //nested for loop to travel entire array
    {
        gen.array[i]=new int[col];      //makes an array for 2 dimensions
        for(j=0;j<col;j++)
        {
            gen.array[i][j] = Random1or0(); //fills the array with a one or zero randomly

            if (gen.array[i][j]==1)         //add to seat count if a person was assigned
            {
                gen.seatCount++;
            }
        }
    }
    gen.seatArray = GenerateSeatArray(gen.array,gen.seatCount,row,col,seatDist,rowDist);    //generates the seatArray
    gen.fitness= ScoreFitness(gen.seatArray,gen.seatCount,minDist); //score the Individual
    return gen;
}

Individual Mate(Individual par1,Individual par2,int row,int col,int seatDist,int rowDist, float minDist)
{                       //creates an individual with a mixed array from both parents and a chance to mutate
    Individual gen;
    int i,j;            //for arrays
    gen.seatCount=0;    //for counting the amount of people seated

    gen.array = new int*[row];          //generates the array/chromosome
    for(i=0;i<row;i++)
    {
        gen.array[i]=new int[col];
        for(j=0;j<col;j++)
        {
            float rand=RandomFloat();   //45% for parent 1 gene, 45% for parent 2 gene, 10% for mutation.
            if (rand<.45)               //take gene from parent 1
            {
                gen.array[i][j]=par1.array[i][j];
            }else{
                if (rand<.9)
                {
                    gen.array[i][j]=par2.array[i][j];     //take gene from parent 2
                } else{
                    gen.array[i][j]=Random1or0();         //mutate
                }
            }
            if (gen.array[i][j]==1)
            {
                gen.seatCount++;
            }
        }
    }
    gen.seatArray = GenerateSeatArray(gen.array,gen.seatCount,row,col,seatDist,rowDist);    //generates the seatArray
    gen.fitness= ScoreFitness(gen.seatArray,gen.seatCount,minDist); //score the Individual
    return gen;
}

void DisplayIndividual(Individual ind,int row,int col)    //prints the array and it's fitness
{
    int i;
    int j;
    for(i=0;i<row;i++)
    {
        for(j=0;j<col;j++)
        {
            std::cout<< ind.array[i][j] <<" ";
        }
        std::cout<< "\n";   //prints a new line after a row is printed
    }
    std::cout<< "Fitness: " << ind.fitness <<std::endl;   //display fitness
    std::cout<< "----------" <<std::endl;   //line for organization
}

void PopulationSort(Individual array[],int low,int high)  //quicksort based on fitness
{
    int pivot,i,j;
    Individual temp;

    if (low<high){      //ends the recurse
        pivot=low;      //sets pivots, i, and j
        i= low;
        j=high;

        while(i<j)      //as high and low approach eachother
        {
            while(array[i].fitness<=array[pivot].fitness && i<high)
            {i++;}                                  //moves "low"
            while(array[j].fitness>array[pivot].fitness)
            {j--;}                                  //moves "high"

            if (i<j)                        //if i<j swap them
            {
                temp=array[i];
                array[i]=array[j];
                array[j]=temp;
            }
        }
        temp=array[pivot];
        array[pivot]=array[j];
        array[j]=temp;                      //swaps the pivot with high
        PopulationSort(array,low,j-1);    //recurses for the 1st partition
        PopulationSort(array,j+1,high);   //recurses for the second partition
    }
}
void PrintError()       //this gets printed a lot for bad input
{
    std::cout << "Please input a value greater than 0." << std::endl;
}


int main() {
    srand((unsigned)(time(0)));     //randomises the randomness seed.
    int rowCount,colCount;
    int seatDist,rowDist;
    std::cout << "Welcome to Covid-19 Theatre Seats: Genetic Algorithm Edition" << std::endl;
    do {
        std::cout << "How many rows are there?" << std::endl;
        std::cin >> rowCount;
        if (rowCount<1)
        {PrintError();}     //loop if bad input
    }while(rowCount<1);
    do {
        std::cout << "How many seats are in a row?" << std::endl;
        std::cin >> colCount;
        if (colCount<1)
        {PrintError();}     //loop if bad input
    }while(colCount<1);

    do {
        std::cout << "How many inches between each seat in a row?" << std::endl;
        std::cin >> seatDist;
        if (seatDist<1)
        {PrintError();}     //loop if bad input
    }while(seatDist<1);
    do {
        std::cout << "How many inches between each row?" << std::endl;
        std::cin >> rowDist;
        if (rowDist<1)
        {PrintError();}     //loop if bad input
    }while(rowDist<1);
    std::cout <<"Rows: "<<rowCount<<", Seats per Row: "<<colCount<<", Distance between seats: "<<seatDist;
    std::cout <<", Distance between Rows: "<<rowDist << std::endl;
    std::cout <<"Genetic Algorithm Configuration"<<std::endl;
    int popCount,genCount;
    do {
        std::cout <<"How many Individuals per generation? Input should be at least 10"<<std::endl;
        std::cin >> popCount;
        if (popCount<10)
        {std::cout << "Please input a value at least 10."<<std::endl;}     //loop if bad input
    }while(popCount<10);

    std::cout << "Generating generation 0..." << std::endl;

    int generation = 0;
    float minDist = 72;     //minimum Distance requirement
    Individual population[popCount];    //the population of Individuals
    int i, loop;
    for (i = 0; i < popCount; i++)        //generates generation 0
    {
        population[i] = GenerateIndividual(rowCount, colCount, seatDist, rowDist, minDist);
    }

    int choice;
    std::cout << "Display generation 0? Enter 1 for yes." << std::endl;
    std::cin >> choice;
    if (choice==1)
    {
        PopulationSort(population, 0, popCount);
        std::cout << "GENERATION: " << generation << std::endl;     //prints out the generation number
        for (i = 0; i < popCount; i++)
        {
            DisplayIndividual(population[i], rowCount, colCount);   //displays every Individual in the population
        }

    }

    do {
        std::cout << "How many generations to create? Entering 0 or lower will exit." << std::endl;
        std::cin >> genCount;
        for (loop = 0; loop < genCount; loop++)     //big loop that creates multiple generations
        {
            PopulationSort(population, 0, popCount);    //sorts the population from lowest fitness to highest

            std::cout << "GENERATION: " << generation << std::endl;     //prints out the generation number
            for (i = 0; i < popCount; i++)
            {
                DisplayIndividual(population[i], rowCount, colCount);   //displays every Individual in the population
            }

            Individual newGeneration[popCount];
            int elitismIndex = (popCount - popCount * .10);     //10% of the top can continue to the next generation
            int minIndex = popCount * .5;       //gets the index for mating so that only the top 50% can mate
            for (i = 0; i < elitismIndex; i++)   //creates a new generation through mating for 90% of the population
            {
                int par1, par2;
                do {
                    par1 = RandomInRange(minIndex, popCount - 1);
                    par2 = RandomInRange(minIndex, popCount - 1);
                } while (par1 != par2);                              //ensures that one Individual is not both parents
                newGeneration[i]= Mate(population[par1],population[par2],rowCount,colCount,seatDist,rowDist,minDist);
            }
            for (i = elitismIndex; i < popCount; i++)   //elitism allowing the top 10% to move to the next generation
            {
                newGeneration[i] = population[i];
            }

            for (i = 0; i < popCount; i++)      //overwrites the population with the new generation
            {
                population[i] = newGeneration[i];
            }
            generation++;                       //add to the generation counter
        }
    }while(genCount>0);
    return 0;
}
