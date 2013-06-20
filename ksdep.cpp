#include "ksdep.h"
#include "maxent.h"
#include "common.h"

#define DEBUG 0
#define PRINTFEATS 0

// global variables for parsing
// (Initial values are irrelevant and will
// be overwritten in main())

using namespace std;

string bidir_postag(const string & s, const vector<ME_Model> & vme, const vector<ME_Model> & cvme, bool dont_tokenize);
void bidir_chunking(vector<Sentence> & vs, const vector<ME_Model> & vme);
void init_morphdic();

extern void load_ne_models();

double LENBEAMFACTOR = 0;
double ACTCUTOFF = 0;
int NUMACTCUTOFF = 0;
int MAXSTATES = 0;

int TRAIN = 0;
item dummyitem;

extern int NUMITER;

// input: a model, a feature vector, a class label
//
// Add training instance -- training is NOT done here,
// just adding the example
int me_train( ME_Model& memod, vector<string>& fvec, string c ) {
  ME_Sample sptr;

  sptr.label = c;
  for( int i = 0; i < fvec.size(); i++ ) {
    sptr.features.push_back( fvec[i] );
  }

  memod.add_training_sample( sptr );

  return 0;
}

// input: a model, a feature vector
// output: a vector containing probabilities for each class
vector<double> me_classify( ME_Model& memod, vector<string>& fvec ) {
  vector<double> res;
  ME_Sample sptr;

  for( int i = 0; i < fvec.size(); i++ ) {
    sptr.features.push_back( fvec[i] );
  }

  res = memod.classify( sptr );

  return res;
} 

// simple discretizer for numerical features
int discr( int n ) {
  if( n > 6 ) return 7;
  if( n > 3 ) return 4;
  return n;
}

