#ifndef NEIGHBORHOODS_H
#define NEIGHBORHOODS_H

#include "solution.h"
#include "move.h"

class Neighborhoods : public Solution {

public:

  Neighborhoods(const Solution &solution): Solution(solution){
intraTruckPos=0;
interTruckPos1=0;
interTruckPos2=1;
insTruckPos1=fleet.size()-1;
insTruckPos2=0;
 }; 

    bool isNotFeasible(const Move& m) const ;
    double getMoveSavings(const Move& m) const ;
    void getInsNeighborhood(std::deque<Move>& moves) const ;
    void getIntraSwNeighborhood(std::deque<Move>& moves) const;
    void getInterSwNeighborhood(std::deque<Move>& moves) const;

    void v_getIntraSwNeighborhood(std::deque<Move>& moves, double factor) const;
    void v_getInsNeighborhood(std::deque<Move>& moves,double factor) const ;
    void v_getInsNeighborhood(std::deque<Move>& moves,double factor, int count) const;
    void v_getInterSwNeighborhood(std::deque<Move>& moves, double factor) const;

    void applyMove(const Move&);
    bool applyInsMove( const Move &move);
    bool applyIntraSwMove( const Move &move);
    bool applyInterSwMove( const Move &move);

    bool v_applyInsMove( const Move &move);
    void v_applyMove(const Move&);
private:
   mutable int intraTruckPos;
   mutable int interTruckPos1;
   mutable int interTruckPos2;
   mutable int insTruckPos1;
   mutable int insTruckPos2;

};

#endif
