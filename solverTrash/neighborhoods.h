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
insTruckPos1=0;
insTruckPos2=1;
 }; 

    bool isNotFeasible(const Move& m) const ;
    double getMoveSavings(const Move& m) const ;
    int clearRelatedMoves(std::deque<Move>& moves, const Move& lastMove)  const;
    int addRelatedMovesIns(std::deque<Move>& moves, const Move& lastMove)  const;
    int addRelatedMovesIntraSw(std::deque<Move>& moves, const Move& lastMove)  const;
    int addRelatedMovesInterSw(std::deque<Move>& moves, const Move& lastMove)  const;
    void getInsNeighborhood(std::deque<Move>& moves, const Move& lastMove) const ;
    void getIntraSwNeighborhood(std::deque<Move>& moves, const Move& lastMove) const;
    void getInterSwNeighborhood(std::deque<Move>& moves, const Move& lastMove) const;

    void v_getIntraSwNeighborhood(std::deque<Move>& moves, double factor) const;
    void v_getInsNeighborhood(std::deque<Move>& moves,double factor) const ;
    void v_getInsNeighborhood(std::deque<Move>& moves,double factor, int count) const;
    void v_getInterSwNeighborhood(std::deque<Move>& moves, double factor) const;

    void applyMove(const Move&);
    bool applyInsMove( const Move &move);
    bool applyIntraSwMove( const Move &move);
    bool applyInterSwMove( const Move &move);

private:
   mutable int intraTruckPos;
   mutable int interTruckPos1;
   mutable int interTruckPos2;
   mutable int insTruckPos1;
   mutable int insTruckPos2;

};

#endif