// input: a parser state, a vector that will contain features
//
// This fills the given vector with features corresponding
// to the given parser state
int makefeats( parserstate& pst, vector<string>& fv ) {

  fv.clear();

  item *s1 = pst.getst( 1 );
  item *s2 = pst.getst( 2 );
  item *s3 = pst.getst( 3 );
  
  item *q1 = pst.getq( 1 );
  item *q2 = pst.getq( 2 );
  item *q3 = pst.getq( 3 );

  int dist1 = s1->idx - s2->idx;
  int dist2 = q1->idx - s1->idx;

  // distance between s1 and s2
  fv.push_back( toString( fv.size() ) + "~" + toString( dist1 ) );
  fv.push_back( toString( fv.size() ) + "~" + toString( dist2 ) );
  fv.push_back( toString( fv.size() ) + "~" + toString( dist1 ) + "." + toString( dist2 ) );

  // pos between s1 and s2, if any
  if( dist1 > 1 ) {
    fv.push_back( toString( fv.size() ) + "~" + pst.getq( pst.getst( 1 )->idx - 1 )->pos );
  }
  else {
    fv.push_back( toString( fv.size() ) + "~" + "NONE" );
  }

  if( dist1 > 2 ) {
    fv.push_back( toString( fv.size() ) + "~" + pst.getq( pst.getst( 2 )->idx + 1 )->pos );
  }
  else {
    fv.push_back( toString( fv.size() ) + "~" + "NONE" );
  }

  //fv.push_back( toString( fv.size() ) + "~" + toString( s1->idx - s2->idx ) );
  //fv.push_back( toString( fv.size() ) + "~" + toString( q1->idx - s1->idx ) );

  fv.push_back( toString( fv.size() ) + "~" + s1->word );
  fv.push_back( toString( fv.size() ) + "~" + s1->pos );
  fv.push_back( toString( fv.size() ) + "~" + s1->cpos );
  fv.push_back( toString( fv.size() ) + "~" + s1->lemma );
  fv.push_back( toString( fv.size() ) + "~" + s1->morph );
  fv.push_back( toString( fv.size() ) + "~" + s1->label );
  fv.push_back( toString( fv.size() ) + "~" + pst.getq( s1->lch )->pos );
  fv.push_back( toString( fv.size() ) + "~" + pst.getq( s1->lch )->label );
  fv.push_back( toString( fv.size() ) + "~" + pst.getq( s1->rch )->pos );
  fv.push_back( toString( fv.size() ) + "~" + pst.getq( s1->rch )->label );
  

  fv.push_back( toString( fv.size() ) + "~" + s2->word );
  fv.push_back( toString( fv.size() ) + "~" + s2->pos );
  fv.push_back( toString( fv.size() ) + "~" + s2->cpos );
  fv.push_back( toString( fv.size() ) + "~" + s2->lemma );
  fv.push_back( toString( fv.size() ) + "~" + s2->morph );
  fv.push_back( toString( fv.size() ) + "~" + s2->label );
  fv.push_back( toString( fv.size() ) + "~" + pst.getq( s2->lch )->pos );
  fv.push_back( toString( fv.size() ) + "~" + pst.getq( s2->lch )->label );
  fv.push_back( toString( fv.size() ) + "~" + pst.getq( s2->rch )->pos );
  fv.push_back( toString( fv.size() ) + "~" + pst.getq( s2->rch )->label );

  fv.push_back( toString( fv.size() ) + "~" + s3->word );
  fv.push_back( toString( fv.size() ) + "~" + s3->pos );
  fv.push_back( toString( fv.size() ) + "~" + s3->cpos );
  fv.push_back( toString( fv.size() ) + "~" + s3->morph );
  fv.push_back( toString( fv.size() ) + "~" + s3->label );

  fv.push_back( toString( fv.size() ) + "~" + q1->word );
  fv.push_back( toString( fv.size() ) + "~" + q1->pos );
  fv.push_back( toString( fv.size() ) + "~" + q1->cpos );
  fv.push_back( toString( fv.size() ) + "~" + q1->morph );

  fv.push_back( toString( fv.size() ) + "~" + q2->word );
  fv.push_back( toString( fv.size() ) + "~" + q2->pos );
  fv.push_back( toString( fv.size() ) + "~" + q2->cpos );
  fv.push_back( toString( fv.size() ) + "~" + q2->morph );

  fv.push_back( toString( fv.size() ) + "~" + q3->pos );

  int n = fv.size();

  for( int i = 0; i < n; i++ ) {
    fv.push_back( toString( fv.size() ) + "~" + fv[i] + "~" + s1->pos );
    fv.push_back( toString( fv.size() ) + "~" + fv[i] + "~" + s2->pos );
    fv.push_back( toString( fv.size() ) + "~" + fv[i] + "~" + q1->pos );
  }

  fv.push_back( toString( fv.size() ) + "~" + q1->pos + s1->pos + s2->pos );
  fv.push_back( toString( fv.size() ) + "~" + s1->pos + s2->pos + s3->pos );
  fv.push_back( toString( fv.size() ) + "~" + q1->pos + q2->pos + s1->pos + s2->pos );
  fv.push_back( toString( fv.size() ) + "~" + s1->pos + s2->pos + s3->pos + q1->pos );

  fv.push_back( toString( fv.size() ) + "~" + pst.prevact + s1->pos );
  fv.push_back( toString( fv.size() ) + "~" + pst.prevact + s1->pos + s2->pos );
  fv.push_back( toString( fv.size() ) + "~" + pst.prevact + q1->pos );
  fv.push_back( toString( fv.size() ) + "~" + pst.prevact + q1->pos + s1->pos);

  return 0;
}

