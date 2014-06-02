#ifndef HAND_HH
#define HAND_HH

#include "card.hh"

class Hand {
  set<Card*> _cards;
 
public: 
  static std::pair<int,int> val_of_hand(set<Card*> cards) {
    map<int,int> val_of_suit;
    int highest_num=0;
    int highest_suit=-1;
    for(Card* card : cards) {
      val_of_suit[card->suit()]+=card->val();
      if(val_of_suit[card->suit()] > highest_num) {
        highest_suit=card->suit();
        highest_num=val_of_suit[card->suit()];
      }
    }
    return make_pair(highest_suit,highest_num);
  }

  std::pair<int,int> val_of_hand() const {
    return val_of_hand(_cards);
  }

  int score() const {
    return score(_cards);
  }
  static int score(set<Card*> const& cards) {
    auto p = val_of_hand(cards);
    return p.second;  
  }

  void add_card(Card* card) {
    _cards.insert(card);
  }
  void remove_card(Card* card) {
    _cards.erase(card);
  }
  
  int size() const {
    return _cards.size();
  }

  Card* worst() const {
    return worst(_cards);
  }

  static Card* worst(std::set<Card*> const& cards) {
    auto p = val_of_hand(cards);
    
    Card* lowest_card=NULL;
    Card* lowest_suited=NULL;
    for(Card* card : cards) {
      if(p.first != card->suit()) {
        if(lowest_card == NULL || lowest_card->val() > card->val()) {
          lowest_card = card;
        }
      } else {
        if(lowest_suited == NULL || lowest_suited->val()> card->val()) {
          lowest_suited = card;
        }
      }
    }
    if(lowest_card) {
      return lowest_card;
    } 
    assert(lowest_suited);
    return lowest_suited;
  }

  std::string print_hand() {
    stringstream ss;
    for(Card* card : _cards) {
      ss << card->name() << ", ";
    }
    return ss.str();
  }

  void reset() {
    _cards.clear();
  }

  std::set<Card*>::const_iterator cards_begin() const {return _cards.begin();}
  std::set<Card*>::const_iterator cards_end()   const {return _cards.end();}
};

#endif
