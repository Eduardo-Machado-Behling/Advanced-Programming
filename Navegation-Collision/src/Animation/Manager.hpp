#ifndef ANIMATION_MANAGER
#define ANIMATION_MANAGER

#include <chrono>
#include <functional>

#include "Observer/Publisher.hpp"
#include "Observer/Subscriber.hpp"

template <class Clock> class AnimationManager {
public:
  using PlayFunc = std::function<bool(double)>;

  enum class PlayerState { INIT, PLAYING, PAUSED, ENDED };

  void start() {
    m_start = Clock::now();
	m_firstTick = true;
    state = PlayerState::PLAYING;
    onStateChanged.notifySubscribers();
  }

  void pause() {
    m_backTrack = std::chrono::duration<double>(Clock::now() - m_start).count();
    state = PlayerState::PAUSED;
    onStateChanged.notifySubscribers();
  }

  void reset() {
    m_start = Clock::now();
    state = PlayerState::PLAYING;
    onStateChanged.notifySubscribers();
  }

  void loop() {
    std::chrono::time_point<Clock> now = Clock::now();
    double dt = std::chrono::duration<double>(now - m_start).count();
	if(m_firstTick){
		dt = 0;
	}
	m_firstTick = false;

    if (play && state == PlayerState::PLAYING) {
      if (play(dt)) {
		  state = PlayerState::ENDED;
		  onStateChanged.notifySubscribers();
      }
    }
    m_start = now;
  }

  void setPlayFunction(PlayFunc func) { play = func; }

  void registerOnStateChanged(Subscriber *sub) {
    onStateChanged.subscribe(sub);
  }

  PlayerState getState() { return state; }

private:
  PlayFunc play;
  Publisher onStateChanged;
  std::chrono::time_point<Clock> m_start;
  double m_backTrack;
  bool m_firstTick = false;
  PlayerState state = PlayerState::INIT;
};

#endif