// input: a vector with the input items, a model, and 
// number of desired output parses
//
// This is where the parsing is done
int parse( vector<item>& inputq, ME_Model &memod, int nparses ) {
 
  int n = inputq.size() - 1;
  vector<double> lenbeam( ( n * 2 ) + 10, 0.0 ); 
  vector<parserstate> psv;
  priority_queue<parserstate> pspq;
  parserstate newpst( inputq );
  newpst.score = 1;
  newpst.shift();
  pspq.push( newpst );

  parserstate currpst;

  while( pspq.size() > 0 ) {

    if( ( pspq.top().i >= n ) && ( pspq.top().s.size() <= 1 ) ) {
      psv.push_back( pspq.top() );
      if( psv.size() >= nparses ) {
	break;
      }
    }

    if( MAXSTATES && ( pspq.size() > ( MAXSTATES * 2 ) ) ) {
      priority_queue<parserstate> tmppq;
      for( int tt = 0; tt < MAXSTATES; tt++ ) {
	tmppq.push( pspq.top() );
	pspq.pop();
      }
      pspq = tmppq;
    }

    currpst = pspq.top();
    pspq.pop();

    if( DEBUG ) cout << "\nGOT PARSER STATE: " << currpst.score << endl;

    string act = "S";
    string label = "NONE";

    // Get the items on top of stack
    item* s1 = currpst.getst( 1 );
    item* s2 = currpst.getst( 2 );

    if( DEBUG ) cout << "S: " << currpst.s.size() << " "
		     << "Q: " << currpst.i << endl;

    if( DEBUG ) cout << "S1: " << s1->word << " IDX: " 
		     << " " << s1->idx << " GOLDIN: " << s1->goldin 
		     << " LINK: " << s1->goldlink << endl;
    if( DEBUG ) cout << "S2: " << s2->word << " IDX: "
		     << " " << s2->idx << " GOLDIN: " << s2->goldin 
		     << " LINK: " << s2->goldlink << endl;


    // Get features from parser state
    vector<string> feats;
    makefeats( currpst, feats );
    vector<double> vp;

    // Choose action
    if( TRAIN ) {

      // is s1 a dependent of s2?
      if( ( s1->goldlink == s2->idx ) && ( s1->goldin == 0 ) && currpst.s.size() > 1 ) {
	act = "L-" + s1->goldlabel;
	label = s1->goldlabel;
      }

      // is s2 a dependent of s1?
      else if( ( s2->goldlink == s1->idx ) && ( s2->goldin == 0 ) && currpst.s.size() > 1 ) {
	act = "R-" + s2->goldlabel;
	label = s2->goldlabel;
      }
      else if( currpst.i < n ) {
	act = "S";
      }
      else {
	act = "F";
      }
      vp.push_back( 1.0 );

      // add training example
      if( !( act == "F" ) ) {
	me_train( memod, feats, act );
      }
	
      if( PRINTFEATS ) {
	cout << act;
	
	for( int i = 0; i < feats.size(); i++ ) {
	  cout << " " << feats[i];
	}
	cout << endl;
      }
    }

    if( !TRAIN ) {
      // classify action
      vp = me_classify( memod, feats );
    }

    // put all possible actions in a priority queue
    priority_queue<pact> actpq;
    for( int i = 0; i < vp.size(); i++ ) {
      if( vp[i] < ACTCUTOFF ) {
	continue;
      }
      if( TRAIN ) {
	actpq.push( pact( vp[i], act ) );
      }
      else {
	actpq.push( pact( vp[i], memod.get_class_label( i ) ) );
	//cout << memod.get_class_label(i) << "\t" << vp[i]<<endl;
      }
    }

    int actcnt = 0;
    while( actpq.size() ) {
      if( NUMACTCUTOFF && ( actcnt == NUMACTCUTOFF ) ) {
	break;
      }
      actcnt++;
      act = actpq.top().label;
      double score = actpq.top().score;
      actpq.pop();

      parserstate npst( currpst );

      if( DEBUG ) cout << "   " << act << " : " << score << endl;

      if( act[0] == 'L' ) {
	if( DEBUG ) cout << ">>> LEFT\n";
	label = act.substr( 2 );
	if( !npst.reduceleft( label ) ) {
	  act = "F";
	  if( DEBUG) cout << "NO!\n";
	}
	else {
	  npst.score *= score;
	}
      }
      else if( act[0] == 'R' ) {
	if( DEBUG ) cout << ">>> RIGHT\n";
	label = act.substr( 2 );
	if( !npst.reduceright( label ) ) {
	  act = "F";
	  if( DEBUG) cout << "NO!\n";
	}
	else {
	  npst.score *= score;
	}
      }
      else if( act == "S" ) {
	if( DEBUG ) cout << ">>> SHIFT\n";
	if( !npst.shift() ) {
	  act = "F";
	  if( DEBUG) cout << "NO!\n";
	}
	else {
	  npst.score *= score;
	}
      }
      else {
	act = "F";
      }
      
      if( act == "F" ) {
	if( DEBUG ) cout << ">>> ACT-FAIL\n";
	continue;
      }
      
      if( DEBUG ) cout << "PUSH NPST\n";
      if( npst.score > ( lenbeam[npst.numacts] * LENBEAMFACTOR ) ) {
	pspq.push( npst );
	if( npst.score > ( lenbeam[npst.numacts] ) ) {
	  lenbeam[npst.numacts] = npst.score;
	}
      }
    }
  }
  
  int errflg = 0;

  // if we are training, don't print out a parse
  if( !TRAIN ) {
    // if we are doing 1-best parsing
    // just use the CoNLL-X output format
    if( nparses == 1 ) {
      for( int i = 1; i < n; i++ ) {
	if( pspq.size() > 0 ) {
	  pspq.top().inputq[i].print();
	}
	else {
	  errflg = 1;
	  currpst.inputq[i].print();
	}
      }
    }
    // n-best output
    else {
      cout << psv.size() << " parses\n";
      for( int j = 0; j < psv.size(); j++ ) {
	cout << psv[j].score << endl;
	for( int i = 1; i < psv[j].inputq.size() - 1; i++ ) {
	  psv[j].inputq[i].print();
	}
	cout << endl;
      }
    }
     
    cout << endl;
  }

  // we didn't find a complete parse
  if( errflg ) {
    return -1;
  }

  // we found at least one complete parse
  return 0;
}
    
