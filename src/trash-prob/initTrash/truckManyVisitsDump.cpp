/*VRP*********************************************************************
 *
 * vehicle routing problems
 *      A collection of C++ classes for developing VRP solutions
 *      and specific solutions developed using these classes.
 *
 * Copyright 2014 Stephen Woodbridge <woodbri@imaptools.com>
 * Copyright 2014 Vicky Vergara <vicky_vergara@hotmail.com>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the MIT License. Please file LICENSE for details.
 *
 ********************************************************************VRP*/

#include <limits>
#include <stdexcept>
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "twc.h"
#include "twbucket.h"
//#include "plot.h"

#include "truckManyVisitsDump.h"


// evalSwap position i with position j on the same truck
// j > i
//  ....  i-1 i i+1 ..... j-1 j j+1
// esto va en twPath
double TruckManyVisitsDump::e_evalIntraSw(Vehicle &truck, POS i, POS j){
  assert(i > 0);
  assert(j > i);
  assert(j < truck.size());

  if (truck[i].isDump() || truck[j].isDump())
      return VRP_MAX();
  double deltaTravelTime;
  double originalTravelTimeI;
  double originalTravelTimeJ;
  double newTravelTimeI;
  double newTravelTimeJ;
    
  if (j > i + 1) {
    originalTravelTimeI = twc->TravelTime(truck[i-1], truck[i], truck[i+1]);
    newTravelTimeI = twc->TravelTime(truck[i-1], truck[j], truck[i+1]);

    if (j != truck.size() - 1) {
      originalTravelTimeJ = twc->TravelTime(truck[j-1], truck[j], truck[j+1]);
      newTravelTimeJ = twc->TravelTime(truck[j-1], truck[i], truck[j+1]);
    } else {
      originalTravelTimeJ = twc->TravelTime(truck[j-1], truck[j], truck.getDumpSite());
      newTravelTimeJ = twc->TravelTime(truck[j-1], truck[i], truck.getDumpSite());
    }
    deltaTravelTime = (newTravelTimeI + newTravelTimeJ)
                   - (originalTravelTimeI + originalTravelTimeJ);

  
#ifdef VRPMAXTRACE
  if (newTravelTimeI < originalTravelTimeI) {
    DLOG(INFO) << std::fixed << std::setprecision(4);
    DLOG(INFO) << "travelTime (" << truck[i-1].id() <<" , " << truck[i].id() << " , " << truck[i + 1].id() << ")= " 
               << originalTravelTimeI;
    DLOG(INFO) << "travelTime (" << truck[j-1].id() <<" , " << truck[j].id() << " , " << truck[j + 1].id() << ")= " 
               << originalTravelTimeJ;

    DLOG(INFO) << "travelTime (" << truck[i-1].id() <<" , " << truck[j].id() << " , " << truck[i + 1].id() << ")= " 
               << newTravelTimeI;
    DLOG(INFO) << "travelTime (" << truck[j-1].id() <<" , " << truck[i].id() << " , " << truck[j + 1].id() << ")= " 
               << newTravelTimeJ;
    DLOG(INFO)  << "Delta TravelTime: " << deltaTravelTime;
  }
#endif
  } else { // i-1 (i == j-1) (i + 1 == j) j+1 son contiguos
    double originalTravelTime = twc->TravelTime(truck[i-1], truck[i], truck[j], truck[j+1]);
    double newTravelTime = twc->TravelTime(truck[i-1], truck[j], truck[i], truck[j+1]);
    deltaTravelTime = newTravelTime - originalTravelTime;
#ifdef VRPMAXTRACE
    DLOG(INFO) << std::fixed << std::setprecision(4);
    DLOG(INFO) << "travelTime (" << i-1 <<" , " << i << " , " << j << " , " << j + 1 << ")= " 
               << originalTravelTime;
    DLOG(INFO) << "travelTime (" << i-1 <<" , " << j << " , " << i << " , " << j + 1 << ")= " 
               << newTravelTime;
#endif

  }
  //if ((originalTravelTimeI - newTravelTimeI) < 0) return originalTravelTimeI - newTravelTimeI;
  return deltaTravelTime;
};

