/*
 * finitePopModel.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: Aditya
 *      Student ID: 2021132636
 */

#include<iostream>
#include"finitePopTypes.h"
#include "rv.h"
#include "event.h"

using namespace std;

/*********************************
 * Declaration of Global Variables
 *********************************/
  double clock = 0.0;             // System clock
  EventList Elist;                // Create event list
  sysConfig configInfo;           // This is needed to store User Inputs
  statCounters countInfo;         // This keeps track of statistical information

static void getUserInputs();
static void initializeQSystem(int numTerminals);
static void generateArrEvent(int numTerminals);


int main()
{
  bool    done             = false;
  double  EN               = 0.0;     // For calculating E[N]
  double  Util             = 0.0;
  Event*  CurrentEvent;

  //Get system configuration inputs from user
  getUserInputs();

  countInfo.terminalPool = configInfo.numTerminals;

  //this will initialize the queuing system
  initializeQSystem(configInfo.numTerminals);

  /*
   * This while loop represents the finite population system where
   * customer arrive (from the event list in our case),
   * get accepted if the system has capacity, get served
   * by the next available server and then depart
   */
  while (!done)
  {
    CurrentEvent = Elist.get();               // Get next Event from list
    double prev = clock;                      // Store old clock value
    clock=CurrentEvent->time;                 // Update system clock

    switch (CurrentEvent->type)
    {
      case ARR:
        countInfo.NArr++;
        EN += countInfo.NSys*(clock-prev);

        if(countInfo.NSys<configInfo.numCPU)
          Util += countInfo.NSys*(clock-prev);
        else
          Util += configInfo.numCPU*(clock-prev);

        if(countInfo.serverPool<configInfo.numCPU)
        {
          //Server is idle, accept the element in the system and depart it
          countInfo.serverPool++;
          countInfo.NSys++;
          Elist.insert(clock+exp_rv(configInfo.serviceRate),DEP);
        }
        else if((countInfo.serverPool==configInfo.numCPU) &&
                (countInfo.NQueue<configInfo.queueSize))
        {
          //All servers are occupied, accept the element and queue it
          countInfo.NSys++;
          countInfo.NQueue++;
        }
        else if((countInfo.serverPool==configInfo.numCPU) &&
                (countInfo.NQueue == configInfo.queueSize))
        {
          //system is full to its capacity. Block the customer and free a terminal
          countInfo.NBlocked++;
          countInfo.terminalPool++;
          generateArrEvent(configInfo.numTerminals);
        }
        break;
      case DEP:
        EN += countInfo.NSys*(clock-prev);

        if(countInfo.NSys<configInfo.numCPU)
          Util += countInfo.NSys*(clock-prev);
        else
          Util += configInfo.numCPU*(clock-prev);

        if(countInfo.NSys)
        {
          countInfo.NSys--;
          countInfo.serverPool--;
          countInfo.terminalPool++;
          countInfo.NDep++;
          if ((countInfo.serverPool < configInfo.numCPU) && (countInfo.NQueue>0))
          {
            //If a server is idle and customer is waiting in queue then generate dep
            countInfo.serverPool++;
            countInfo.NQueue--;
            Elist.insert(clock+exp_rv(configInfo.serviceRate),DEP);
          }
        }
        //triger new arrival. If a terminal is free then it will generate arr
        generateArrEvent(configInfo.numTerminals);
        break;
    }
    delete CurrentEvent;
    if (countInfo.NDep > 100000)
      done=true;        // End condition
  }
  cout<<endl<<"****Statistical Output with lambda="<<configInfo.arrivalRate<<"****"<<endl;
  cout <<"Expected number of customers (simulation)= " << EN/clock << endl;
  cout<<"Expected time spent in the system= "<<EN/100000<<endl;
  cout<<"P(Blocking)= "<<(double)countInfo.NBlocked/countInfo.NArr<<endl;
  cout<<"Utilization= "<<(double)Util/(clock*configInfo.numCPU)<<endl;
}


/*
 * This Function gets System Configuration
 * information from the user
 */
static void getUserInputs()
{
  int l   = 0;
  int m   = 0;
  int k   = 0;
  int mu  = 0;

  cout<<"****Finite Population Queuing System Simulation****"<<endl<<endl;
  cout<<"Please Provide Following Inputs:"<<endl;

  cout<<"L - Number of Terminals In The System [Range is 1-10][Default is 1]:";
  cin>>l;
  if(l>=1 && l<=10)
  {
    configInfo.numTerminals=l;
  }
  cout<<"m - Number of CPUs in the System [Default is 1]:";
  cin>>m;
  if(m>=1)
  {
    configInfo.numCPU = m;
  }
  cout<<"K - Queue Size [Default is 1]:";
  cin>>k;
  if(k>=1)
  {
    //Queue Size is K (system size) - m (number of CPU)
    configInfo.queueSize = k-configInfo.numCPU;
  }
  cout<<"mu - Service Rate of Each CPU [Default is 2 for stability]: ";
  cin>>mu;
  if(mu>=2)
  {
    configInfo.serviceRate = mu;
  }
}

/*
 * This function initializes the system by
 * generating arrivals for the available free
 * terminal
 */
static void initializeQSystem(int numTerminals)
{
  //generate arrival request for each request.
  for(int i=0; i<numTerminals; i++)
  {
    generateArrEvent(numTerminals);
  }
}

/*
 * This function checks if there is free terminal
 * and generates arrival if there is free terminal
 */
static void generateArrEvent(int numTerminals)
{
  if((countInfo.terminalPool>0) && (countInfo.terminalPool<=numTerminals))
  {
    Elist.insert(clock + exp_rv(configInfo.arrivalRate),ARR);
    countInfo.terminalPool--;
  }
}
