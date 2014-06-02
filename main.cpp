#include <stdio.h>
#include <iostream>
#include "boost/program_options.hpp"
#include <iomanip> 
#include "game.hh"


namespace po = boost::program_options;

static void addPlayer(Player* p, Game& game,
                                 std::vector<Player*>& players) {
    players.push_back(p);
    game.addPlayer(p);
}

int main(int argc, char** argv) {
  int ngames=1;
  bool print_game=false;

  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("print_game", "Print Game Output")
      ("rand_inputs", "Randomize the inputs")
      ("games", po::value<int>(), "set number of games")
  ;
  
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  
  if (vm.count("help")) {
      cout << desc << "\n";
      return 1;
  }
  
  if(vm.count("print_game")) {
    print_game=true;
  }

  if(vm.count("rand_inputs")) {
    std::srand(std::time(0));
  }

  if (vm.count("games")) {
    ngames=vm["games"].as<int>();
    cout << "Running" << ngames  << "\n";
  } else {
    cout << "Running Game.\n";
  }

  Game game;
  std::map<Player*,int> losses;
  std::vector<Player*> players;

  //for(int i = 0; i < 2; ++i) {
  //  addPlayer(new Player(),game,players);
  //}
  addPlayer(new TimingPlayer(),game,players);
  addPlayer(new WittyPlayer(),game,players);
  addPlayer(new SecretivePlayer(),game,players);
  //addPlayer(new TimingPlayer(),game,players);
  addPlayer(new HoldoutPlayer(),game,players);

  for(int i = 0; i < ngames; ++i) {
    game.shufflePlayers();
    std::set<Player*> losers = game.play(print_game);
    for(Player* loser : losers) {
      losses[loser]+=1;
      if(print_game) {
        cout << loser->player_name() << " " << loser->id() << " lost\n";
      }
    }
  }

  for(Player* p : players) {
    //cout << p->player_name() << " lost " << losses[p] << " games\n";
    cout << p->player_name() << " lost " << std::setprecision(5) <<
(double)losses[p]/(double)ngames << " games\n";
  }
}

