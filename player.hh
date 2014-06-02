#ifndef PLAYER_HH
#define PLAYER_HH
#include <sstream>

#include "card.hh"
#include "hand.hh"
#include <set>
#include <vector>

using namespace std;

class Player {

protected:
  int n_turns;

  struct num_sorter_functor {
    bool operator() (const Card* lhs, const Card* rhs) const{
      if(lhs->num() < rhs->num()) {
        return true;
      } else {
        return lhs->suit() < rhs->suit();
      }
    }
  };

  typedef set<Card*,num_sorter_functor> NumSortedSet;

public:
  int _my_id;
  int _num_players;
  set<Card*> _cards_left;
  Hand const* _hand;
  vector<Hand> _hands;

  Player() {
  }

  int id() {
    return _my_id;
  }

  virtual void reset() {}

  static NumSortedSet largestNCardsForSuit(std::set<Card*> cards, int n, int suit) {
    NumSortedSet ret_set;
    if(n==0) {
      return ret_set;
    }
    for(Card* card : cards) {
      if(card->suit() !=suit) {
        continue;
      }
      if(ret_set.size() < n || (*ret_set.begin())->num() < card->num()) {
        ret_set.insert(card);
      }
      if(ret_set.size() > n) {
        ret_set.erase(ret_set.begin());
      }
    }
    return ret_set;
  }

  void initialize(std::set<Card*> const& orig_cards, Hand* hand, int id, int np) {
    _my_id=id;
    _num_players=np;
    //std::copy(orig_cards.begin(),orig_cards.end(),_cards_left.begin()); 
    for(Card* card : orig_cards) {
      _cards_left.insert(card);
    }
    _hand=hand;
    for(auto i=_hand->cards_begin(), e=_hand->cards_end();i!=e;++i) {
      Card* card = *i;
      _cards_left.erase(card);
    }
    _hands.clear();
    _hands.resize(np);
    n_turns=0;
  }

  virtual string player_name() {
    return string("default");
  }

  virtual void inform_turn(int player_id, bool knocked, bool stack, 
                     bool pile, Card* pile_card, Card* drop_card) {
    if(player_id==0) {
      n_turns++;
    }
    // Lets keep track of what are the remaining cards.
    if(pile_card) {
      _cards_left.erase(pile_card);
      _hands[player_id].add_card(pile_card);
    } 
    if(drop_card) {
      _cards_left.erase(drop_card);
      _hands[player_id].remove_card(drop_card);
    }
/*    if(_hands[player_id].size()>=1) {
      cout << this->player_name() <<  " size " << _hands[player_id].size() << "\n";
    }*/
  }

  virtual bool choose_pile(Card* face) {
    //create hypothetical hand
    Hand hyp_hand(*_hand);
    hyp_hand.add_card(face);
    hyp_hand.remove_card(hyp_hand.worst());

    if(hyp_hand.score() > _hand->score()) {
      return true;
    }
    return false;
  }

  virtual bool choose_knock(Card* face) {
    if (_hand->val_of_hand().second >= 30) {
      return true;
    } else {
      return false;
    }
  }

  virtual Card* choose_drop() {
    Card* drop_card = _hand->worst(); 
    return drop_card;
  }

};

class SecretivePlayer : public Player {
  virtual bool choose_pile(Card* face) {
    return false;
  }
  virtual string player_name() {return "secret-";}
};

class WittyPlayer : public Player {
protected:
  virtual string player_name() {return "witty--";}

  //safe to knock calculates whether it's a gaurantee that there is
  //some player with gaurantted *currently* less points on this turn.
  //This does NOT return true if you are gauranteed to win, only very
  //likely, b/c on the subsequent turn, the next player could win
  virtual bool safe_to_knock(int score) {
    //cout << _my_id << " " << _num_players << "\n";
    for(int player_id = 0; player_id < _num_players; ++player_id) {
      if(player_id == _my_id) {
        continue;
      }

      Hand& hand = _hands[player_id];

      int largest_score=0;
      for(int suit = 0; suit < 4; ++suit) {
        int num_unknown = 3 - hand.size();
        NumSortedSet topset = largestNCardsForSuit(_cards_left, num_unknown, suit);
        Hand fake_hand(hand);
        for(Card* card : topset) {
          fake_hand.add_card(card);
        }
        int fake_score = fake_hand.score();
        if(fake_score > largest_score) {
          largest_score = fake_score;
        }
      }
      if(score > largest_score) {
        return true;
      }
    }
    return false;
  }

  virtual bool choose_knock(Card* face) override {
    int score = _hand->score();
    if (safe_to_knock(score) || score==30) {
      return true;
    } else {
      return false;
    }
  }

  virtual bool choose_pile(Card* face) {
    //create hypothetical hand
    Hand hyp_hand(*_hand);
    hyp_hand.add_card(face);
    hyp_hand.remove_card(hyp_hand.worst());

    if((hyp_hand.score() > _hand->score() && hyp_hand.score()>=29) || 
       (hyp_hand.score() > _hand->score() + 4) ||
       (hyp_hand.score() > _hand->score() && face->num()>=10) ) {
      return true;
    }
    return false;
  }


};


class TimingPlayer : public WittyPlayer {
protected:

  virtual bool choose_knock(Card* face) override {
    auto pair = _hand->val_of_hand();
    int score = pair.second;
    if (safe_to_knock(score) || score >= 22 + 3*n_turns|| score == 30) {
      return true;
    } else {
      return false;
    }
  }
  virtual string player_name() {return "timing-";}
};

class HoldoutPlayer : public TimingPlayer {
protected: 
  virtual string player_name() {return "holdout";}

  virtual bool choose_pile(Card* face) override {
    Hand hyp_hand(*_hand);
    hyp_hand.add_card(face);
    hyp_hand.remove_card(hyp_hand.worst());

    if(n_turns<=5 && face->num() >= 10 && hyp_hand.score() <= 14
        && hyp_hand.score() -4 >= _hand->score()) {
      return true;
    } else {
      return TimingPlayer::choose_pile(face);
    }
  }
};


#endif
