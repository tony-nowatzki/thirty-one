#ifndef CARD_HH
#define CARD_HH

#include <sstream>
#include <string>
using namespace std;


class Card {
  int _suit=0;
  int _num=0;

public:
  Card(int s, int n) {
    _suit=s;
    _num=n;
    assert(_num >=1 && _num <=13);
  }

  int val() const {
    assert(_num >=1 && _num <=13);
    if((_num) ==  1) {
      return 11;
    } else if(_num > 10) {
      return 10;
    } else {
      return _num;
    }
  }

  int suit() const {return _suit;}
  int num()  const { return _num;}

  string name() const {
    stringstream ss;
    assert(_num >=1 && _num <=13);

    if(_num==1) {
      ss << " A";
    } else if(_num==11){
      ss << " J";
    } else if(_num==12) {
      ss << " Q";
    } else if(_num==13) {
      ss << " K";
    } else if(_num==10) {
      ss << _num;
    } else {
      ss << " " << _num;
    }

    switch(_suit) {
      case 0: ss << "\u2665";   break;
      case 1: ss << "\u2663";    break;
      case 2: ss << "\u2666"; break;
      case 3: ss << "\u2660";   break;
      default: assert(0);
    }
    return ss.str();
  }
}; 

#endif
