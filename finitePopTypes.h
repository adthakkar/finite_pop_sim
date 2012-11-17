/*
 * finitePopTypes.h
 *
 *  Created on: Oct 29, 2012
 *      Author: Aditya
 */

#ifndef FINITEPOPTYPES_H_
#define FINITEPOPTYPES_H_

/*********************************
 * Declaration of Structures
 *********************************/
struct sysConfig
{
  int numTerminals;
  int numCPU;
  int queueSize;
  int serviceRate;
  double arrivalRate;

  sysConfig()
  {
    numTerminals                = 1;
    numCPU                      = 1;
    queueSize                   = 1;
    serviceRate                 = 2;
    arrivalRate                 = 0.6;
  }
};

struct statCounters
{
  int NArr;
  int NDep;
  int NSys;
  int terminalPool;
  int serverPool;
  int NQueue;
  int NBlocked;
};

/*********************************
 * Declaration of Enumerations
 *********************************/
enum
{
  ARR,
  DEP
};

#endif /* FINITEPOPTYPES_H_ */
