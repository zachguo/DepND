#ifndef __KSUTIL_H_
#define __KSUTIL_H_

#include<iostream>
#include<map>
#include<string>
#include<sstream>
#include<vector>
#include<fstream>
#include<iostream>

using namespace std;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// stringTo and toString: string conversion functions
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

template<typename T> const T stringTo(const std::string &s) {
  std::istringstream iss(s);
  T x;
  iss >> x;
  return x;
}

template<typename T> std::string toString(const T& x) {
  std::ostringstream oss;
  oss << x;
  return oss.str();
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// Tokenize: a simple tokenizer
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

int Tokenize( string str, vector<string> &tokens, string sep = " " ) {
  int p = 0;
  int p2 = 0;

  tokens.clear();

  p = str.find_first_not_of( sep, 0 );

  while ( ( p = str.find_first_not_of( sep, p2 ) ) >= 0 ) {
    p2 = str.find_first_of( sep, p );
    if ( p2 < 0 ) { 
      p2 = str.length();
    }
    tokens.push_back( str.substr( p, p2 - p ) );
  }
  
  return tokens.size();
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// ksopts: a class to deal with command line options
//
// Usage example:
//
//  ksopts opts( ":cs 2.0 t 0 :blah something :m tmp.mod", argc, argv );
//  double cost;
//  opts.optset( "cs", cost );
//
//  If -cs (or --cs) is not present in the command line options,
//  the variable (double)cost will have the default value of 2.0.
//
//  The collon before the name of the parameter indicates that this
//  option will be followed by a value in the command line.
//
//  Anything in the command line that is not an option or a value for
//  an option will be stored in vector<string> args.
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

class ksopts {
public:
  
  map<string, int> argopts;
  map<string, string> opts;
  map<string, string> optshelp;
  vector<string> args;
  
  ksopts( string str, int argc, char **argv ) {

    vector<string> strvec;
    Tokenize( str, strvec );
    
    for( int i = 0; i < strvec.size(); i += 2 ) {
      if( strvec[i][0] == ':' ) {
	argopts[strvec[i].substr( 1 )] = 2;
	opts[strvec[i].substr( 1 )] = strvec[i+1];
      }
      else {
	argopts[strvec[i]] = 1;
	opts[strvec[i]] = strvec[i+1];
      }
    }

    for( int i = 1; i < argc; i++ ) {
      string tmpstr( argv[i] );

      if( argv[i][0] == '-' ) {
	int p = tmpstr.find_first_not_of( "-", 0 );
	tmpstr = tmpstr.substr( p );

	if( argopts[tmpstr] == 2 ) {
	  if( i < ( argc - 1 ) ) {
	    opts[tmpstr] = argv[++i];
	  }
	  continue;
	}
	else if( argopts[tmpstr] == 1 ) {
	  opts[tmpstr] = "1";
	  continue;
	}
      }
      
      args.push_back( argv[i] );
    }
  }

  int printopts() {
    for( map<string, string>::iterator it = opts.begin();
	 it != opts.end(); it++ ) {
      cout << it->first << " : " << it->second << endl;
    }

    cout << "Arguments: " << endl;
    for( int i = 0; i < args.size(); i++ ) {
      cout << args[i] << endl;
    }
    
    return 0;
  }

  int printhelp() {
    for( map<string, string>::iterator it = optshelp.begin();
         it != optshelp.end(); it++ ) {
      cerr << it->first << "\t" << it->second << endl;
    }

    return 0;
  }

  template<typename T> int optset( const string &opt, T& val ) {
    if( opts.find( opt ) != opts.end() ) {
      val = stringTo<T> ( opts[opt] );
      return 1;
    }
    return 0;
  }

  template<typename T> int optset( const string &opt, T& val, const string &hstr ) { 
    if( opts.find( opt ) != opts.end() ) {
      val = stringTo<T> ( opts[opt] );
      optshelp[opt] = hstr;
      return 1;
    }
    return 0;
  }

}; 



#endif