void TruckManyVisitsDump::IntraSwMoves(Vehicle &truck) {
  DLOG(INFO) << "starting IntraSwMoves";
  double deltaTravelTime;
  int count = 0;
  for (int i = 1; i < truck.size()-1; ++i) {
    for (int j = i + 1; j < truck.size()-1; ++j) {
      deltaTravelTime = e_evalIntraSw(truck,i,j);
      if (deltaTravelTime == VRP_MAX()) {
        continue;
      }
      if (deltaTravelTime < 0) {
        truck.e_swap(i,j);
#if 0
        DLOG(INFO) << "swap: (" << truck[i].id() <<" , " << truck[j].id() << ")= "
                   << "Delta TravelTime: " << deltaTravelTime;
#endif
        ++count;
        //continue;
//if (count == 5) assert(true==false);
      }
    }  // for j
  }  // for i
  DLOG(INFO) << "count:" << count;
  if (count > 0) {
    // truck.tau();
    if (osrmi->getUse() == true) return;
    IntraSwMoves(truck);
  }
}

void TruckManyVisitsDump::InsMoves(Vehicle &truck) {
  DLOG(INFO) << "starting InsMoves";
  double deltaTravelTime;
  Trashnode deletedNode;
  int count = 0;
  for (POS i = 1; i < truck.size()-1; ++i) {
    for (POS j = i + 1; j < truck.size()-1; ++j) {
      if (i == j) continue;
      if ((i + 1)== j) continue;
      deltaTravelTime = e_evalIns(truck,i,j);  // i adelante de j
      if (deltaTravelTime < 0) {
        ++count;
#if 0
        DLOG(INFO) << "delete: " << i <<" insert after: " << j << " = "
                   << "Delta TravelTime: " << deltaTravelTime;
#endif
        deletedNode = truck[i];
        truck.e_remove(i);
        truck.e_insert(deletedNode,j);
        //assert(true==false);
        continue;
      }
#if 1
      deltaTravelTime = e_evalIns(truck,j,i); // j after i
      if (deltaTravelTime == VRP_MAX()) continue;
      if (deltaTravelTime < 0) {
        ++count;
#if 0
        DLOG(INFO) << "delete: " << j <<" insert after: " << i << " = "
                   << "Delta TravelTime: " << deltaTravelTime;
#endif
        deletedNode = truck[i];
	truck.e_remove(i);
        truck.e_insert(deletedNode,j);
        continue;
      }
#endif
    }  // for j
  }  // for i
#if 1
  DLOG(INFO) << "count:" << count;
  if (count > 0) {
    truck.tau();
//    osrmi->useOsrm(!osrmi->getUse());
//    InsMoves(truck);
  }
#endif
}

/// deleting node at position i, insert it after node in position j

// 0 ..... i-1  i  i+1 ...       j-1 j  j+1
// 0 ......i-1 i+1 ......    j-1  j  i  j+1    

double TruckManyVisitsDump::e_evalIns(Vehicle &truck, POS i, POS j){
  assert(i > 0);
  assert(j > 0);
  assert(j < truck.size()-1);
  assert(i < truck.size()-1);

  if (truck[i].isDump())  // not deleting a dump
      return VRP_MAX();
  if ((j + 1) == i )  // i is already after j
      return 0;
  double deltaTravelTime;
   // if (j > i + 1) {
    double originalTravelTimeI = twc->TravelTime(truck[i-1], truck[i], truck[i+1]);
    double originalTravelTimeJ = twc->TravelTime(truck[j-1], truck[j], truck[j+1]);
    double newTravelTimeI = twc->TravelTime(truck[i-1], truck[i+1]);
    double newTravelTimeJ = twc->TravelTime(truck[j-1], truck[j], truck[i], truck[j+1]);
    deltaTravelTime = (newTravelTimeI + newTravelTimeJ)
                   - (originalTravelTimeI + originalTravelTimeJ);

#ifdef VRPMAXTRACE
    DLOG(INFO) << std::fixed << std::setprecision(4);
    DLOG(INFO) << "travelTime (" << i-1 <<", " << i << ", " << i + 1 << ")= "
               << originalTravelTimeI;
    DLOG(INFO) << "travelTime (" << j-1 <<", " << j << ", " << j + 1 << ")= "
               << originalTravelTimeJ;
    DLOG(INFO) << "travelTime (" << i-1 <<", " << i + 1 << ")= "
               << newTravelTimeI;
    DLOG(INFO) << "travelTime (" << j-1 <<", " << j << ", "<< i << ", " << j + 1 << ")= "
               << newTravelTimeJ;
    DLOG(INFO)  << "Delta TravelTime: " << deltaTravelTime;
#endif
#if 0
  } else { // i-1 (i == j-1) (i + 1 == j) j+1 son contiguos
    double originalTravelTime = twc->TravelTime(truck[i-1], truck[i], truck[j], truck[j+1]);
    double newTravelTime = twc->TravelTime(truck[i-1], truck[j], truck[i], truck[j+1]);
    deltaTravelTime = newTravelTime - originalTravelTime;
#ifdef VRPMAXTRACE
    DLOG(INFO) << std::fixed << std::setprecision(4);
    DLOG(INFO) << "travelTime (" << i-1 <<" , " << i << " , " << j << " , " << j + 1 << ")= "
               << originalTravelTime;
    DLOG(INFO) << "travelTime (" << i-1 <<" , " << j << " , " << i << " , " << j + 1 << ")= "
               << newTravelTime;
#endif

  }
