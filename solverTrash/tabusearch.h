#ifndef TABUSEARCH_H
#define TABUSEARCH_H

#include <map>
#include <cassert>
#include <cstdlib>

#include "trashstats.h"
#include "timer.h"
#include "move.h"
#include "neighborhoods.h"

class TabuSearch {

   typedef enum { Ins, IntraSw, InterSw } neighborMovesName;

  private:
    std::map<const Move, int> TabuList;

    int tabuLengthIns;
    int tabuLengthIntraSw;
    int tabuLengthInterSw;

    bool allTabu;
    Move bestMoveAllTabu;

    int maxIteration;
    int currentIteration;
    mutable int currentIterationIns;
    mutable int currentIterationIntraSw;
    mutable int currentIterationInterSw;

    Neighborhoods bestSolution;
    double bestSolutionCost;

    Neighborhoods currentSolution;

  public:
    TabuSearch(const Neighborhoods initialSolution) :
        bestSolution(initialSolution), currentSolution(initialSolution)
    {
        bestSolution.computeCosts();
        bestSolution.dump();
        bestSolutionCost = bestSolution.getCost();
        currentIteration = currentIterationIns = currentIterationIntraSw = currentIterationInterSw = 0;
        maxIteration = 1000;
        int ncnt = initialSolution.getNodeCount() / 5;
        tabuLengthIns     = std::max(5, std::min(ncnt, 40));
        tabuLengthIntraSw = std::max(5, std::min(ncnt, 15));
        tabuLengthInterSw = std::max(5, std::min(ncnt, 10));

        STATS->set("tabu Length Ins", tabuLengthIns);
        STATS->set("tabu Length IntraSw", tabuLengthIntraSw);
        STATS->set("tabu Length InterSw", tabuLengthInterSw);

        // for repeatible results set this to a constant value
        // for more random results use: srand( time(NULL) );
        srand(37);
	limitIntraSw=bestSolution.getFleetSize();
	limitInterSw=limitIntraSw*(limitIntraSw-1) ;
	limitIns=limitIntraSw ;
    };

    int getCurrentIteration() const { return currentIteration; };
    int getMaxIteration() const { return maxIteration; };
    int getTabuLengthIns() const { return tabuLengthIns; };
    int getTabuLengthIntraSw() const { return tabuLengthIntraSw; };
    int getTabuLengthInterSw() const { return tabuLengthInterSw; };
    Solution getBestSolution() const { return bestSolution; };
    Solution getCurrentSolution() const {return currentSolution; };
    void dumpTabuList() const;
    void dumpStats() const;
    bool isTabu(const Move& m) const;

    void makeTabu(const Move m);
    void cleanExpired();

    void setMaxIteration(int n) { assert(n>0); maxIteration = n; };
    void settabuLengthIns(int n) { assert(n>0); tabuLengthIns = n; };
    void settabuLengthIntraSw(int n) { assert(n>0); tabuLengthIntraSw = n; };
    void settabuLengthInterSw(int n) { assert(n>0); tabuLengthInterSw = n; };

    void search();
    void generateNeighborhoodStats(std::string mtype, double tm, int cnt) const;
    void generateNeighborhood(neighborMovesName whichNeighborhood, std::deque<Move>& neighborhood, const Move& lastMove) const;
    bool doNeighborhoodMoves(neighborMovesName whichNeighborhood, int maxStagnation);


    void v_search();
    bool v_doNeighborhoodMoves(neighborMovesName whichNeighborhood, int maxCnt, std::deque<Move> notTabu, std::deque<Move> tabu);
    void v_getNeighborhood(neighborMovesName whichNeighborhood,std::deque<Move> &neighborhood,double factor) const;
    bool applyAspirational(std::deque<Move> &neighborhood, std::deque<Move> &notTabu,std::deque<Move> &tabu);
    bool applyNonTabu (std::deque<Move> &notTabu);
    bool applyTabu (std::deque<Move> &tabu);
    bool applyTabu (std::deque<Move> &tabu, int strategy);

    private:
	int limitIntraSw;
	int limitInterSw;
	int limitIns;

};

#endif

