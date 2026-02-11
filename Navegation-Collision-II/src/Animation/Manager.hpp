#ifndef ANIMATION_MANAGER
#define ANIMATION_MANAGER

#include <chrono>
#include <functional>

#include "Observer/Publisher.hpp"
#include "Observer/Subscriber.hpp"

template <class Clock> class AnimationManager {
public:
  using PlayFunc = std::function<bool(double, double)>;
  using IdleFunc = std::function<void()>;

  enum class PlayerState {
    INIT,
    PLAYING,
    PAUSED,
    ENDED,
  };

  void start() {
    if (!play)
      return;

    m_start = Clock::now();
    m_firstTick = true;
    state = PlayerState::PLAYING;
    play(0, 0);
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

  void loop(double dt) {
    if (state == PlayerState::PLAYING) {
      std::chrono::time_point<Clock> now = Clock::now();
      double elapsedTime = std::chrono::duration<double>(now - m_start).count();

      if (play && play(dt, elapsedTime)) {
        state = PlayerState::ENDED;
        onStateChanged.notifySubscribers();
      }
    } else if (idle) {
      idle();
    }
  }

  void setPlayFunction(PlayFunc func) { play = func; }
  void setIdleFunction(IdleFunc func) { idle = func; }

  void registerOnStateChanged(Subscriber *sub) {
    onStateChanged.subscribe(sub);
  }

  PlayerState getState() { return state; }

private:
  PlayFunc play;
  IdleFunc idle;

  Publisher onStateChanged;
  std::chrono::time_point<Clock> m_start;
  double m_backTrack;
  bool m_firstTick = false;
  PlayerState state = PlayerState::INIT;
};

#endif