#endif
  return deltaTravelTime;
};





void TruckManyVisitsDump::fillOneTruck(
         Vehicle &truck,       // truck to be filled
         Bucket &unassigned,   // unassigned containers
         Bucket &assigned ) {  // assigned containers
  
  // nothing left to be assigned
  if (unassigned.size() == 0) return;

  Trashnode bestNode;
  UID bestPos;
  double bestTime;
  Bucket streetNodes;
  Bucket unassignedStreetNodes;
  Bucket aux;
  uint64_t  street_id;
  bool first = true;
  

  while (unassigned.size() != 0) {
    // find a costly node
    if (truck.findFastestNodeTo(true, unassigned, bestPos, bestNode, bestTime)) {
      aux.clear();
      aux.push_back(bestNode);
      // of the costly node find the cheaper position
      truck.findFastestNodeTo(false, aux, bestPos, bestNode, bestTime);
      
DLOG(INFO) << "1) inserting: " << bestNode.id()  << "\tfrom street: " << bestNode.streetId() << "\t time:" <<bestTime;
      truck.e_insert(bestNode, bestPos);
   truck.tau();
      assigned.push_back(bestNode);
      unassigned.erase(bestNode);

      // get containers that are in the path
      twc->getNodesOnPath(truck.Path(), truck.getDumpSite(), unassigned, streetNodes);

      while (streetNodes.size() != 0) {
        aux.clear();
        aux.push_back(streetNodes[0]);
        if (truck.findFastestNodeTo(false, aux, bestPos, bestNode, bestTime) ) {
        // insert only nodes that dont change the structure of the path ???  some nodes change
DLOG(INFO) << "2) inserting: " << bestNode.id()  << "\tfrom street: " << bestNode.streetId() << "\t time:" <<bestTime;
          truck.e_insert(bestNode, bestPos);
          assigned.push_back(bestNode);
truck.tau();
          unassigned.push_back(bestNode);
        }
        streetNodes.erase(bestNode);
      }
    // first = false;
    } else break;
  }
assert(true==false);

#if 0
      // store same street
      street_id = bestNode.streetId();
      unsigned int i = 0;
      while (i < unassigned.size()) {
        if (unassigned[i].streetId() == street_id) {
          streetNodes.push_back(unassigned[i]);
          unassigned.erase(unassigned[i]);
        } else i++;
      }
  assert(streetNodes.size() == 0);
  if (unassigned.size() != 0) fillOneTruck(truck, unassigned, assigned);
#endif

#if 0
      while (streetNodes.size() != 0) {
        if (truck.findFastestNodeTo(streetNodes, bestPos, bestNode, bestTime)) {
DLOG(INFO) << "3) inserting: " << bestNode.id()  << "\tfrom street: " << bestNode.streetId() << "\t time:" <<bestTime;
          truck.e_insert(bestNode, bestPos);
    truck.tau();
          //truck.e_adjustDumpsToNoCV(bestPos);
          assigned.push_back(bestNode);
          streetNodes.erase(bestNode);
          // unassigned.erase(bestNode);
        }
      }
#endif



#if 0
assert(true==false);
  // add unassigned nodes at end
  while (unassigned.size() != 0) {
      bestNode = unassigned[0];
      truck.push_back(bestNode);
      assigned.push_back(bestNode);
      unassigned.erase(bestNode);
assert(true==false);
  }
#endif
}



