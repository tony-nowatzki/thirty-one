#ifndef GAME_HH
#define GAME_HH

#include <iostream>
#include <vector>
#include <map>
#include <deque>
#include <string>
#include <assert.h>
#include <set>
#include <algorithm>
#include "player.hh"
#include "card.hh"

using namespace std;

class Game {
  bool over=false;

  //At any time, cards are either in the deck or pile.
  //They must be deleted when the game is over
  set<Card*> _cards;
  deque<Card*> _deck;
  deque<Card*> _pile;

  vector<Player*> _players;
  map<Player*,Hand*> _hands;

public:
  Game() {
    createCards();
  }

  void addPlayer(Player* player) {
    _players.push_back(player);
    _hands[player]=new Hand();
  }

  void initialize_hands(deque<Card*>& deck) {
    for(auto iter : _hands) {
      Hand* hand = iter.second;
      hand->reset();
      for(int i = 0; i < 3; ++i) {
        hand->add_card(deck.back());
        deck.pop_back();
      }
    }
  }

  void shufflePlayers() {
    random_shuffle(_players.begin(), _players.end());
  }

  void createCards() {
    for(int i =1; i <= 13; i++) {
      for(int j=0; j< 4; ++j) {
        Card* card = new Card(j,i);
        _cards.insert(card);
      }
    }
  }


  void reset() {
    _pile.clear();
    _deck.clear();

    for(Card* card : _cards) {
      _deck.push_back(card);
    } 
    //copy cards to deck, then shuffle TODO
    random_shuffle(_deck.begin(), _deck.end());
    initialize_hands(_deck);

    for(int i = 0; i < _players.size(); ++i) {
      _players[i]->initialize(_cards,_hands[_players[i]],i,_players.size());
      _players[i]->reset();
    }

    //Pop the last card
    _pile.push_back(_deck.back());
    _deck.pop_back();
  }
  
  std::set<Player*> losers(int knock_id) {
    Player* knock_player = _players[knock_id];
    assert(knock_player);

    set<Player*> lowest_players;
    int lowest_hand=1000;

    //_players with lowest score lose
    for(Player* p : _players) {
      int score = _hands[p]->score();
      if(score < lowest_hand) {
        lowest_players.clear();
        lowest_hand=score;
      }
      if(score <= lowest_hand) {
        lowest_players.insert(p);
      }
    }
    if(knock_id!=-1) {
      //if knocker is tied, he is the loser
      for(Player* p : lowest_players) {
        if(p == knock_player) {
          std::set<Player*> knocker;
          knocker.insert(p);
          return knocker;
        }
      }
    }
    return lowest_players;
  }


  std::set<Player*> play(bool print_game) {
    reset();
    int player_id=-1;
    int knock_id=-1;

    while(true) {
      if(++player_id == _players.size()) {
        player_id = 0;
      }
      Player* player = _players[player_id];

      if(player_id == knock_id || _deck.size()==0) {
        return losers(knock_id);
      }

      bool knock=false,stack=false,pick_pile=false;
      Card* pile_card=NULL, *drop_card=NULL;
      Card* deck_card=_deck.back();

      //cout << "later " << player_id << " " << player->player_name() << " " << knock_id << " \n";


      if(knock_id == -1 && player->choose_knock(_pile.back())) {
        knock=true;
        knock_id=player_id;
      } else {
        //first choose stack or pile
        if(!player->choose_pile(_pile.back())) {
          stack=true;
          _hands[player]->add_card(_deck.back());
          _deck.pop_back();
        } else {
          pick_pile=true;
          pile_card = _pile.back();
          _hands[player]->add_card(pile_card);
          _pile.pop_back();
        }
        //now choose drop card
        drop_card = player->choose_drop();
        assert(drop_card);
        _pile.push_back(drop_card);
        _hands[player]->remove_card(drop_card);
      }

      for(Player* p : _players) {
        p->inform_turn(player_id,knock,stack,
                      pick_pile,pile_card,drop_card);
      }

      if(print_game) {
        cout << player->player_name() << player_id << " ";
        if(knock) {
          cout << "knocked\n";
        } else {
          if(stack) {
            cout << "drew " << deck_card->name();
          } else if(pick_pile) {
            cout << "pick " << pile_card->name();
          }
          cout << " drop " << drop_card->name() << ";";
          cout << "(" << _hands[player]->print_hand() << ")\n"; 
        }
      }

      //check if player got 31
      if(_hands[player]->score() == 31) {
        std::set<Player*> losers;
        for(Player* p : _players) {
          losers.insert(p);
        }
        losers.erase(player);
        return losers;
      } 

    }
  }

};
#endif