main( int argc, char **argv ) {

  string modelname;
  int nparses;
  int heldout;
  int fcutoff;
  int helpmsg;
  double ineq;
  int donttok = 0;

  ME_Model memod;

  // read options
  ksopts opt( ":maxst 10 :numactcut 0 :actcut 0 :b 0.1 :m genia.mod :n 1 help 0 nt 0", argc, argv );
  opt.optset( "help", helpmsg, "Print this help message and exit" );
  opt.optset( "m", modelname, "(string) Model name" );
  opt.optset( "n", nparses, "(int) Number of parses to output (n-best output)" );
  opt.optset( "b", LENBEAMFACTOR, "(float) Beam factor (between 0 and 1, where 0 is no beam, and 1 keeps only locally best path)" );
  opt.optset( "numactcut", NUMACTCUTOFF, "(int) Consider at most this many actions per iteration" );
  opt.optset( "actcut", ACTCUTOFF, "(float) Consider only actions with probability higher or equal to this value" );
  opt.optset( "maxst", MAXSTATES, "(int) Maximum number of parser states in the priority queue" );
  opt.optset( "nt", donttok, "Don't tokenize text");

  if( helpmsg ) {
    cerr << "GDEP\n\nThis is a version of the KSDep parser tuned for biomedical text in the style of the GENIA treebank.  Using \"gdep INPUTFILE\" should suffice.  INPUTFILE should be a text file with one sentence per line.  Output is written to stdout.  See below for more general information on usage.\n\n";

    cerr << "USAGE: gdep [--help][-m MODELNAME][-n NUMPARSES][-b BEAMFACTOR][-numactcut NUMACTS][-actcut ACTPROB][-maxst MAXSTATES] INPUTFILE\n\n";
    cerr << "By default, the parser uses sensible pruning for efficient accurate parsing of GENIA text.\n\n";

    opt.printhelp();
    exit( 0 );
  }

  bool dtok = false;
  if( donttok ) {
    dtok = true;
  }
  
  // open the input file...
  ifstream infile;
  if( opt.args.size() > 0 ) {
    infile.open( opt.args[0].c_str() );

    if( !infile.is_open() ) {
      cerr << "File not found: " << opt.args[0] << endl;
      exit( 1 );
    }
  }

  // ... or use stdin
  istream *istrptr;
  if( infile.is_open() ) {
     istrptr = &infile;
  }
  else {
     istrptr = &std::cin;
  }


  // read the models
  vector<ME_Model> vme(16);
 
  cerr << "Reading parsing model...\n";
  init_morphdic();
  
  // load the maxent model
  if( !memod.load_from_file( modelname ) ) {
    cerr << "Error opening model " << modelname << endl;
    exit( 1 );
  }    
  cerr << "Done.\n";

    cerr << "loading pos_models";
  for (int i = 0; i < 16; i++) {
    char buf[1000];
    sprintf(buf, "./models_medline/model.bidir.%d", i);
    vme[i].load_from_file(buf);
    cerr << ".";
  }
  cerr << "done." << endl;

  cerr << "loading chunk_models";
  vector<ME_Model> vme_chunking(16);
  for (int i = 0; i < 8; i +=2 ) {
    char buf[1000];
    sprintf(buf, "./models_chunking/model.bidir.%d", i);
    vme_chunking[i].load_from_file(buf);
    cerr << ".";
  }
  cerr << "done." << endl;

  load_ne_models();
  
  string str;
  int linenum = 0;
  int sentnum = 0;
  int errnum = 0;

  vector<item> q;
  q.push_back( item() );
  q.back().word = "LeftWall";
  q.back().pos = "LW";
  q.back().idx = 0;
  q.back().goldlink = 0;
  q.back().goldlabel = "LW";
  q.back().link = -1;
  q.back().label = "*NONE*";

  // main loop
  while( getline( *istrptr, str ) ) {

    // increase line counter
    linenum++;
        
    if( str.size() > 1024 ) {
      cerr << "warning: the sentence seems to be too long at line " << linenum;
      cerr << " (please note that the input should be one-sentence-per-line)." << endl;
    }

    // remove new line and carriage return characters
    //if( str.find( "\r", 0 ) != string::npos ) {
    //  str.erase( str.find( "\r", 0 ), 1 );
    // }

    //if( str.find( "\n", 0 ) != string::npos ) {
    //  str.erase( str.find( "\n", 0 ), 1 );
    //}
    
    // clear the queue, and initialize
    q.clear();

    q.push_back( item() );
    q.back().word = "LeftWall";
    q.back().pos = "LW";
    q.back().idx = 0;
    q.back().goldlink = 0;
    q.back().goldlabel = "LW";
    q.back().link = -1;
    q.back().label = "*NONE*";

    string postagged = bidir_postag(str, vme, vme_chunking, dtok);

    vector<string> lines;
    Tokenize( postagged, lines, "\n" );

    for( int lnum = 0; lnum < lines.size(); lnum++ ) {
      
      vector<string> tokens;
      
      Tokenize( lines[lnum], tokens, " \t" );

      // insert the current word in the queue
      string word = tokens[0];
      string lemma = tokens[1];
      string cpos = tokens[3];
      string pos = tokens[2];
      string morph = tokens[4];
      string label = "_";
      int link = 0;
      int idx = lnum + 1;
      
      q.push_back( item() );
      q.back().word = word;
      q.back().lemma = lemma;
      q.back().cpos = cpos;
      q.back().pos = pos;
      q.back().morph = morph;
      q.back().idx = idx;
      q.back().goldlink = link;
      q.back().goldlabel = label;
      q.back().link = -1;
      q.back().label = "*NONE*";
    }

    if( q.size() <= 1 ) {
      continue;
    }
      
    sentnum++;

    if( !( sentnum % 100 ) ) {
      cerr << sentnum << "... ";
    }

    // insert the right wall
    q.push_back( item() );
    q.back().word = "RightWall";
    q.back().pos = "RW";
    q.back().idx = q.size() - 1;
    q.back().goldlink = 0;
    q.back().goldlabel = "RW";
    q.back().link = -1;
    q.back().label = "*NONE*";
    
    if( parse( q, memod, nparses ) == -1 ) {
      cerr << "Sentence " << sentnum << ": parse failed.\n";
      errnum++;
    }    
  } // while( getline(*istrptr, str) );
  
  if( infile.is_open() ) {
    infile.close();
  }
  
  cerr << endl;
  
  return 0; 
}