Vehicle  TruckManyVisitsDump::getTruck()
{
  Vehicle truck = unusedTrucks[0];
  unusedTrucks.erase( unusedTrucks.begin() );
  usedTrucks.push_back( truck );
  return truck;
}


//    PROCESS
//
//    This implements a feasable solution

void TruckManyVisitsDump::process()
{
  // THE INVARIANT
  // union must be pickups
#ifdef VRPMAXTRACE
  assert(pickups == (unassigned + problematic + assigned));
  // all intersections must be empty set
  assert(!(unassigned * problematic).size());
  assert(!(unassigned * assigned).size());
  assert(!(problematic * assigned).size());
  //END INVARIANT
#endif
DLOG(INFO) << "Starting initial Solution Proccess\n";

  // preparing a big truck where to store everything
  osrmi->useOsrm(true);
  Vehicle bigTruck = getTruck();
  bigTruck.getCost();
  fillOneTruck(bigTruck, unassigned, assigned);
  //osrmi->useOsrm(false);
  //InsMoves(bigTruck);
  bigTruck.evaluate();
STATS->dump(" Dump 1 ");
#if 1
DLOG(INFO) << "evaluation before updating tables";
  bigTruck.evaluate();
  bigTruck.getCost();
  bigTruck.dumpCostValues();

  bigTruck.e_adjustDumpsToNoCV(1);
  bigTruck.getCost();
  bigTruck.dumpCostValues();

 osrmi->useOsrm(true);
 for (int i = 0 ; i < 2; ++i) {
  osrmi->useOsrm(true);
  IntraSwMoves(bigTruck);
  bigTruck.evaluate();
  //InsMoves(bigTruck);
  //bigTruck.evaluate();
  bigTruck.getCost();
  bigTruck.dumpCostValues();
  // bigTruck.tau();
}

assert(true==false);
#endif 
  //bigTruck.e_makeFeasable(0);
  for (int i = 0; i < 20; i++) {
  osrmi->useOsrm(true);
  IntraSwMoves(bigTruck);
  osrmi->useOsrm(false);
   InsMoves(bigTruck);
  // bigTruck.tau();
  }
  bigTruck.e_makeFeasable(0);
  bigTruck.evaluate();
  bigTruck.dumpCostValues();
  bigTruck.tau();
  STATS->dump("intermidiate");

assert(true==false);
  IntraSwMoves(bigTruck);
  osrmi->useOsrm(true);
  IntraSwMoves(bigTruck);
  InsMoves(bigTruck);
  osrmi->useOsrm(false);
  IntraSwMoves(bigTruck);

  bigTruck.getCost();
  bigTruck.dumpCostValues();
#ifdef DOSTATS
    STATS->dump("intermidiate");
#endif
  // adding the dumps
  bigTruck.e_makeFeasable(0);

  bigTruck.getCost();
  bigTruck.dumpCostValues();

  //repeat the same process
  for (int i = 0; i < 20; i++) {
  osrmi->useOsrm(!osrmi->getUse());
  IntraSwMoves(bigTruck);
  bigTruck.tau();
  }

  osrmi->useOsrm(true);
  IntraSwMoves(bigTruck);
  osrmi->useOsrm(false);
  IntraSwMoves(bigTruck);

  bigTruck.getCost();
  bigTruck.dumpCostValues();
#ifdef DOSTATS
    STATS->dump("intermidiate");
#endif
  assert(true==false);


#if 0
  bigTruck.evaluate();
bigTruck.tau();
DLOG(INFO) << "first round";
  //InsMoves(bigTruck);
DLOG(INFO) << "second round";
  osrmi->useOsrm(false);
  IntraSwMoves(bigTruck);
  //InsMoves(bigTruck);
bigTruck.tau();
assert(true==false);
  assert(fleet.size());
#ifdef DOVRPLOG
  fleet[0].dump( "fleet[0]" );
#endif
  bigTruck = fleet[0].Path();
  fleet.clear();
  bigTruck.dump( "bigTruck" );
  int goingPos = 1;
  Vehicle truck;
  truck = getTruck();
#ifdef DOVRPLOG
  truck.dump( "Truck" );
#endif
  insertGoing( bigTruck, truck, goingPos );
  fleet.push_back( truck ); //need to save the last truck ??


  //truck.plot( "truckManyVisitsDump-", "Many Visits", truck.getVid() );
  return;
#endif
}
