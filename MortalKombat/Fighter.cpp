#include "Fighter.h"
#include "AnimationFilmHolder.h"
#include "AnimatorHolder.h"

Fighter::Fighter(string Name, Point position) {
	name = Name;
	tickAnimator = new TickTimerAnimator(NULL);
	FighterPos = position;
};

bool Fighter::initialize(const string& path) {
	try {

		json config;
		std::ifstream config_file(path, std::ifstream::binary);
		input::key_combination keycodes;
		config_file >> config;
		for (json::iterator it = config["combinations"].begin(); it != config["combinations"].end(); it++) {
			keycodes.clear();
			int i = 0;
			json tmp = *it;
			string action = tmp["action"];
			for (json::iterator i = tmp["key_strokes"].begin(); i != tmp["key_strokes"].end(); i++) {
				keycodes.push_back(*i);
			}
			Fighter::inputController.AddAction(keycodes, action);
		}

		stateTransitions.SetState("READY");
		setStateMachine(path._Equal("config/subzero_controller.json"));//for debugging


		return true;//true or false catch here
	}
	catch (const std::exception& e) {
		cerr << e.what();
		return false;

	}
};

void Fighter::Draw(SDL_Surface& gScreenSurface, string test, int w, int h) {

	/*
	* TEMP CODE HERE
	*/
	AnimationFilm* tmp = AnimationFilmHolder::Get()->GetFilm(test);
	//{0 , 0 } coordinates

	tmp->DisplayFrame(gScreenSurface, FighterPos, 4, w, h);//100 x100 is the size of the player
	using Input = logic::StateTransitions::Input;
	/*
	if (tickAnimator&&tickAnimator->GetState() != ANIMATOR_RUNNING) {
			TickTimerAnimation* tmp2 = new TickTimerAnimation(10);
			tmp2->setOnTick([] {
				//Nothing to do here
			}).SetDelay(1000).SetReps(1);
			tickAnimator = new TickTimerAnimator(tmp2);
			tickAnimator->SetOnFinish([&]() {
				AnimatorHolder::Remove(tickAnimator);
				inputController.GetLogical();
			});
				tickAnimator->Start(SDL_GetTicks());
			AnimatorHolder::MarkAsRunning(tickAnimator);
		}
	*/
	if (test._Equal("subzero.stance")) {//debug
		Input tmpInput;
		tmpInput.insert(Make_key(inputController.GetLogical()));
		Fighter::stateTransitions.PerformTransitions(tmpInput, false);//Investigate this flag how works
	}

}
/*
* Watchout in which time you want to make the transition before or after the act like 1st print and then change state ?
or the opposite
*/
void Fighter::setStateMachine(bool debug) {
	using Input = logic::StateTransitions::Input;
	stateTransitions.
		/*
		*  PUNCHES-> LOW/HIGH AND PUNCH
		*/
		SetTransition("READY", Input{ ".PUNCH" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Punch-> State " << stateTransitions.GetState() << "\n";
		});
	})
		.SetTransition("DOWN", Input{ ".DOWN.PUNCH" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Low Punch-> State " << stateTransitions.GetState() << "\n";
		});

	})
		/* IGNORE THIS FOR NOW
		.SetTransition("DOWN", Input{ ".DOWN.SPECIAL" }, [&](void) {
		//SetActionWithAnimator([&]() {
		//	AnimatorHolder::Remove(tickAnimator);
		if (debug) cout << "TEST------blur------> State " << stateTransitions.GetState() << "\n";
		//});

	})
		.SetTransition("DOWN", Input{ ".DOWN.SPECIAL.BCK" }, [&](void) {
		//SetActionWithAnimator([&]() {
		//	AnimatorHolder::Remove(tickAnimator);
		if (debug) cout << "TEST------blur------> State " << stateTransitions.GetState() << "\n";
		//});

	})*/
		.SetTransition("UP", Input{ ".PUNCH.UP" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "High Punch-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("READY");//do the animation and the fall down here cant stay at air forever
		});
	})
		/*
		* KICKS-> LOW/HIGH AND KICK
		*/
		.SetTransition("READY", Input{ ".KICK" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Kick-> State " << stateTransitions.GetState() << "\n";
		});
	})
		.SetTransition("DOWN", Input{ ".DOWN.KICK" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Low KICK-> State " << stateTransitions.GetState() << "\n";
		});

	})
		.SetTransition("UP", Input{ ".KICK.UP" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "High Kick-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("READY");//do the animation and the fall down here cant stay at air forever
		});

	})
		/*
		* MOVES-> UP/BACK/FORWARD/DOWN
		*/
		.SetTransition("READY", Input{ ".UP" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Up-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("UP");
		});
	})
		.SetTransition("READY", Input{ ".BCK" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Back-> State " << stateTransitions.GetState() << "\n";;
		});
	})
		.SetTransition("READY", Input{ ".FWD" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Forward-> State " << stateTransitions.GetState() << "\n";
		});
	})
		.SetTransition("READY", Input{ ".DOWN" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "DUCKING-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("DOWN");
		});
	})
		.SetTransition("DOWN", Input{ ".DOWN" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "still ducking-> State " << stateTransitions.GetState() << "\n";
		});
	})
		/*
		* BLOCKS->BLOCK/BLOCK DOWN
		*/
		.SetTransition("READY", Input{ ".BLOCK" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Block -> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("BLOCK");
		});
	})
		.SetTransition("DOWN", Input{ ".BLOCK.DOWN" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Block Down -> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("BLOCKDWN");
		});
	})
		.SetTransition("BLOCK", Input{ ".BLOCK" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "still blocking-> State " << stateTransitions.GetState() << "\n";
		});
	})
		.SetTransition("BLOCKDWN", Input{ ".BLOCK.DOWN" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "still blocking down-> State " << stateTransitions.GetState() << "\n";
		});
	})
		/*
		* DEFAULT TRANSITIONS
		*/
		.SetTransition("DOWN", Input{}, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug)	cout << "Getting Up-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("READY");
		});
	})
		.SetTransition("UP", Input{}, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug)	cout << "Falling Down-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("READY");
		});
	})
		.SetTransition("BLOCK", Input{}, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Unblocking-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("READY");
		});
	})
		.SetTransition("BLOCKDWN", Input{}, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Unblocking DOWN-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("DOWN");
		});
	})
		.SetTransition("READY", Input{}, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Ready waiting -> State " << stateTransitions.GetState() << "\n";
		});
	})
		/*
		* COMBOS
		*/
		.SetTransition("UP", Input{ ".FWD.UP" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "FlipFWD-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("FlipFWD");
		});
	})
		.SetTransition("UP", Input{ ".BCK.UP" }, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "FlipBCK-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("FlipBCK");
		});
	})
		.SetTransition("FlipFWD", Input{}, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Falling from FlipFWD-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("READY");
		});
	})
		.SetTransition("FlipBCK", Input{}, [&](void) {
		SetActionWithAnimator([&]() {
			AnimatorHolder::Remove(tickAnimator);
			if (debug) cout << "Falling from flipBCK-> State " << stateTransitions.GetState() << "\n";
			stateTransitions.SetState("READY");
		});
	});
};

void Fighter::Handler() {
	inputController.Handle();
};


void Fighter::SetActionWithAnimator(const std::function<void()>& f) {
	if (tickAnimator&&tickAnimator->GetState() != ANIMATOR_RUNNING) {
		TickTimerAnimation* tmp2 = new TickTimerAnimation(10);
		tmp2->setOnTick([] {
			//Nothing to do here
		}).SetDelay(FIGHTER_ACTION_DELAY_MSECS).SetReps(1);
		tickAnimator = new TickTimerAnimator(tmp2);
		tickAnimator->SetOnFinish(f);
		tickAnimator->Start(SDL_GetTicks());
		AnimatorHolder::MarkAsRunning(tickAnimator);
	}
};

const string Fighter::Make_key(const Input& input) const {
	std::string result;
	for (auto& s : input)
		result += "." + s;
	return result;
}

