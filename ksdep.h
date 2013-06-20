#include<iostream>
#include<map>
#include<string>
#include<sstream>
#include<vector>
#include<queue>
#include<fstream>
#include<iostream>

#include "ksutil.h"

using namespace std;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// item: a class for elements in the stack or the input list in 
// stack-based parsing algorithms.
//
// goldin and goldout are the number of incoming and outgoing arcs,
// according to the training data (not used at test time).
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

class item {
 public:
  
  string word;
  string pos;
  string lemma;
  string morph;
  string cpos;
  int idx;
  int link;
  string label;
  int goldlink;
  string goldlabel;
  double score;
  int goldin;
  int goldout;
  int lch;
  int rch;
  int nch;
  int nlch;
  int nrch;

  item(): 
    word( "_" ), 
    pos( "_" ), 
    lemma( "_" ), 
    morph( "_" ), 
    cpos( "_" ), 
    idx( 0 ),
    score( 0.0 ),
    link( 0 ),
    label( "_" ),
    goldlink( 0 ),
    goldlabel( "_" ),
    goldin( 0 ),
    goldout( 0 ),
    lch( 0 ),
    rch( 0 ), 
      nch( 0 ), 
      nlch( 0 ),
      nrch( 0 )  {
  }

  item( const item &it ): 
    word( it.word ), 
    pos( it.pos ), 
    lemma( it.lemma ), 
    morph( it.morph ), 
    cpos( it.cpos ), 
    idx( it.idx ),
    score( it.score ),
    link( it.link ),
    label( it.label ),
    goldlink( it.goldlink ),
    goldlabel( it.goldlabel ),
    goldin( it.goldin ),
    goldout( it.goldout ),
    lch( it.lch ),
    rch( it.rch ),
      nch( it.nch ),
      nlch( it.nrch ),
      nrch( it.nlch ) {
  } 
    
  void print() const {
    cout << idx << "\t";
    cout << word << "\t";
    cout << lemma << "\t";
    cout << cpos << "\t";
    cout << pos << "\t";
    cout << morph << "\t";
    cout << link << "\t";
    cout << label << "\n";
  }
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// parserstate: sortable parser states
//
// A parser state includes a stack (s), a queue (q), a score (score),
// a previous action (prevact), and a number of actions in the
// derivation so far (numacts).
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

class parserstate {
 public:

  item dummyitem;
  vector<item> inputq;
  vector<int> s;

  vector<int> outlinks;
  vector<string> outlabels;
  
  double score;
  int numacts;
  string prevact;
  int i;

  parserstate(): 
    score( 0.0 ), 
    numacts( 0 ), 
    prevact( "_" ),
    i( 0 ) {
  }

  parserstate( vector<item> inq ):
    inputq( inq ),
    score( 0.0 ),
    numacts( 0 ),
    prevact( "_" ),
    i( 0 ) {
  }
 
  parserstate( const parserstate &ps ):
    inputq( ps.inputq ),
    s( ps.s ),
    outlinks( ps.outlinks ),
    outlabels( ps.outlabels ),
    score( ps.score ),
    numacts( ps.numacts ),
    prevact( ps.prevact ),
    i( ps.i ) {
  }
 
  bool operator<( const parserstate &right ) const {
    return score < right.score;
  }

  /////////////////////
  // Actions
  /////////////////////

  // SHIFT
  int shift() {

    if( i > ( inputq.size() - 2 ) ) {
      // there is only the RIGHTWALL left, 
      // so we can't shift
      return 0;
    }
    
    s.push_back( inputq[i].idx );
    i++;
  
    prevact = "S";
    numacts++;

    return 1;
  }

  // REDUCE-LEFT
  int reduceleft( string &label ) {
    
    if( s.size() < 2 ) {
      // we need at least two items to reduce
      return 0;
    }

    int itright = s.back();
    int itleft = s[s.size() - 2];

    inputq[itright].link = itleft;
    inputq[itright].label = label;
    inputq[itright].goldout--;
    inputq[itleft].goldin--;
    inputq[itleft].rch = itright;
    inputq[itleft].nrch++;
    inputq[itleft].nch++;

    s.pop_back();
  
    prevact = "L-" + label;
    numacts++;
    
    return 1;
  }

  // REDUCE-RIGHT
  int reduceright( string &label ) {
    
    if( s.size() < 2 ) {
      // we need at least two items to reduce
      return 0;
    }

    int itright = s.back();
    int itleft = s[s.size() - 2];

    inputq[itleft].link = itright;
    inputq[itleft].label = label;
    inputq[itleft].goldout--;
    inputq[itright].goldin--;
    inputq[itright].lch = itleft;
    inputq[itright].nlch++;
    inputq[itright].nch++;

    s.pop_back();
    s.pop_back();
    s.push_back( itright );
  
    prevact = "R-" + label;
    numacts++;

    return 1;
  }

  ///////////////////////////
  // Access stack and queue
  ///////////////////////////

  item* getst( int si ) {
    
    si = s.size() - si;

    if( si < 0 ) {
      return &dummyitem;
    }

    if( si > ( s.size() - 1 ) ) {
      return &dummyitem;
    }

    return &( inputq[s[si]] );
  }

  item* getq( int qi ) {
    
    qi = i + qi - 1;

    if( qi < 0 ) {
      return &dummyitem;
    }
    
    if( qi > ( inputq.size() - 1 ) ) {
      return &dummyitem;
    }

    return &( inputq[qi] );
  }

};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// 
// pact: a simple class for sortable parser actions
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

class pact {
public:
  string label;
  double score;
  
  pact( double p, string lab ) { label = lab; score = p; }
  
  bool operator<( const pact & right ) const {
    return score < right.score;
  }
};

